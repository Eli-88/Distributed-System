file(GLOB_RECURSE PAXOS_SRC_FILES "./*.cpp")


add_executable(paxos)
target_sources(paxos PRIVATE ${PAXOS_SRC_FILES})
target_include_directories(paxos PRIVATE ${CMAKE_SOURCE_DIR}/paxos)
target_link_libraries(paxos PRIVATE ${COMMON_LIBS})