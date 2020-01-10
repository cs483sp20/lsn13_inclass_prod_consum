cmake_minimum_required(VERSION 3.3)
project(Lsn14_Example_prod_con)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES main.c)
add_executable(prod_con ${SOURCE_FILES})
target_link_libraries(prod_con pthread)