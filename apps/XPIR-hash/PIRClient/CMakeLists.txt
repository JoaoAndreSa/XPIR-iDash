cmake_minimum_required(VERSION 2.6.0)

include_directories(../../..)

add_custom_target(build-time-make-directory-XPIR-hash-PIR-client-logs ALL COMMAND ${CMAKE_COMMAND} -E make_directory "logs")
add_custom_target(build-time-make-directory-XPIR-hash-PIR-client-data ALL COMMAND ${CMAKE_COMMAND} -E make_directory "data")
add_custom_target(build-time-make-directory-XPIR-hash-PIR-client-data-nonces ALL COMMAND ${CMAKE_COMMAND} -E make_directory "data/nonces")
file(COPY "vcf" DESTINATION ".")

add_executable(client AES256/AES_ctr_256.cpp SHA256/SHA_256.cpp ../Constants/constants.cpp ../Socket/Socket.cpp ../Tools/Tools.cpp PIRClient.cpp Pipeline/PIRClientPipeline.cpp Sequential/PIRClientSequential.cpp ../XPIR/XPIRc.cpp ../XPIR/Pipeline/XPIRcPipeline.cpp ../XPIR/Sequential/XPIRcSequential.cpp client.cpp)

target_link_libraries(client ssl crypto pir ${MPFR_LIBRARIES} ${Boost_LIBRARIES} ${GMP_LIBRARIES} ${GMPXX_LIBRARIES} nfllib)
