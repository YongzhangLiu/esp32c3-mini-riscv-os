#include "timer.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"
#include "soc/systimer_reg.h"
#include "soc/systimer_struct.h"
#include "esp_private/systimer.h"
#include "riscv.h"
#include "soc/system_struct.h"
#include "riscv/interrupt.h"
#include "riscv/rv_utils.h"

#define ESP_INTR_TYPE_LEVEL 0
#define ESP_INTR_TYPE_EDGE 1

// TODO: check if this is correct
#define OS_TICK_HZ 1

#define SYSTIMER_INTR_SRC_ID 37
#define SYSTIMER_INTR_CPU_INT_NUM 1

static systimer_hal_context_t systimer_hal;
static uint32_t alarm_id = SYSTIMER_ALARM_OS_TICK_CORE0;

static inline __attribute__((always_inline)) void interrupt_intc_ll_route(int intr_src, int intr_num)
{
    REG_WRITE(DR_REG_INTERRUPT_BASE + 4 * intr_src, intr_num);
}

void timer_init()
{
    /* enable bus clock */
    systimer_ll_enable_clock(systimer_hal.dev, true);

    /* initialize systimer hal */
    systimer_hal_init(&systimer_hal);
    systimer_hal_tick_rate_ops_t ops = {
        .ticks_to_us = systimer_ticks_to_us,
        .us_to_ticks = systimer_us_to_ticks,
    };

    /* set tick rate ops */
    systimer_hal_set_tick_rate_ops(&systimer_hal, &ops);

    /* set counter value */
    systimer_ll_set_counter_value(systimer_hal.dev, SYSTIMER_COUNTER_OS_TICK, 0);
    systimer_ll_apply_counter_value(systimer_hal.dev, SYSTIMER_COUNTER_OS_TICK);

    /* configure the timer */
    systimer_hal_connect_alarm_counter(&systimer_hal, alarm_id, SYSTIMER_COUNTER_OS_TICK);

    /* set alarm period to be (1000000 / OS_TICK_HZ) us */
    systimer_hal_set_alarm_period(&systimer_hal, alarm_id, 1000000UL / OS_TICK_HZ);

    /* set alarm mode to be period */
    systimer_hal_select_alarm_mode(&systimer_hal, alarm_id, SYSTIMER_ALARM_MODE_PERIOD);

    /* enable counter can be stalled by cpu */
    systimer_hal_counter_can_stall_by_cpu(&systimer_hal, SYSTIMER_COUNTER_OS_TICK, 0, true);

    /* enable alarm interrupt */
    systimer_hal_enable_alarm_int(&systimer_hal, alarm_id);

    /* enable counter */
    systimer_hal_enable_counter(&systimer_hal, SYSTIMER_COUNTER_OS_TICK);

    /* map timer interrupt to cpu interrupt line 31 */
    REG_WRITE(DR_REG_INTERRUPT_BASE + 4 * SYSTIMER_INTR_SRC_ID, SYSTIMER_INTR_CPU_INT_NUM);

    /* enable cpu interrupt line 31 */
    rv_utils_intr_enable(1 << SYSTIMER_INTR_CPU_INT_NUM);
    esprv_intc_int_set_priority(SYSTIMER_INTR_CPU_INT_NUM, 1);

    /* set interrupt type to be level */
    esprv_intc_int_set_type(SYSTIMER_INTR_CPU_INT_NUM, ESP_INTR_TYPE_LEVEL);

    /* enable cpu interrupt */
    RV_SET_CSR(mstatus, MSTATUS_MIE);
    asm volatile("fence\n"); // ensure MIE is visible
}

void timer_handler(void)
{
    /* clear alarm interrupt */
    systimer_ll_clear_alarm_int(systimer_hal.dev, alarm_id);

    /* print current time */
    lib_printf("timer interrupt: cur time: %ld\r\n", (uint32_t)systimer_hal_get_time(&systimer_hal, SYSTIMER_COUNTER_OS_TICK));
}
