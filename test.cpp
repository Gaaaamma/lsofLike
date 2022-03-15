#include <iostream>
#include <regex>
#include <sys/types.h>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <map>
#include <pwd.h>
#include <grp.h>
#define bufferSize 256

using namespace std;
string typeCheck(dirent* dirp);
void traverseMap(map<string,string> mMap); 
string extractInput(char buffer[bufferSize],int readCount);

int main(int argc, char* argv[]){  
	cout << O_RDONLY  << endl;
	cout << O_WRONLY << endl;
	cout << O_RDWR << endl;
}

string typeCheck(dirent* dirp){
	string result="";
	switch(dirp->d_type){
		case(DT_DIR):
			result = "DIR";
			break;
		case(DT_REG):
			result = "REG";
			break;
		case(DT_CHR):
			result = "CHR";
			break;
		case(DT_FIFO):
			result = "FIFO";
			break;
		case(DT_SOCK):
			result = "SOCK";
			break;
		default:
			result ="unknown";
			break;
	}
	return result;
}

void traverseMap(map<string,string> mMap){
	cout << mMap["COMMAND"] <<"\t\t";
	cout << mMap["PID"] <<"\t\t";
	cout << mMap["USER"] <<"\t\t";
	cout << mMap["FD"] <<"\t\t";
	cout << mMap["TYPE"] <<"\t\t";
	cout << mMap["NODE"] <<"\t\t";
	cout << mMap["NAME"] <<"\t\t";
	cout << endl;
} 
string extractInput(char buffer[bufferSize],int readCount){
	string result ="";
    for(int i=0;i<readCount;i++){
    	if(buffer[i]=='\n' || buffer[i]=='\r' || buffer[i]=='\0'){
           break ;
    	}
       	result += buffer[i] ;
   	}
   	return result; 
}
