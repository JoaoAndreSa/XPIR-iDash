cmake_minimum_required(VERSION 2.6.0)

file(COPY "paramsPIR.txt" DESTINATION ".")
file(COPY "paramsSHA.txt" DESTINATION ".")

add_executable(changeParamsPIR changeParamsPIR.cpp)
add_executable(changeParamsSHA changeParamsSHA.cpp)