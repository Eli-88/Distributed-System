cmake_minimum_required(VERSION 3.22)

project(DistributedSystem)


set(CMAKE_CXX_STANDARD 20)

find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost 1.82 REQUIRED COMPONENTS 
    system
    program_options)


set(COMMON_LIBS 
    spdlog::spdlog
    fmt::fmt
    nlohmann_json::nlohmann_json
    Boost::system
    Boost::program_options)


set(COMPILE_OPTIONS
    -Wall
    -Wextra
    -pedantic
    -Werror)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(COMMON_LIBS
            ${COMMON_LIBS}
            asan)

        set(COMPILE_OPTIONS
            ${COMPILE_OPTIONS}
            -fomit-frame-pointer
            -fsanitize=address)
    endif()
endif()


add_subdirectory(example)

add_compile_options(${COMPILE_OPTIONS})
add_subdirectory(paxos)
add_subdirectory(tools/load_test)