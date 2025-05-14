enable_language(C ASM)

list(APPEND srcs
    "start.S"
    "vectors.S"
    "syscalls.c"
    "panic.c"
    "interrupt.c"
    )

add_library(common STATIC ${srcs})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(common PROPERTIES C_STANDARD 11)
target_compile_options(common PRIVATE -g -Og)
target_link_libraries(common INTERFACE -nostartfiles)
