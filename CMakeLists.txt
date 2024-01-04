cmake_minimum_required(VERSION 3.18)
project(POSGameOfLifeServer C)

set(CMAKE_C_STANDARD 99)

add_executable(POSGameOfLifeServer main.c pos_sockets/passive_socket.c pos_sockets/char_buffer.c pos_sockets/active_socket.c)
target_link_libraries(POSGameOfLifeServer pthread.h)
