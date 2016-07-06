#####-------- SIMPLE TEST BASH --------#####
lxterminal --working-directory=Desktop/XPIR/apps/XPIR-hash/PIRServer/ -e "./server"
sleep 1












#####-------- 1000 ENTRIES --------#####
./changeParamsSHA 10

# ///////////////////--------NO PACKING---------////////////////////////////////
#./changeParamsPIR 1 1 1024 0 0 0 LWE:97:1024:60

cd ..
cd PIRClient
rm -rf logs/log_1000.txt
rm -rf output.txt
#./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf >> logs/log_1000.txt #in db single file
#./client -c 1 -p 161617087 -r T -a . -f RCV000015246_1000.vcf >> logs/log_1000.txt #in db multiple files
#./client -c 2 -p 161617087 -r T -a . -f RCV000015246_1000.vcf >> logs/log_1000.txt #not in db single file
#./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf >> logs/log_1000.txt #not in db multiple files

# ///////////////////--------PACKING (35)---------////////////////////////////////
cd ..
cd Tests
./changeParamsPIR 1 30 35 0 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_1000.vcf >> logs/log_1000.txt #in db single file
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf >> logs/log_1000.txt #in db multiple files
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_1000.vcf >> logs/log_1000.txt #not in db single file
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf >> logs/log_1000.txt #not in db multiple files

# ///////////////////--------PACKING & DIM (15,9*8)---------////////////////////////////////
cd ..
cd Tests
./changeParamsPIR 2 15 9 8 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_1000.vcf >> logs/log_1000.txt #in db single file
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf >> logs/log_1000.txt #in db multiple files
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_1000.vcf >> logs/log_1000.txt #not in db single file
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_1000.vcf >> logs/log_1000.txt #not in db multiple files

cd ..
cd Tests












#####-------- 10000 ENTRIES --------#####
./changeParamsSHA 13

# ///////////////////--------NO PACKING---------////////////////////////////////
#./changeParamsPIR 1 1 8192 0 0 0 LWE:97:1024:60

cd ..
cd PIRClient
rm -rf logs/log_10000.txt
#./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf >> logs/log_10000.txt #in db single file
#./client -c 1 -p 164781110 -r ATATAAG -a . -f RCV000015246_10000.vcf >> logs/log_10000.txt  #in db multiple files
#./client -c 2 -p 161617087 -r T -a . -f RCV000015246_10000.vcf >> logs/log_10000.txt  #not in db single file
#./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_10000.vcf >> logs/log_10000.txt  #not in db multiple files



# ///////////////////--------PACKING (217)---------////////////////////////////////
cd ..
cd Tests
./changeParamsPIR 1 217 38 0 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf >> logs/log_10000.txt #in db single file
./client -c 1 -p 164781110 -r ATATAAG -a . -f RCV000015246_10000.vcf >> logs/log_10000.txt  #in db multiple files
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_10000.vcf >> logs/log_10000.txt  #not in db single file
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_10000.vcf >> logs/log_10000.txt  #not in db multiple files

# ///////////////////--------PACKING & DIM (15,24*23)---------////////////////////////////////
cd ..
cd Tests
./changeParamsPIR 2 15 24 23 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_10000.vcf >> logs/log_10000.txt #in db single file
./client -c 1 -p 164781110 -r ATATAAG -a . -f RCV000015246_10000.vcf >> logs/log_10000.txt  #in db multiple files
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_10000.vcf >> logs/log_10000.txt  #not in db single file
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_10000.vcf >> logs/log_10000.txt  #not in db multiple files

cd ..
cd Tests























######-------- 100000 ENTRIES --------#####
./changeParamsSHA 16

# ///////////////////--------NO PACKING---------////////////////////////////////
#./changeParamsPIR 1 1 65536 0 0 0 LWE:97:1024:60

cd ..
cd PIRClient
rm -rf logs/log_100000.txt
#./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf >> logs/log_100000.txt #in db single file
#./client -c 1 -p 161617087 -r T -a . -f RCV000015246_100000.vcf >> logs/log_100000.txt  #in db multiple files
#./client -c 2 -p 161617087 -r T -a . -f RCV000015246_100000.vcf >> logs/log_100000.txt  #not in db single file
#./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf >> logs/log_100000.txt  #not in db multiple files

# ///////////////////--------PACKING (1643)---------////////////////////////////////
cd ..
cd Tests
./changeParamsPIR 1 1643 40 0 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf >> logs/log_100000.txt #in db single file
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_100000.vcf >> logs/log_100000.txt  #in db multiple files
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_100000.vcf >> logs/log_100000.txt  #not in db single file
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf >> logs/log_100000.txt  #not in db multiple files

# ///////////////////--------PACKING & DIM (28,48*49)---------////////////////////////////////
cd ..
cd Tests
./changeParamsPIR 2 28 49 48 0 0 LWE:97:1024:60

cd ..
cd PIRClient
./client -c 1 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf >> logs/log_100000.txt #in db single file
./client -c 1 -p 161617087 -r T -a . -f RCV000015246_100000.vcf >> logs/log_100000.txt  #in db multiple files
./client -c 2 -p 161617087 -r T -a . -f RCV000015246_100000.vcf >> logs/log_100000.txt  #not in db single file
./client -c 2 -p 160955085 -r CTA -a TTG -f RCV000015246_100000.vcf >> logs/log_100000.txt  #not in db multiple files

cd ..
cd PIRServer
rm -rf db/*

cd ..
cd Tests

./checkExecution
