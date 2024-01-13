function(add_example_executable TARGET SRC_FILE)
    add_executable(${TARGET})
    target_sources(${TARGET} PRIVATE ${SRC_FILE})
    target_link_libraries(${TARGET} PRIVATE ${COMMON_LIBS})
endfunction()


add_example_executable(hello_world hello_world.cpp)
add_example_executable(asio_demo asio_demo.cpp)
add_example_executable(json_demo json_demo.cpp)
add_example_executable(asio_coroutine asio_coroutine.cpp)
add_example_executable(beast_demo beast_demo.cpp)
