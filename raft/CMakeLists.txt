add_executable(raft main.cpp)

SET(RAFT_SRC_FILES 
    leader.cpp
    util/tcp_socket.cpp
    util/tcp_client.cpp
    util/tcp_server.cpp
    util/event_loop.cpp
    util/tcp_stream.cpp
    vote_manager.cpp
    raft.cpp
    message_decoder.cpp
    follower.cpp
    raft_context.cpp
    vote.cpp
    candidate.cpp
    message_encoder.cpp
    core.cpp
    in_memory_log_entry.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET(RAFT_SRC_FILES ${RAFT_SRC_FILES} util/selector_linux.cpp)
endif()

target_sources(raft PRIVATE ${RAFT_SRC_FILES})

target_include_directories(raft PRIVATE ${CMAKE_SOURCE_DIR}/raft)
target_link_libraries(raft PRIVATE ${COMMON_LIBS})