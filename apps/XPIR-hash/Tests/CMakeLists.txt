cmake_minimum_required(VERSION 2.6.0)

file(COPY "simple_test.sh" DESTINATION ".")

add_executable(changeParamsPIR ../Constants/changeParamsPIR.cpp)
add_executable(changeParamsSHA ../Constants/changeParamsSHA.cpp)
add_executable(checkExecution checkExecution.cpp)
