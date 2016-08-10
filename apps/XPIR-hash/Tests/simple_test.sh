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
./changeParamsSHA 13
./changeParamsPIR 2 3 53 52 0 0 LWE:97:1024:60

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
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_1000.vcf
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf
./client -c 1 -p 160999478 -r A -a G -f RCV000015246_1000.vcf
./client -c 1 -p 161000239 -r . -a TAAA -f RCV000015246_1000.vcf
./client -c 1 -p 161005671 -r TGGA -a ACCAACAT -f RCV000015246_1000.vcf
./client -c 1 -p 161645470 -r A -a C -f RCV000015246_1000.vcf
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_1000.vcf
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf

#####-------- 10,000 ENTRIES --------#####
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf
./client -c 1 -p 164781110 -r . -a ATATAAG -f RCV000015246_10000.vcf
./client -c 1 -p 160952937 -r . -a GGAGGTTTCAGTGAGCT -f RCV000015246_10000.vcf
./client -c 1 -p 161777655 -r TTTTTTTTTTTTTTTTTTTT -a . -f RCV000015246_10000.vcf
./client -c 1 -p 160929435 -r G -a A -f RCV000015246_10000.vcf
./client -c 1 -p 167175221 -r TGTGT -a ATTCAG -f RCV000015246_10000.vcf
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_10000.vcf
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_10000.vcf

######-------- 100,000 ENTRIES --------#####
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_100000.vcf
./client -c 1 -p 165508295 -r TGTGTGTGTGTGTGTGTGTG -a . -f RCV000015246_100000.vcf
./client -c 1 -p 167194435 -r T -a C -f RCV000015246_100000.vcf
./client -c 1 -p 80349420 -r . -a AAAAGGCAAGTCCCTCCCACCTATGAGCCTGTAAAATCA -f RCV000015246_100000.vcf
./client -c 1 -p 80412577 -r AT -a CAC -f RCV000015246_100000.vcf
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_100000.vcf
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf

######-------- 5,000,000 ENTRIES --------#####
./client -c 4 -p 160955085 -r CTA -a TTG -f RCV000015246_5000000.vcf
./client -c 4 -p 161617087 -r T -a . -f RCV000015246_5000000.vcf
./client -c 3 -p 165508295 -r TGTGTGTGTGTGTGTGTGTG -a . -f RCV000015246_5000000.vcf
./client -c 8 -p 167194435 -r T -a C -f RCV000015246_5000000.vcf
./client -c 5 -p 80349420 -r . -a AAAAGGCAAGTCCCTCCCACCTATGAGCCTGTAAAATCA -f RCV000015246_5000000.vcf
./client -c 1 -p 80412577 -r AT -a CAC -f RCV000015246_5000000.vcf
./client -c 52 -p 161617087 -r T -a . -f RCV000015246_5000000.vcf
./client -c 52 -p 160955085 -r CTA -a TTG -f RCV000015246_5000000.vcf

cd ..
cd Tests
./checkExecution
