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

#define NUM_TESTS 4 		//number of different entries tested (1,000 - 10,000 - 100,000)
#define NUM_YES 6
#define NUM_NO 2

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
				std::cout << "#######-----NUM ENTRIES: " << pow(10,i+3) << "-----#######" << std::endl;

				int j;
				for(j=0;j<NUM_YES;j++){
					getline (f,line); printCheck(line,j,"All query variation(s) found in file(s).");
					getline(f,line); getTime(line);
					getline(f,line);
				}

				int k;
				for(k=j;k<NUM_NO+j;k++){
					getline (f,line); printCheck(line,2,"At least one query variation not in file(s).");
					getline(f,line); getTime(line);
					getline(f,line);
				}

				std::cout << std::endl;
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