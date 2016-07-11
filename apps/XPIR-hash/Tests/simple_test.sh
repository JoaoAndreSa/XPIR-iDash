#!/bin/bash

#    XPIR-hash
#    simple_test.sh
#    Purpose: Run a simple batch of Constants (multiple runs)
#
#    @author Joao Sa
#    @version 1.0 01/07/16

#####-------- SIMPLE TEST BASH --------#####
#START SERVER
lxterminal --working-directory=Desktop/XPIR/_build/apps/XPIR-hash/PIRServer/ -e "./server"
sleep 1

#START CLIENT
#####-------- 1,000 ENTRIES --------#####
cd ..
cd PIRClient
rm -rf logs/log_1000.txt
rm -rf output.txt

cd ..
cd Constants
./changeParamsSHA 10

# ///////////////////--------PACKING (30)---------////////////////////////////////
./changeParamsPIR 1 30 35 0 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_1000.vcf 		>> logs/log_1000.txt 	#in db
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf 	>> logs/log_1000.txt 	#in db
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_1000.vcf 		>> logs/log_1000.txt 	#not in db
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf 	>> logs/log_1000.txt 	#not in db

# ///////////////////--------PACKING & DIM (15,9x8)---------////////////////////////////////
cd ..
cd Constants
./changeParamsPIR 2 15 9 8 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_1000.vcf 		>> logs/log_1000.txt 	#in db
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf 	>> logs/log_1000.txt 	#in db
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_1000.vcf 		>> logs/log_1000.txt 	#not in db
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf 	>> logs/log_1000.txt 	#not in db







#####-------- 10,000 ENTRIES --------#####
rm -rf logs/log_10000.txt

cd ..
cd Constants
./changeParamsSHA 13

# ///////////////////--------PACKING (217)---------////////////////////////////////
./changeParamsPIR 1 217 38 0 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf 			>> logs/log_10000.txt 	#in db
./client -c 1 -p 164781110 -r ATATAAG -a . -f RCV000015246_10000.vcf 	>> logs/log_10000.txt  	#in db
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_10000.vcf 			>> logs/log_10000.txt  	#not in db
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_10000.vcf 		>> logs/log_10000.txt  	#not in db

# ///////////////////--------PACKING & DIM (15,24x23)---------////////////////////////////////
cd ..
cd Constants
./changeParamsPIR 2 15 24 23 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf 			>> logs/log_10000.txt 	#in db
./client -c 1 -p 164781110 -r ATATAAG -a . -f RCV000015246_10000.vcf 	>> logs/log_10000.txt  	#in db
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_10000.vcf 			>> logs/log_10000.txt  	#not in db
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_10000.vcf 		>> logs/log_10000.txt  	#not in db







######-------- 100,000 ENTRIES --------#####
rm -rf logs/log_100000.txt

cd ..
cd Constants
./changeParamsSHA 16

# ///////////////////--------PACKING (1643)---------////////////////////////////////
./changeParamsPIR 1 1643 40 0 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf 	>> logs/log_100000.txt 	#in db
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_100000.vcf 		>> logs/log_100000.txt  #in db
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_100000.vcf 		>> logs/log_100000.txt  #not in db
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf 	>> logs/log_100000.txt  #not in db

# ///////////////////--------PACKING & DIM (28,48x49)---------////////////////////////////////
cd ..
cd Constants
./changeParamsPIR 2 28 49 48 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf 	>> logs/log_100000.txt 	#in db
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_100000.vcf 		>> logs/log_100000.txt  #in db
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_100000.vcf 		>> logs/log_100000.txt 	#not in db
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf 	>> logs/log_100000.txt  #not in db

cd ..
cd PIRServer
rm -rf db/*

cd ..
cd Tests
./checkExecution
