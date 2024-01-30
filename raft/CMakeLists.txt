file(GLOB_RECURSE RAFT_SRC_FILES "./*.cpp")
file(GLOB_RECURSE RAFT_UTIL_SRC_FILES "util/*.cpp")

set(RAFT_SRC_FILES 
    ${RAFT_SRC_FILES} 
    ${RAFT_UTIL_SRC_FILES})

add_executable(raft)
target_sources(raft PRIVATE ${RAFT_SRC_FILES})
target_include_directories(raft PRIVATE ${CMAKE_SOURCE_DIR}/raft)
target_link_libraries(raft PRIVATE ${COMMON_LIBS})