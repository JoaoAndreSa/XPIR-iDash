#include <iostream>
#include <fstream>

int main(int argc, char* argv[]){

	std::ofstream f("paramsPIR.txt");
	if(f.is_open()){
		for(int i=1;i<argc;i++){
			f << argv[i];
			if((i+1)<argc) f << "\n";
		}
    	f.close();
	}
  	else std::cout << "Unable to open file" << std::endl;
  	
  	return 0;
}