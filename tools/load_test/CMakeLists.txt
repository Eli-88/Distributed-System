function(add_load_test_executable TARGET SRC_FILES)
    add_executable(${TARGET} ${SRC_FILES})
    target_link_libraries(${TARGET} PRIVATE ${COMMON_LIBS})
endfunction()


add_load_test_executable(paxos_loadtest paxos_load_test.cpp)
