/**
    XPIR-hash
    changeParamsSHA.cpp
    Purpose: Change HMAC (SHA256) parameters

    @author Joao Sa
    @version 1.0 07/09/16
*/

/**
	paramsSHA.txt:
		num_bits: - number of bits to extract from the HMAC return value
		          - size of DB catalog where data is stored and then sent to the server (2^num_bits)

		example (could be used for 100,000 entries):

			13 (size of DB catalog = 8192 and 13 bits to extract from HMAC) 
*/

#include <iostream>
#include <fstream>

//HOW TO USE: ./changeParamsSHA 13
int main(int argc, char* argv[]){

	try{

		std::ofstream f("paramsSHA.txt");

		if(f.is_open()){
			for(int i=1;i<argc;i++){
				f << argv[i];
				if((i+1)<argc){
					f << "\n";
				}
			}
	    	f.close();
		}
	  	else{
	  		std::cout << "Unable to open file" << std::endl;
	  		return 1;
	  	}

	}catch (std::ios_base::failure &fail){
        std::cout << "Error opening file" << std::endl;
        return 1;
    }

  	return 0;
}