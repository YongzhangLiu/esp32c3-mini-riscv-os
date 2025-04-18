cmake_minimum_required(VERSION 3.12)

set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../)
set(CMAKE_TOOLCHAIN_FILE ${ROOT_DIR}/toolchain-rv32.cmake)

# Set assembly compilation rule
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")

project(mini_os)

add_executable(mini_os
    sys.s
    lib.c
    os.c
    main.c
)
# include dirs
add_subdirectory(${ROOT_DIR}/common common)
add_subdirectory(${ROOT_DIR}/hal hal)

# link dirs
target_link_libraries(mini_os PRIVATE common hal)

target_compile_options(mini_os PRIVATE -g -Og)
target_link_options(mini_os PRIVATE -g)

target_compile_options(mini_os PRIVATE -Wall -Werror -Wextra)

# 引入自定义 CMake 函数
include(${ROOT_DIR}/utils.cmake)

add_linker_scripts(mini_os)
add_map_file(mini_os mini_os.map)
make_binary(mini_os mini_os.bin)
