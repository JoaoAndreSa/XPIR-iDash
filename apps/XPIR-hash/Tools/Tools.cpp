/**
    XPIR-hash
    Tools.cpp
    Purpose: Class to manage a set of functions used by both client and server (e.g. read parameters, verify parameters, etc.).

    @author Joao Sa
    @version 1.0 01/07/16
*/

#include "Tools.hpp"

//***PRIVATE METHODS***//
void Tools::readFromBinFile(string filename, char* recvBuf){
    try{
        streampos size;
        ifstream f(filename,ios::in|ios::binary);

        error(f==nullptr || f.is_open()==0,"Error reading binary file");
        if(f.is_open()){
            size = f.tellg();
            f.seekg(0,ios::beg);
            f.read(recvBuf,size);
        }
        f.close();
    }catch(std::ios_base::failure &fail){
        error(1,"Error reading binary file");
    }
}

void Tools::writeToBinFile(string filename, char* recvBuf, int size){
    try{
        ofstream f(filename,ios::out|ios::binary|std::fstream::app);

        error(f==nullptr || f.is_open()==0,"Error writing binary file");
        if(f.is_open()){
            f.write(recvBuf,size);
        }
        f.close();
    }catch(std::ios_base::failure &fail){
        error(1,"Error writing binary file");
    }
}

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
        error(f==NULL || f.is_open()==0,"Error reading file");
        if (f.is_open()){
            getline(f,line);
            int num_bits = atoi(line.c_str());

            error(num_bits<=0,"Wrong SHA parameters");
            return num_bits;
        }
    }catch (std::ios_base::failure &fail){
        std::cout << "Error opening file" << std::endl;
        return 1;
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

        error(f==NULL || f.is_open()==0,"Error reading paramsPIR.txt file");
        if (f.is_open()){
            getline(f,line);
            params.d=atoi(line.c_str());

            getline(f,line);
            params.alpha=atoi(line.c_str());

            for(int i=0;i<4;i++){
                getline(f,line);
                params.n[i]=atoi(line.c_str());
            }
            error(params.d<1 || params.d>4 || params.alpha<1 || params.alpha>num_entries || verifyParams(params.d,params.alpha,params.n,num_entries)==0,"Wrong PIR parameters");

            getline(f,line);
            params.crypto_params=line;
        }
        f.close();

    }catch (std::ios_base::failure &fail){
        error(1,"Error reading paramsPIR.txt file");
    }

    return params;
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