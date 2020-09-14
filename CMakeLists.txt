cmake_minimum_required(VERSION 3.16.3)
project(tcpip)

add_subdirectory(lib)

link_directories(${PROJECT_BINARY_DIR}/lib/)
link_libraries(common)

add_subdirectory(select)
add_subdirectory(poll)
add_subdirectory(udp)

add_executable(main main.c)
