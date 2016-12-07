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
#./changeParamsSHA 13
#./changeParamsPIR 2 3 53 52 0 0 LWE:97:1024:60
./changeParamsSHA 20
./changeParamsPIR 3 4 65 64 64 0 LWE:97:1024:60

cd ..
cd PIRServer
rm -rf db/*

cd ..
cd PIRClient
rm -rf logs/*
rm -rf output.txt

#START SERVER
read -p "START THE SERVER (./server). Press any key to continue... " -n1 -s

#START CLIENT
./client -f vcf/

#####-------- 1,000 ENTRIES --------#####
#./client -c 1 -p 161617087 -r T -a . -f RCV000015246_1000.vcf
#./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf
#./client -c 1 -p 160999478 -r A -a G -f RCV000015246_1000.vcf
#./client -c 1 -p 161000239 -r . -a TAAA -f RCV000015246_1000.vcf
#./client -c 1 -p 161005671 -r TGGA -a ACCAACAT -f RCV000015246_1000.vcf
#./client -c 1 -p 161645470 -r A -a C -f RCV000015246_1000.vcf
#./client -c 1,1 -p 160934818,160935328 -r A,AAA -a G,TGC -f RCV000015246_1000.vcf
#./client -c 1,1,1,1,1 -p 160929435,161645470,161257204,161252703,161240641 -r G,A,.,C,TGAT -a A,C,AA,T,. -f RCV000015246_1000.vcf

#./client -c 1 -p 161645470 -r A -a C -f RCV000015246_1000.vcf,RCV000015246_10000.vcf
#./client -c 1,1,1,1,1 -p 160929435,161645470,161257204,161252703,161240641 -r G,A,.,C,TGAT -a A,C,AA,T,. -f RCV000015246_1000.vcf,RCV000015246_10000.vcf

#./client -c 2 -p 161617087 -r T -a . -f RCV000015246_1000.vcf
#./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf
#./client -c 1,1 -p 160934818,160935328 -r A,AAAA -a G,TGC -f RCV000015246_1000.vcf
#./client -c 1,1,2,1,1 -p 160929435,161645470,161257204,161252703,161240641 -r G,A,.,C,TGAT -a A,C,AA,T,. -f RCV000015246_1000.vcf
#./client -c 1,1,2,1,1 -p 160929435,161645470,161257204,161252703,161240641 -r G,A,.,C,TGAT -a A,C,AA,T,. -f RCV000015246_1000.vcf,RCV000015246_10000.vcf


#####-------- 10,000 ENTRIES --------#####
#./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf
#./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf
#./client -c 1 -p 164781110 -r . -a ATATAAG -f RCV000015246_10000.vcf
#./client -c 1 -p 160952937 -r . -a GGAGGTTTCAGTGAGCT -f RCV000015246_10000.vcf
#./client -c 1 -p 161777655 -r TTTTTTTTTTTTTTTTTTTT -a . -f RCV000015246_10000.vcf
#./client -c 1 -p 160929435 -r G -a A -f RCV000015246_10000.vcf
#./client -c 1 -p 167175221 -r TGTGT -a ATTCAG -f RCV000015246_10000.vcf
#./client -c 1,1,1 -p 160940625,160942818,160945777 -r CT,A,. -a TG,C,T -f RCV000015246_10000.vcf
#./client -c 1,1,1,1 -p 160952522,160952708,160952937,160953538 -r C,C,.,. -a T,T,GGAGGTTTCAGTGAGCT,T -f RCV000015246_10000.vcf

#./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf,RCV000015246_1000.vcf,RCV000015246_100000.vcf,RCV000015246_5000000.vcf
#./client -c 1,1,1,1 -p 160952522,160952708,160952937,160953538 -r C,C,.,. -a T,T,GGAGGTTTCAGTGAGCT,T -f RCV000015246_10000.vcf,RCV000015246_1000.vcf

#./client -c 2 -p 161617087 -r T -a . -f RCV000015246_10000.vcf
#./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_10000.vcf
#./client -c 1,2,1 -p 160940625,160942818,160945777 -r CT,A,. -a TG,C,T -f RCV000015246_10000.vcf
#./client -c 1,1 -p 167187316,167187390 -r T,C -a C,T -f RCV000015246_10000.vcf,RCV000015246_1000.vcf
#./client -c 1 -p 73931553 -r G -a A -f RCV000015246_10000.vcf,RCV000015246_100000.vcf

######-------- 100,000 ENTRIES --------#####
#./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf
#./client -c 1 -p 161617087 -r T -a . -f RCV000015246_100000.vcf
#./client -c 1 -p 165508295 -r TGTGTGTGTGTGTGTGTGTG -a . -f RCV000015246_100000.vcf
#./client -c 1 -p 167194435 -r T -a C -f RCV000015246_100000.vcf
#./client -c 1 -p 80349420 -r . -a AAAAGGCAAGTCCCTCCCACCTATGAGCCTGTAAAATCA -f RCV000015246_100000.vcf
#./client -c 1 -p 80412577 -r AT -a CAC -f RCV000015246_100000.vcf
#./client -c 1,1 -p 73931553,73931594 -r G,G -a A,C -f RCV000015246_100000.vcf
#./client -c 1,1 -p 73934717,73934895 -r .,A -a T,G -f RCV000015246_100000.vcf

#./client -c 1 -p 73940291 -r G -a C -f RCV000015246_100000.vcf,RCV000015246_5000000.vcf
#./client -c 1 -p 165508295 -r TGTGTGTGTGTGTGTGTGTG -a . -f RCV000015246_100000.vcf,RCV000015246_5000000.vcf

#./client -c 2 -p 161617087 -r T -a . -f RCV000015246_100000.vcf
#./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf
#./client -c 1,1 -p 73931553,73931594 -r GA,G -a A,C -f RCV000015246_100000.vcf
#./client -c 2 -p 73940291 -r G -a C -f RCV000015246_100000.vcf,RCV000015246_5000000.vcf
#./client -c 2 -p 165508295 -r TGTGTGTGTGTGTGTGTGTG -a . -f RCV000015246_100000.vcf,RCV000015246_5000000.vcf

######-------- 5,000,000 ENTRIES --------#####
./client -c 4 -p 160955085 -r CTA -a TTG -f RCV000015246_5000000.vcf
./client -c 4 -p 161617087 -r T -a . -f RCV000015246_5000000.vcf
./client -c 3 -p 165508295 -r TGTGTGTGTGTGTGTGTGTG -a . -f RCV000015246_5000000.vcf
./client -c 8 -p 167194435 -r T -a C -f RCV000015246_5000000.vcf
./client -c 5 -p 80349420 -r . -a AAAAGGCAAGTCCCTCCCACCTATGAGCCTGTAAAATCA -f RCV000015246_5000000.vcf
./client -c 1 -p 80412577 -r AT -a CAC -f RCV000015246_5000000.vcf
./client -c 15,12 -p 73931553,73931594 -r G,G -a A,C -f RCV000015246_5000000.vcf
./client -c 9,10 -p 73934717,73934895 -r .,A -a T,G -f RCV000015246_5000000.vcf

./client -c 1,1 -p 74231684,74221890 -r .,A -a TCTC,G -f RCV000015246_5000000.vcf
./client -c 1,1,1 -p 74222034,74222036,74223964 -r .,A,A -a C,C,C -f RCV000015246_5000000.vcf

./client -c 52 -p 161617087 -r T -a . -f RCV000015246_5000000.vcf
./client -c 52 -p 160955085 -r CTA -a TTG -f RCV000015246_5000000.vcf
./client -c 1,1 -p 74231684,74221890 -r .,A -a TCTCA,G -f RCV000015246_5000000.vcf
./client -c 4 -p 74231684 -r . -a TTG -f RCV000015246_5000000.vcf
./client -c 3 -p 160955085 -r CT -a TTG -f RCV000015246_5000000.vcf
#./client -c 4 -p 161617087 -r T -a . -f RCV000015246_5000000.vcf,RCV000015246_1000.vcf
#./client -c 3 -p 165508295 -r TGTGTGTGTGTGTGTGTGTG -a . -f RCV000015246_5000000.vcf,RCV000015246_10000.vcf

cd ..
cd Tests
./checkExecution
