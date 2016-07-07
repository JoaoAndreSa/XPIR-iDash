cmake_minimum_required(VERSION 2.6.0)

file(COPY "simple_test.sh" DESTINATION ".")

add_executable(checkExecution checkExecution.cpp)
