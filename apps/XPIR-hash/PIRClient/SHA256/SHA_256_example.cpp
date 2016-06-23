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

	for(int i=0;i<entries.size();i++){
		int pos=sha256.hash(entries[i]);
		if(data[pos]!=""){
			data[pos]+="->";
		}
		data[pos]+=entries[i];
	}
	//sha256.printVector(data);

	string query="1	73931850	rs11210256	G	A	.	.	SVTYPE=SNP;END=73931851"; //is in file
	//string query="1	118737704	rs3085974	 	ACTCTGT	.	.	SVTYPE=INS;END=118737704"; //is in file
	//string query="2	73931850	rs11210256	G	A	.	.	SVTYPE=SNP;END=73931851"; //is not in file

	int pos=sha256.hash(query);
	cout << sha256.search(data[pos],query) << endl;

	return 0;
}