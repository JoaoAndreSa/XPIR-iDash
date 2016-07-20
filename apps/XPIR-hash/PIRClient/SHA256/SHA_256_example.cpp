#include "SHA_256.hpp"

#include <stdio.h>
#include <string.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

using namespace std;

vector<string> readFile(string filename,SHA_256 sha256){
	vector<string> entries;
	string line;

	ifstream f(filename);
	if (f.is_open()){
		while(getline(f,line)){
			if(line.substr(0,1)!="#"){
				entries.push_back(line);
			}
		}
		f.close();
	}
	return entries;
}

//SHA256 example
int main(){
	SHA_256 sha256(16);

	vector<string> data(sha256.getSizeBits());
	vector<string> entries;

	entries=readFile("RCV000015246_100000.vcf",sha256);

	int max_bit_size=0;
	for(int i=0;i<entries.size();i++){
		string encoded=sha256.encoding(entries[i]);
		int pos=sha256.hash(encoded);
		data[pos]+=encoded;

		if(data[pos].length()>max_bit_size){
			max_bit_size=data[pos].length();
		}
	}
	cout << max_bit_size << endl;
	//sha256.printVector(data);

	//string query="1	161617087	.	T	 "; 		//is in file
	//string query="1	164781110	.	 	ATATAAG"; 	//is in file
	//string query="1	161617087	.	T	 "; 		//is in file
	//string query="1	73934717	.	 	T	.	.	SVTYPE=INS;END=73934717"; 			//is in file
	string query="1	118737704	rs3085974	 	ACTCTGT	.	.	SVTYPE=INS;END=118737704"; 	//is in file
	//string query="2	73931850	rs11210256	G	A	.	.	SVTYPE=SNP;END=73931851"; 	//is not in file
	string encoded=sha256.encoding(query);
	int pos=sha256.hash(encoded);

	unsigned char* bin=sha256.uchar_binary(encoded);

	cout << encoded << endl;
	cout << data[pos] << endl;
	cout << bin << endl;

	cout << sha256.search(sha256.uchar_binary(data[pos]),bin,encoded.length()/8) << endl;	//we need to ensure that each entry size is multiple of 8

	return 0;
}