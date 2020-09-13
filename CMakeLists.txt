cmake_minimum_required(VERSION 3.16.3)
project(tcpip)

add_subdirectory(include)

link_directories(${PROJECT_BINARY_DIR}/include/)
link_libraries(common)

add_subdirectory(select)
add_subdirectory(poll)
add_subdirectory(udp)
