/**
    XPIR-hash
    Tools.cpp
    Purpose: Class to manage a set of functions used by both client and server (e.g. read parameters, verify parameters, etc.).

    @author Joao Sa
    @version 1.0 01/07/16
*/

#include "Tools.hpp"

//***PRIVATE METHODS***//
/**
    Check 3rd error on the next list of errors - see function readParamsPIR().

    @param d recursion/dimension value
    @param alpha aggregation value
    @param n recursion array (number of elements in each dimension)
    @param num_entries total number of entries (size of database)

    @return 0/1 0-INCORRECT, 1-CORRECT.
*/
int Tools::verifyParams(uint64_t d, uint64_t alpha, unsigned int* n, uint64_t num_entries){
    int total=alpha;

    for(int i=0;i<d;i++){
        total*=n[i];
    }

    if(total<num_entries) return 0;
    else return 1;
}

//***PUBLIC METHODS***//
std::vector<string> Tools::listFilesFolder(string foldername){
    std::vector<string> listFiles;

    DIR* dir;
    struct dirent* file;
    dir = opendir(foldername.c_str());

    if(dir==NULL) Error::error(1,"Error reading file");

    while ((file = readdir(dir)) != NULL){
        string filename(file->d_name);
        if (filename.find(".vcf") != std::string::npos){
            listFiles.push_back(filename);
        }
    }
    (void)closedir(dir);

    return listFiles;
}


void Tools::readFromBinFile(string filename, char* recvBuf, int size){
    try{
        ifstream f(filename,ios::in|ios::binary);

        Error::error(f==nullptr || f.is_open()==0,"Error opening binary file");
        if(f.is_open()){
            f.read(recvBuf,size);
            f.close();
        }
    }catch(std::ios_base::failure &fail){
        Error::error(1,"Error reading binary file");
    }
}

void Tools::writeToBinFile(string filename, char* recvBuf, int size){
    try{
        ofstream f(filename,ios::out|ios::binary|std::fstream::app);

        Error::error(f==nullptr || f.is_open()==0,"Error opening binary file");
        if(f.is_open()){
            f.write(recvBuf,size);
            f.close();
        }
    }catch(std::ios_base::failure &fail){
        Error::error(1,"Error writing binary file");
    }
}

string Tools::readFromTextFile(string filename){
    try{
        string content="";
        ifstream f(filename);

        Error::error(f==nullptr || f.is_open()==0,"Error opening text file");
        if (f.is_open()){
            string line;
            while(getline(f,line)){
              content+=line+"\n";
            }
        }
        f.close();
        return content;
    }catch (std::ios_base::failure &fail){
        Error::error(1,"Error writing text file");
    }
}

void Tools::writeToTextFile(string filename, string output){
    try{
        ofstream f(filename, std::ios_base::app);

        Error::error(f==nullptr || f.is_open()==0,"Error opening text file");
        if (f.is_open()){
            f << output << "\n";
            f.close();
        }
    }catch (std::ios_base::failure &fail){
        Error::error(1,"Error writing text file");
    }
}

/**
    Read SHA params and check for errors on paramsSHA.txt file (e.g. num_bits<0).
    The SHA params are simply the number of bits that are going to be extracted from the HMAC (SHA256) to generate an id for each 
    variant, as well as, the size of the 'catalog' (number of entries in DB) used to store data (size=2^num_bits).
    NOTE: a big number of bits reduces the number of possible collisions among elements but increases the size of
    the database and consequently the computation costs, and vice-versa.

    @param
    @return num_bits number of bits for the hash 'mechanism'
*/
int Tools::readParamsSHA(){
    std::string line;

    try{
        ifstream f("../Constants/paramsSHA.txt");
        Error::error(f==NULL || f.is_open()==0,"Error opening file");
        if (f.is_open()){
            getline(f,line);
            int num_bits = atoi(line.c_str());

            Error::error(num_bits<=0,"Wrong SHA parameters");
            return num_bits;
        }
    }catch (std::ios_base::failure &fail){
        Error::error(1,"Error reading paramsSHA.txt file");
    }
}

/**
    Read PIR params and check for errors on paramsPIR.txt file (e.g. dimension>4 || alpha<1...). 
    List of errors checked:
        - 4 < dimension < 1;
        - num_elements in db < alpha < 1;
        - alpha * N[i] (where i=0..d) < num_elements in db (the aggregation and dimension parameters have to fit the amount of 
            data in DB - example: 1000 entries> alpha=10, d=2, N[0]=10, N[1]=10 - 10*10*10>=1000 CORRECT)

    @param num_entries total number of entries (size of database)

    @return params PIR parameters.
*/
PIRParameters Tools::readParamsPIR(uint64_t num_entries){
    std::string line;
    PIRParameters params;

    try{

        ifstream f("../Constants/paramsPIR.txt");

        Error::error(f==NULL || f.is_open()==0,"Error reading paramsPIR.txt file");
        if (f.is_open()){
            getline(f,line);
            params.d=atoi(line.c_str());

            getline(f,line);
            params.alpha=atoi(line.c_str());

            for(int i=0;i<4;i++){
                getline(f,line);
                params.n[i]=atoi(line.c_str());
            }
            Error::error(params.d<1 || params.d>4 || params.alpha<1 || params.alpha>num_entries || verifyParams(params.d,params.alpha,params.n,num_entries)==0,"Wrong PIR parameters");

            getline(f,line);
            params.crypto_params=line;
        }
        f.close();

    }catch (std::ios_base::failure &fail){
        Error::error(1,"Error reading paramsPIR.txt file");
    }

    return params;
}

std::vector<std::string> Tools::tokenize(std::string entry,std::string delimiter){
    std::vector<std::string> tokens;

    size_t pos = 0;
    while ((pos=entry.find(delimiter)) != std::string::npos) {
        tokens.push_back(entry.substr(0,pos));
        entry.erase(0,pos+delimiter.length());
    }
    tokens.push_back(entry.substr(0,pos));

    return tokens;
}

/**
    Free allocated memory.

    @param v vector of char* to be freed

    @return
*/
void Tools::cleanupVector(vector<char*> v){
    for(uint64_t i=0;i<v.size();i++){
        delete[] v[i];
    }
}