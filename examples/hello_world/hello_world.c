#include <stdio.h>
#include "hal/wdt_hal.h"
#include "soc/rtc_cntl_reg.h"

static void delay(void);
static void super_wdt_auto_feed(void);
static void super_wdt_reset_config(bool);

int main(void)
{
    // Super WDT is still enabled; no HAL API for it yet
    super_wdt_reset_config(true);
    super_wdt_auto_feed();

    // Disable the watchdogs
    wdt_hal_context_t mwdt_ctx = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&mwdt_ctx);
    wdt_hal_disable(&mwdt_ctx);
    wdt_hal_set_flashboot_en(&mwdt_ctx, false);
    wdt_hal_context_t rwdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rwdt_ctx);
    wdt_hal_disable(&rwdt_ctx);
    wdt_hal_set_flashboot_en(&rwdt_ctx, false);

    while(1) {
        printf("Hello, world!\r\n");
        delay();
    }
    return 0;
}

static void delay(void)
{
    for (int i = 0; i < 300000; i++) {
        asm volatile ("nop");
    }
}

static void __unused super_wdt_reset_config(bool enable)
{
    REG_CLR_BIT(RTC_CNTL_FIB_SEL_REG, RTC_CNTL_FIB_SUPER_WDT_RST);

    if (enable) {
        REG_CLR_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_BYPASS_RST);
    } else {
        REG_SET_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_BYPASS_RST);
    }
}

static void __unused super_wdt_auto_feed(void)
{
    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, RTC_CNTL_SWD_WKEY_VALUE);
    REG_SET_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_AUTO_FEED_EN);
    REG_WRITE(RTC_CNTL_SWD_WPROTECT_REG, 0);
}
