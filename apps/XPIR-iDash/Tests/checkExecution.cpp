#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <vector>
#include <math.h>

#define NUM_TESTS 3
#define TEST_TYPES 2
#define VARIANT_SETUPS 1

void printCheck(std::string line, int test, std::string response){
	if(line==response) std::cout << "[TEST " << test << "] - CORRECT > ";
	else std::cout << "[TEST " << test << "] - INCORRECT > ";
}

void getTime(std::string line){
	using namespace std;
	istringstream iss(line);
	vector<string> tokens{istream_iterator<string>{iss},istream_iterator<string>{}};

	cout << tokens[3] << endl;
}

int main(int argc, char* argv[]){
	std::string line;
	std::ifstream f("../PIRClient/output.txt");

	if (f.is_open()){
		for(int i=0;i<NUM_TESTS;i++){
			std::cout << "NUM ENTRIES: " << pow(10,i+3) << std::endl;
			for(int j=0;j<TEST_TYPES;j++){
				//if(j==0) std::cout << "###---NO PACKING---###" << std::endl;
				if(j==0) std::cout << "###---PACKING---###" << std::endl;
				else if(j==1) std::cout << "###---PACKING & DIM---###" << std::endl;

				for(int k=0;k<VARIANT_SETUPS;k++){
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
  	else std::cout << "Unable to open file"; 

  	return 0;
}