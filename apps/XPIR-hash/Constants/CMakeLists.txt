cmake_minimum_required(VERSION 2.6.0)

add_custom_target(build-time-make-directory-XPIR-hash-Constants ALL COMMAND ${CMAKE_COMMAND} -E make_directory "Constants")

file(COPY "paramsPIR.txt" DESTINATION "Constants/.")
file(COPY "paramsSHA.txt" DESTINATION "Constants/.")

add_executable(changeParamsPIR changeParamsPIR.cpp)
add_executable(changeParamsSHA changeParamsSHA.cpp)