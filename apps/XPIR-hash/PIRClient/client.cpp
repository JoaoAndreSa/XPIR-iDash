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
    Parse query. It's organised in the following way:
    	-c (chromosome)
    	-p (position)
    	-r (reference allele)
    	-a (alternate allele)
    	-f (db file)

    @param argc number of input elements
    @param argv array with all query input elements

    @return entry map (dictionary) that stores the data of a query in a key-value way
*/
std::map<char,std::string> parseEntry(int argc,char* argv[]){
	std::map<char,std::string> entry;

	for(int i=1;i<argc;i+=2){
		std::string element{argv[i]};
		std::string elemenmatch{argv[i+1]};

		if(element=="-f"){
			elemenmatch="vcf/"+elemenmatch;
		}
		entry.insert(std::make_pair(element.at(1),elemenmatch));
	}
	return entry;
}

/**
    Parse query. It's organised in the following way:
    	-c (chromosome)
    	-p (position)
    	-r (reference allele)
    	-a (alternate allele)
    	-f (db file)

    @param argc number of input elements
    @param argv array with all query input elements

    @return entry map (dictionary) that stores the data of a query in a key-value way
*/
void writeToFile(string filename, string output){
	try{
		ofstream f(filename, std::ios_base::app);
	 	if (f.is_open()){
			f << output << "\n";
			f.close();
			return;
		}
  		Socket::errorExit(1,"Unable to open file");
	}catch (std::ios_base::failure &fail){
        Socket::errorExit(1,"Error writing output file");
    }
}

int main(int argc, char* argv[]){
	//example input: ./client -c 1 -p 161276680 -r A -a T -f RCV000015246_10000.vcf -> Query variation in file;
	//               ./client -c 1 -p 160929435 -r G -a A -f RCV000015246_10000.vcf -> Query variation in file;
	//				 ./client -c 2 -p 161276680 -r A -a T -f RCV000015246_10000.vcf -> Query variation not in file;
	Socket::errorExit(argc<5,"Syntax : ./client [-c chromosome] [-p startPosition] [-r refAllele] [-a altAllele] [-f vcfFile]");	
	std::map<char,std::string> entry = parseEntry(argc,argv);

	//Start server connection
	Socket::errorExit((Constants::port > 65535) || (Constants::port < 2000),"Please choose a port number between 2000 - 65535");
	Socket socket(1);

	PIRClient* c;
	if(Constants::pipeline){	//if PIPELINE execution
		c=new PIRClientPipeline(socket);
	}else{						//if SEQUENTIAL execution
		c=new PIRClientSequential(socket);
	}

	c->initSHA256();
	uint64_t num_entries=c->uploadData(entry['f']);

	c->setRTTStart();
	std::string resp=c->searchQuery(num_entries,entry);
	c->setRTTStop();
	string output="";
	if(resp==""){
		output+="Query variation not in file.\n";
        std::cout << output;
    }else{
    	output+="Query variation in file:\n";
        std::cout << output;
        std::cout << resp << "\n\n"; 
    }

    std::ostringstream strs;
	strs << (c->getRTTStop()-c->getRTTStart());
	std::string time = "Elapsed time (RTT): "+strs.str()+"(s)\n";
   	std::cout << time;

   	output+=time;
   	writeToFile("output.txt",output);
	return 0;
}