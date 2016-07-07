cmake_minimum_required(VERSION 2.6.0)

add_subdirectory("PIRClient")
add_subdirectory("PIRServer")
add_subdirectory("Tests")

add_custom_target(build-time-make-directory-XPIR-hash-Constants ALL COMMAND ${CMAKE_COMMAND} -E make_directory "Constants")

file(COPY "Constants/paramsPIR.txt" DESTINATION "Constants/")
file(COPY "Constants/paramsSHA.txt" DESTINATION "Constants/")