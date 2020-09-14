include_directories(${PROJECT_SOURCE_DIR}/lib/)

add_executable(echoclient_udp echoclient_udp.c)
add_executable(echoserver_udp echoserver_udp.c)