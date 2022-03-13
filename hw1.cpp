#include <iostream>
#include <regex>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <map>
#define bufferSize 256

using namespace std;

string typeCheck(dirent* dirp);
string extractInput(char buffer[bufferSize],int readCount);
void traverseMap(map<string,string> mMap); 

int main(int argc, char* argv[]){
	string defPath = "/proc/";

	DIR* dp;
	struct dirent* dirp;
	regex  procReg("^[0-9]+$");	
	map<string,string> outputMap;

	cout << "COMMAND\t\tPID\t\tUSER\t\tFD\t\tTYPE\t\tNODE\t\tNAME\n";

	if((dp = opendir(defPath.c_str())) != NULL){
		// Got something
		while((dirp = readdir(dp)) != NULL){
			// only DIR && number can go ahead
			if(typeCheck(dirp) == "DIR" && regex_search(dirp->d_name,procReg)){
				// Handle command
				string procPath = defPath + dirp->d_name ;
				string procPathComm = procPath + "/comm";
				char buffer[bufferSize]={};
				int procFd =0;

				if((procFd = open(procPathComm.c_str(),O_RDONLY)) != -1){
					int length = read(procFd,buffer,sizeof(buffer));
					outputMap["COMMAND"] = extractInput(buffer,length);
					close(procFd);

				}else{
					// error occur
					cout << "open("+procPath+",O_RDONLY) FAIL\n";
				}
				// Handle PID
				outputMap["PID"] = dirp->d_name ;

				// Handle USER

				// Handle FD

				// Handle TYPE

				// Handle NODE

				// Handle NAME
				traverseMap(outputMap);				
			}
			/*
			cout << "inode number: " << dirp->d_ino <<endl;
			cout << "type:         " << typeCheck(dirp) <<endl;
			cout << "name:         " << dirp->d_name <<endl ;
			cout << endl;
			*/
		}
		
		if(closedir(dp) ==-1){
			cout << "closeDir error\n";
		}

	}else{
		// Can't access
		switch(errno){
			case EACCES:
				cout << "Permission denied\n";
				break;
			case ENOENT:
				cout << "Directory does not exist\n";
				break;
			case ENOTDIR:
				cout << argv[1] <<" is not a directory\n";
		}
	}
	return 0;
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
void traverseMap(map<string,string> mMap){
	map<string,string>::iterator iter = mMap.begin();
	while(iter != mMap.end()){
		cout << iter->second <<"\t\t";
		iter ++;
	}
	cout << endl;
} 
