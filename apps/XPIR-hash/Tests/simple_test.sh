#!/bin/bash

#    XPIR-hash
#    simple_test.sh
#    Purpose: Run a simple batch of Constants (multiple runs)
#
#    @author Joao Sa
#    @version 1.0 01/07/16

#####-------- SIMPLE TEST BASH --------#####
#####-------- SETUP --------#####
cd ..
cd Constants
./changeParamsSHA 22
./changeParamsPIR 3 256 27 27 27 0 LWE:97:1024:60

cd ..
cd PIRClient
rm -rf logs/*
rm -rf data/*
rm -rf output.txt

#START SERVER
lxterminal --working-directory=Desktop/XPIR/_build/apps/XPIR-hash/PIRServer/ -e "./server"
sleep 1

#START CLIENT
./client -f vcf/													>> logs/log_setup.txt 

#####-------- 1,000 ENTRIES --------#####
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_1000.vcf 		>> logs/log_1000.txt 	#in db
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf 	>> logs/log_1000.txt 	#in db
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_1000.vcf 		>> logs/log_1000.txt 	#not in db
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf 	>> logs/log_1000.txt 	#not in db


#####-------- 10,000 ENTRIES --------#####
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf 			>> logs/log_10000.txt 	#in db
./client -c 1 -p 164781110 -r . -a ATATAAG -f RCV000015246_10000.vcf 	>> logs/log_10000.txt  	#in db
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_10000.vcf 			>> logs/log_10000.txt  	#not in db
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_10000.vcf 		>> logs/log_10000.txt  	#not in db


######-------- 100,000 ENTRIES --------#####
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf 	>> logs/log_100000.txt 	#in db
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_100000.vcf 		>> logs/log_100000.txt  #in db
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_100000.vcf 		>> logs/log_100000.txt 	#not in db
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf 	>> logs/log_100000.txt  #not in db

######-------- 5,000,000 ENTRIES --------#####
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_5000000.vcf 	>> logs/log_100000.txt 	#in db
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_5000000.vcf 		>> logs/log_100000.txt  #in db
./client -c 21 -p 161617087 -r T -a . -f RCV000015246_5000000.vcf 		>> logs/log_100000.txt 	#not in db
./client -c 21 -p 160955085 -r CTA -a TTG -f RCV000015246_5000000.vcf 	>> logs/log_100000.txt  #not in db

cd ..
cd Tests
./checkExecution
