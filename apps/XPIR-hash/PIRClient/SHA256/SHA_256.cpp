#include "SHA_256.hpp"

//***PRIVATE METHODS***//

void SHA_256::printElem(unsigned char* conv, int size){
	for (int i=0; i<size; i++){
		printf("%02x", conv[i]);
	}
	printf("\n");
}

std::string SHA_256::data_to_binary(std::string op,int len, std::string alt){
	std::string binary="";
	if(op=="01"){							//if DEL
		return decimal_to_binary(len);
	}else if(op=="11"){						//if SUB
		binary+=decimal_to_binary(len);
		binary+=base_to_binary(alt);
		return binary;
	}else{									//if INS or SNP
		return base_to_binary(alt);
	}
}

std::string SHA_256::op_to_binary(std::string ref, std::string alt){
	if(alt==" "){														//if DEL
		return "01";
	}else if(ref.length()>1 || alt.length()>1 && ref!=" " && alt!=" "){	//if SUB
		return "11";
	}else{																//if INS or SNP
		return "10";
	}
}

std::string SHA_256::chr_to_binary(std::string number){
	return std::bitset<5>(atoi(number.c_str())).to_string();
}

std::string SHA_256::pos_to_binary(std::string number){
	return std::bitset<28>(atol(number.c_str())).to_string();
}

std::string SHA_256::base_to_binary(std::string base){
	std::string binary="";
	for(int i=0;i<base.length();i++){
		if(base[i]=='A'){
			binary+="00";
		}else if(base[i]=='T'){
			binary+="01";
		}else if(base[i]=='G'){
			binary+="10";
		}else if(base[i]=='C'){
			binary+="11";
		}
	}
	return binary;
}

std::string SHA_256::decimal_to_binary(unsigned n){
    const int size=sizeof(n)*8;
    std::string res;
    bool s=0;
    for (int a=0;a<size;a++){
        bool bit=n>>(size-1);
        if (bit)
            s=1;
        if (s)
            res.push_back(bit+'0');
        n<<=1;
    }
    if (!res.size())
        res.push_back('0');
    return res;
}

/*void SHA_256::sha256(std::string str, unsigned char* hash){
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.length());
    SHA256_Final(hash, &sha256);
}*/

std::string SHA_256::hex_to_binary(std::string hex){
    std::string  binary="";

    for(int i=0;i<hex.length();i++){
		std::stringstream ss;
    	ss << std::hex << hex[i];
    	unsigned n;
	    ss >> n;

	    std::bitset<4> b(n);
	    binary+=b.to_string();
    }
    return binary;
}

void SHA_256::mac256(std::string str,unsigned char* hash){
	const char* key= reinterpret_cast<char*>(m_key);

	HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);

    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
    HMAC_Init_ex(&ctx, key, sizeof(key), EVP_sha256(), NULL);
    HMAC_Update(&ctx, (unsigned char*)str.c_str(),str.length()+1);

    unsigned int len=static_cast<unsigned int>(SHA256_DIGEST_LENGTH);
    HMAC_Final(&ctx,hash,&len);
    HMAC_CTX_cleanup(&ctx);
}

//***PUBLIC METHODS***//
//ASCII  to hexadecimal
std::string SHA_256::uchar_to_binary(unsigned char* str, int size, int curr_bits){
	string final_hash="";
	for(int i=0;i<(curr_bits/8);i++){
		final_hash+=std::bitset<std::numeric_limits<unsigned char>::digits>(str[i]).to_string();
	}
	return final_hash;
}

unsigned char* SHA_256::binary_to_uchar(std::string line){
	int size=ceil(line.length()/8);
	unsigned char* result = new unsigned char[size];
	memset(result,0,size);

	for(int j=0;j<size;j++){
		for (int i=0;i<8;i++){
    		result[j] |= (line[i+j*8]=='1') << (7-i);
		}
	}
    return result;
}

std::string SHA_256::encoding(std::string line){
	std::vector<std::string> tokens = Tools::tokenize(line,"\t");
	std::string op = op_to_binary(tokens[3],tokens[4]);

	return op+chr_to_binary(tokens[0])+pos_to_binary(tokens[1])+data_to_binary(op,tokens[3].length(),tokens[4]);
}

string SHA_256::hash(std::string str){
	unsigned char* sha_hash = new unsigned char[SHA256_DIGEST_LENGTH];
	mac256(str,sha_hash);
	std::string final_hash=uchar_to_binary(sha_hash,SHA256_DIGEST_LENGTH,DATA_SIZE);
	delete[] sha_hash;

	return final_hash;
}


bool SHA_256::search(std::string query, std::string decoded_pack){
    for(int i=0;i<decoded_pack.length();i+=DATA_SIZE){
        if(query.compare(decoded_pack.substr(i,DATA_SIZE))==0){
            return true;
        }
    }
    return false;
}

int SHA_256::getHashSize(){
	return HASH_SIZE;
}

void SHA_256::printVector(std::vector<std::string> v){
	for(uint64_t i=0; i<v.size();i++){
		std::cout << v[i] << std::endl;
	}
}