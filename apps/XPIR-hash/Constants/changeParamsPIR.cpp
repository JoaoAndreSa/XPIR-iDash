/**
    XPIR-hash
    changeParamsPIR.cpp
    Purpose: Change PIR parameters

    @author Joao Sa
    @version 1.0 07/09/16
*/

/**
	paramsPIR.txt:
		dimensionality: - recursion (0<d<4)
		alpha:			- packing (number of elements per ciphertext/polynomial)

		(ordered values, the biggest dimension is always in N[0])
		N[0]:			- number of query elements in dimension 1
		N[1]:			- (...) 2 (should 0 if d<2)
		N[2]:			- (...) 3 (should 0 if d<3)
		N[3]:			- (...) 4 (should 0 if d<4)

		crypto:			- encryption_scheme:max_security:number_coefficients:bits_per_coefficient

		example (for 1000 entries):

			2
			3
			53   (structure 53x52 aggregated elements [3])
			52
			0
			0
			LWE:97:1024:60 
*/

#include <iostream>
#include <fstream>

//HOW TO USE: ./changeParamsPIR 2 3 53 52 0 0 LWE:97:1024:60
int main(int argc, char* argv[]){

	try{

		std::ofstream f("paramsPIR.txt");
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