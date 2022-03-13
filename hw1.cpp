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
#include <pwd.h>
#include <grp.h>
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
				string procPath = defPath + dirp->d_name ;
				string procPathComm = procPath + "/comm";
				string procPathCWD = procPath + "/cwd";
				string procPathROOT = procPath + "/root";
				string procPathEXE = procPath + "/exe";
				string procPathMAP = procPath + "/maps";
				string procPathFD = procPath + "/fd";

				char buffer[bufferSize]={};
				int procFd =0;
				
				// Handle COMMAND
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
				/***************************** CWD ***********************************/
				// Sequence = cwd -> root -> exe -> (maps) -> fd
				// Handle FD (/proc/{pid}/fd)
				outputMap["FD"] = "cwd";

				struct stat statBuffer;
				if( stat(procPathCWD.c_str(),&statBuffer) != -1 ){
					// Handle USER
					struct passwd* pw = getpwuid(statBuffer.st_uid);
					outputMap["USER"] = pw->pw_name;

					// Handle TYPE
					switch(statBuffer.st_mode & S_IFMT){
						case S_IFDIR: outputMap["TYPE"] = "DIR"; break;
						case S_IFREG: outputMap["TYPE"] = "REG"; break;
						case S_IFCHR: outputMap["TYPE"] = "CHR"; break;
						case S_IFIFO: outputMap["TYPE"] = "FIFO"; break; 
						case S_IFSOCK: outputMap["TYPE"] = "SOCK"; break;
						default: outputMap["TYPE"] = "unknown"; break;
					}
					// Handle NODE
					outputMap["NODE"] = to_string(statBuffer.st_ino);
					
					// Handle NAME (readlink)
					char linkName[bufferSize];
					int linkNameLength =readlink(procPathCWD.c_str(),linkName,bufferSize);
					outputMap["NAME"] = extractInput(linkName,linkNameLength);

				}else{
					// Error -> maybe permission denied
					switch(errno){
						case EACCES:
							outputMap["USER"] = "?"; // UNDO
							outputMap["FD"] = "cwd";
							outputMap["TYPE"] = "unknown";
							outputMap["NODE"] = "";
							outputMap["NAME"] = procPathCWD + " (Permission denied)";
							break;
					}
				}
				traverseMap(outputMap);				
				/**********************************************************************/

				/***************************** ROOT ***********************************/
				// ...
				/**********************************************************************/

				/***************************** EXE ***********************************/
				// ...
				/**********************************************************************/

				/***************************** MAPS ***********************************/
				// ...
				/**********************************************************************/

				/***************************** fd ***********************************/
				// ...
				/**********************************************************************/
			}
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
	cout << mMap["COMMAND"] <<"\t\t";
	cout << mMap["PID"] <<"\t\t";
	cout << mMap["USER"] <<"\t\t";
	cout << mMap["FD"] <<"\t\t";
	cout << mMap["TYPE"] <<"\t\t";
	cout << mMap["NODE"] <<"\t\t";
	cout << mMap["NAME"] <<"\t\t";
	cout << endl;
} 
