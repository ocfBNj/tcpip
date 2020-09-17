cmake_minimum_required(VERSION 3.16.3)
project(tcpip)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(_GNU_SOURCE)

include_directories(${PROJECT_SOURCE_DIR}/lib/)
add_subdirectory(lib)

link_directories(${PROJECT_BINARY_DIR}/lib/)
link_libraries(common pthread)

add_subdirectory(select)
add_subdirectory(poll)
add_subdirectory(udp)
add_subdirectory(time)
add_subdirectory(http)

add_executable(main main.c)
