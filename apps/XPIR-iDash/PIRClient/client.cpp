#include "PIRClient.hpp"
#include "Parallel/PIRClientParallel.hpp"
#include "Sequential/PIRClientSequential.hpp"

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

void writeToFile(string filename, string output){
	ofstream f(filename, std::ios_base::app);
 	if (f.is_open()){
		f << output << "\n";
		f.close();
		return;
	}
  	PIRClient::errorExit(1,"Unable to open file");
}

int main(int argc, char* argv[]){
	//example input: ./client -c 1 -p 161276680 -r A -a T -f RCV000015246_10000.vcf -> Query variation in file;
	//               ./client -c 1 -p 160929435 -r G -a A -f RCV000015246_10000.vcf -> Query variation in file;
	//				 ./client -c 2 -p 161276680 -r A -a T -f RCV000015246_10000.vcf -> Query variation not in file;
	//				 ./client -c 1 -p 155951628	-r A -a G -f RCV000015246_100000.vcf
	PIRClient::errorExit(argc<5,"Syntax : ./client [-c chromosome] [-p startPosition] [-r refAllele] [-a altAllele] [-f vcfFile]");	
	std::map<char,std::string> entry = parseEntry(argc,argv);

	PIRClient* c;
	if(Constants::parallel){
		c=new PIRClientParallel(const_cast<char*>(Constants::hostname),Constants::port);
	}else{
		c=new PIRClientSequential(const_cast<char*>(Constants::hostname),Constants::port);
	}

	c->initSHA256();
	uint64_t num_entries=c->uploadData(entry['f']);
	c->initXPIR(num_entries);

	c->setRTTStart();
	std::string resp=c->searchQuery(num_entries,entry);
	string output="";
	if(resp==""){
		output+="Query variation not in file.\n";
        std::cout << output;
    }else{
    	output+="Query variation in file:\n";
        std::cout << output;
        std::cout << resp << "\n\n"; 
    }
    c->cleanup();

    c->setRTTStop();

    std::ostringstream strs;
	strs << (c->getRTTStop()-c->getRTTStart());
	std::string time = "Elapsed time (RTT): "+strs.str()+"(s)\n";
   	std::cout << time;

   	output+=time;
   	writeToFile("output.txt",output);
	return 0;
}