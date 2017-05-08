#include <iostream>
#include <fstream>

using namespace std;


int main(){

	char data[100];

	ifstream infile;
	infile.open("/home/ipung/Downloads/CppMT-master/file/inputpir.txt");

	cout << "Reading from the file" <<endl;
	infile >> data;
	cout << data <<endl;

	infile.close();

	return  0;

}