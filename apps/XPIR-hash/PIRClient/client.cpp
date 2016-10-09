/**
    XPIR-hash
    client.cpp
    Purpose: CLIENT main

    @author Joao Sa
    @version 1.0 01/07/16
*/

#include "PIRClient.hpp"
#include "Pipeline/PIRClientPipeline.hpp"
#include "Sequential/PIRClientSequential.hpp"

/**
    Parse command. The query command is organised in the following way:
    	-c chromosome1,2,3,...
    	-p position1,2,3,...
    	-r reference allele1,2,3,...
    	-a alternate allele1,2,3,...
    	-f db file1,2,3,...

    The upload command is organised in the following way:
    	-f (path to folder where all the .vcf files are, ready to be uploaded)

    @param argc number of input elements
    @param argv array with all command input elements

    @return entry map (dictionary) that stores the data of the command in a key-value way
*/
std::map<char,std::string> parseEntry(int argc,char* argv[]){
	std::map<char,std::string> entry;

	if(argc%2!=0){
		for(int i=1;i<argc;i+=2){
			std::string element{argv[i]};
			std::string elemenmatch{argv[i+1]};

			if(element.at(1)!='f'){
				std::replace(elemenmatch.begin(),elemenmatch.end(),'.',' ');
			}
			entry.insert(std::make_pair(element.at(1),elemenmatch));
		}
	}
	return entry;
}

/**
	Verify input. Here are a list of condittions:
		-if there was an error during parsing (entry==NULL) or the number of arguments does not fit the two
		execution modes (see parse function for more details)

		-if uploading mode -> an entry named f must exist (folder path)

		-if querying mode -> all of the necessary fields must exist (c,p,r,a,f)
							 we need to ensure that the numbers are actual 'numbers'

    @param argc number of input elements
    @param entry map (dictionary) that stores the data of the command in a key-value way

    @return true/false true if there is an error, false otherwise
*/
bool entryError(int argc,std::map<char,std::string> entry){
	if(entry.empty() || argc!=3 && argc!=11) return true;

	std::map<char,std::string>::iterator it;
	if(argc==3){
		it = entry.find('f');
  		if(it == entry.end()) return true;
	}else{
		string chars("cpraf");
		for(int i=0;i<chars.length();i++){
			it = entry.find(chars[i]);
			if(it == entry.end()) return true;
		}

		for(int i=0;i<entry['c'].length();i++){
			if(isdigit(entry['c'][i])==0 && entry['c'][i]!=',') return true;
		}
		for(int i=0;i<entry['p'].length();i++){
			if(isdigit(entry['p'][i])==0 && entry['p'][i]!=',') return true;
		}
	}

	return false;
}

int main(int argc, char* argv[]){
	//example input(uploading): ./client -f vcf/
	//example input(querying): 	./client -c 1 -p 161276680 -r A -a T -f RCV000015246_10000.vcf -> Query variation in file;
	//               			./client -c 1 -p 160929435 -r G -a A -f RCV000015246_10000.vcf -> Query variation in file;
	//				 			./client -c 2 -p 161276680 -r A -a T -f RCV000015246_10000.vcf -> Query variation not in file;
	std::map<char,std::string> entry = parseEntry(argc,argv);

	Error::error(entryError(argc,entry),"Input Error\nUploading: ./client [-f folderPath]\nQuerying:  ./client [-c chromosome1,2,...] [-p startPosition1,2,...] [-r refAllele1,2,...] [-a altAllele1,2,...] [-f vcfFile1,2,...]\n");

	//Start server connection
	Error::error((Constants::port > 65535) || (Constants::port < 2000),"Please choose a port number between 2000 - 65535");
	Socket socket(1);			//1= client socket

	PIRClient* c;
	if(Constants::pipeline){	//if PIPELINE execution
		c=new PIRClientPipeline(socket);
	}else{						//if SEQUENTIAL execution
		c=new PIRClientSequential(socket);
	}

    c->initAES256();
	c->initSHA256();
	if(argc==3){			//SEND data to server
		socket.sendInt(1);
		double start = omp_get_wtime();
		c->uploadData(entry['f']);
		double end = omp_get_wtime();
		std::cout << "PIRClient: Setup database took " << end-start << " (s)" << endl;
		return 0;
	}else{					//QUERY the server
		socket.sendInt(0);

		c->setRTTStart();
		bool resp=c->searchQuery(entry);
		c->setRTTStop();

		string output="";
		if(resp==false){
			output+="At least one query variation not in file(s).\n";
	        std::cout << output;
	    }else{
	    	output+="All query variation(s) found in file(s).\n";
	        std::cout << output;
	    }

	    std::ostringstream strs;
		strs << (c->getRTTStop()-c->getRTTStart());
		std::string time = "Elapsed time (RTT): "+strs.str()+"(s)\n";
	   	std::cout << time;

	   	output+=time;
	   	Tools::writeToTextFile("output.txt",output);
	}
	cout << endl << endl << endl << endl << endl;

	return 0;
}