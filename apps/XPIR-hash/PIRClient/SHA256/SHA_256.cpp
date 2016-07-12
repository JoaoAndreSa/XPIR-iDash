#include "SHA_256.hpp"

//***PRIVATE METHODS***//

void SHA_256::printElem(unsigned char* conv, int size){
	for (int i=0; i<size; i++){
		printf("%02x", conv[i]);
	}
	printf("\n");
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

std::string SHA_256::type_to_binary(int len, std::string ref, std::string alt){
	std::string  binary="";

	if(alt==" "){							//if DEL
		binary+="01";
		binary+=decimal_to_binary(len);
	}else if(ref==" "){						//if INS
		binary+="11";
		binary+=decimal_to_binary(len);
		binary+=base_to_binary(alt);
	}else if(len>1 || alt.length()>1){		//if SUB
		binary+="10";
		binary+=base_to_binary(alt);
	}else{  								//if SNP
		binary+="00";
		binary+=base_to_binary(alt);
	}
	return binary;
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

//ASCII  to hexadecimal
std::string SHA_256::reduceSHA(unsigned char* str, int size){
	std::string final_hash="";

	int curr_bits=HASH_SIZE;
	for (int i=0; i<size && curr_bits>0; i++) {
		char buff[4];
		snprintf(buff, sizeof(buff), "%02x",str[i]);

		std::string buffAsStdStr=buff;
		std::string bin=hex_to_binary(buffAsStdStr);

		final_hash+=bin.substr(0,std::min(curr_bits,static_cast<int>(bin.length())));
		curr_bits-=std::min(curr_bits,static_cast<int>(bin.length()));
	}
	return final_hash;
}

/*void SHA_256::sha256(std::string str, unsigned char* hash){
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.length());
    SHA256_Final(hash, &sha256);
}*/

void SHA_256::mac256(std::string str,unsigned char* hash){
	const char* key= reinterpret_cast<char*>(m_key);

	HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);

    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
    HMAC_Init_ex(&ctx, key, strlen(key), EVP_sha256(), NULL);
    HMAC_Update(&ctx, (unsigned char*)str.c_str(),str.length()+1);

    unsigned int len=static_cast<unsigned int>(SHA256_DIGEST_LENGTH);
    HMAC_Final(&ctx,hash,&len);
    HMAC_CTX_cleanup(&ctx);
}

std::string SHA_256::encoding(std::string line){
	std::vector<std::string> tokens = tokenize(line,"\t");

	return chr_to_binary(tokens[0])+pos_to_binary(tokens[1])+type_to_binary(tokens[3].length(),tokens[3],tokens[4]);
}

std::vector<std::string> SHA_256::tokenize(std::string entry,std::string delimiter){
	std::vector<std::string> tokens;

	size_t pos = 0;
	while ((pos=entry.find(delimiter)) != std::string::npos) {
    	tokens.push_back(entry.substr(0,pos));
    	entry.erase(0,pos+delimiter.length());
	}
	tokens.push_back(entry.substr(0,pos));

	return tokens;
}


//***PUBLIC METHODS***//

uint64_t SHA_256::hash(std::string str){
	unsigned char* sha_hash = new unsigned char[SHA256_DIGEST_LENGTH];
	mac256(encoding(str),sha_hash);

	std::string final_hash=reduceSHA(sha_hash,SHA256_DIGEST_LENGTH);
	delete[] sha_hash;

	return stol(final_hash, nullptr,2);
}

std::string SHA_256::search(std::string entry,std::string query){
	if(entry=="") return 0;

	std::vector<std::string> tokens = tokenize(entry,"->");

	for(uint64_t i=0; i<tokens.size();i++){
		if(compareSNPs(tokens[i],query)==1){
			return tokens[i];
		}
	}

	return "";
}

uint64_t SHA_256::getSizeBits(){
	return pow(2,HASH_SIZE);
}

void SHA_256::printVector(std::vector<std::string> v){
	for(uint64_t i=0; i<v.size();i++){
		std::cout << v[i] << std::endl;
	}
}

//return num_entries if SNPs are equal 
int SHA_256::compareSNPs(std::string t_curr,std::string t_entry){
    std::vector<std::string> tokens_curr = tokenize(t_curr,"\t");
    std::vector<std::string> tokens_entry = tokenize(t_entry,"\t");

    if( (atoi(tokens_curr[0].c_str()) == atoi(tokens_entry[0].c_str())) && (atoi(tokens_curr[1].c_str()) == atoi(tokens_entry[1].c_str())) && (tokens_curr[3]==tokens_entry[3])  && (tokens_curr[4]==tokens_entry[4]) ){
        return 1;
    }else{
        return 0;   
    }
}

