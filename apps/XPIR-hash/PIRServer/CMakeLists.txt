cmake_minimum_required(VERSION 2.6.0)

include_directories(../../..)

add_custom_target(build-time-make-directory-XPIR-hash-PIR-server-db ALL COMMAND ${CMAKE_COMMAND} -E make_directory "db")

add_executable(server ../Constants/constants.cpp ../Socket/Socket.cpp ../Tools/Tools.cpp PIRServer.cpp Pipeline/PIRServerPipeline.cpp Sequential/PIRServerSequential.cpp ../XPIR/XPIRc.cpp ../XPIR/Pipeline/XPIRcPipeline.cpp ../XPIR/Sequential/XPIRcSequential.cpp server.cpp)

target_link_libraries(server pir pthread ${MPFR_LIBRARIES} ${Boost_LIBRARIES} ${GMP_LIBRARIES} ${GMPXX_LIBRARIES} nfllib)
