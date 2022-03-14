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
            string procPathFD = "/proc/1/fd";
            map<string,string> outputMap;
            /***************************** fd ***********************************/
            DIR* dp_fd;
            struct dirent* dirp_fd;
            if((dp_fd = opendir(procPathFD.c_str())) != NULL){
                regex  fdReg("^[0-9]+$");	
                while((dirp_fd = readdir(dp_fd)) != NULL){
                    // Each fd in /proc/{pid}/fd
                    // only number can go ahead
                    if(regex_search(dirp_fd->d_name,fdReg)){
                        // Handling USER (stat)
                        struct stat statBuffer;
				        if( stat( (procPathFD+"/"+dirp_fd->d_name).c_str(),&statBuffer) != -1 ){
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
					        int linkNameLength =readlink( (procPathFD+"/"+dirp_fd->d_name).c_str(),linkName,bufferSize);
					        outputMap["NAME"] = extractInput(linkName,linkNameLength);
                            traverseMap(outputMap);
				        }
                        // Handling FD
                        
                    }else{
                        cout << "NO MATCH: " << dirp_fd->d_name << endl;
                    }
                }

            }else{
                // Can't access
                cout << "Can't Access\n";
                struct stat statBuffer;
                switch(errno){
                    case EACCES:
                        // Get previous dir user
                        if( stat(procPathFD.c_str(),&statBuffer) != -1 ){
                            struct passwd* pw = getpwuid(statBuffer.st_uid);
                            outputMap["USER"] = pw->pw_name;
                        }else{
                            outputMap["USER"] = "root";
                        }
                        outputMap["FD"] = "NOFD";
                        outputMap["TYPE"] = "";
                        outputMap["NODE"] = "";
                        outputMap["NAME"] = procPathFD + " (Permission denied)";
                        break;
                    case ENOENT:
                        cout << "Directory does not exist\n";
                        break;
                    case ENOTDIR:
                        cout << procPathFD <<" is not a directory\n";
                        break;
                }
            }
            //traverseMap(outputMap);
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
