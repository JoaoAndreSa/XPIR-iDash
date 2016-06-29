/**
    XPIR-hash
    checkExecution.cpp
    Purpose: Check result of tests (by reading ../PIRClient/output.txt)

    @author Joao Sa
    @version 1.0 01/07/16
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <vector>
#include <math.h>

#define NUM_TESTS 3 		//number of different entries tested (1,000 - 10,000 - 100,000)
#define TEST_TYPES 2		//number of different combinations of PIR parameters (with/without recursion)
#define VARIANT_SETUPS 1	//number of different crypto parameters (LWE:97:1024:60)

/**
    Print test correctness depending on the results in ../PIRClient/output.txt.

    @param line output.txt entry.
    @param test number/ID of the test.
    @param expected the predicted result.

    @return
*/
void printCheck(std::string line, int test, std::string expected){
	if(line==expected) std::cout << "[TEST " << test << "] - CORRECT > ";
	else std::cout << "[TEST " << test << "] - INCORRECT > ";
}

/**
    Extract and print the execution time from the results in ../PIRClient/output.txt.

    @param line output.txt entry.

    @return
*/
void getTime(std::string line){
	using namespace std;
	istringstream iss(line);
	vector<string> tokens{istream_iterator<string>{iss},istream_iterator<string>{}};

	//example output: Elapsed time (RTT): 0.568602(s)
	cout << tokens[3] << endl;
}

int main(int argc, char* argv[]){
	std::string line;

	try{
		std::ifstream f("../PIRClient/output.txt");

		if (f.is_open()){
			for(int i=0;i<NUM_TESTS;i++){
				std::cout << "NUM ENTRIES: " << pow(10,i+3) << std::endl;

				for(int j=0;j<TEST_TYPES;j++){

					switch(j){
						//std::cout << "###---NO PACKING---###" << std::endl;
	  					case 0:
	  						std::cout << "###---PACKING---###" << std::endl;
	  						break;
	  					case 1:
	  						std::cout << "###---PACKING & DIM---###" << std::endl;
	  						break;
	  					 default:
	    					std::cout << "An error that does not make any sense!" << std::endl;
	    					return 1;
					}

					for(int k=0;k<VARIANT_SETUPS;k++){
						//for each varint setup we run 4 different queries (you can check simple_test.sh for more information)
						getline (f,line); printCheck(line,0,"Query variation in file:");
						getline(f,line); getTime(line);
						getline(f,line);

						getline (f,line); printCheck(line,1,"Query variation in file:");
						getline(f,line); getTime(line);
						getline(f,line);

						getline (f,line); printCheck(line,2,"Query variation not in file.");
						getline(f,line); getTime(line);
						getline(f,line);

						getline (f,line); printCheck(line,3,"Query variation not in file.");
						getline(f,line); getTime(line);
						getline(f,line);
						std::cout << std::endl;
					}
				}
				std::cout << std::endl << std::endl;
			}
		}
	  	else{
	  		std::cout << "Unable to open file";
	  		return 1;
	  	}

	}catch (std::ios_base::failure &fail){
        std::cout << "Error opening file" << std::endl;
        return 1;
    }

  	return 0;
}