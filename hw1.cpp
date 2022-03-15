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
#include <sstream>
#define bufferSize 256

using namespace std;

string typeCheck(dirent* dirp);
string extractInput(char buffer[bufferSize],int readCount);
vector<string> extractMEM(vector<string> mVec);
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
			// only DIR && number can go ahead ex: /proc/1 , /proc/2 , /proc/3 ...
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
					// /proc/{pid} may die -> Escape this turn
					continue;
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
					struct stat statBuffer;
					switch(errno){
						case EACCES:
							// Get previous dir user
							if( stat(procPath.c_str(),&statBuffer) != -1 ){
								struct passwd* pw = getpwuid(statBuffer.st_uid);
								outputMap["USER"] = pw->pw_name;
							}else{
								outputMap["USER"] = "root";
							}
							outputMap["FD"] = "cwd";
							outputMap["TYPE"] = "unknown";
							outputMap["NODE"] = "";
							outputMap["NAME"] = procPathCWD + " (Permission denied)";
							break;
						default:
							// /proc/{pid} may die
							continue;
							break;
					}
				}
				traverseMap(outputMap);				
				/**********************************************************************/
				/***************************** ROOT ***********************************/
				outputMap["FD"] = "rtd";
				if( stat(procPathROOT.c_str(),&statBuffer) != -1 ){
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
					int linkNameLength =readlink(procPathROOT.c_str(),linkName,bufferSize);
					outputMap["NAME"] = extractInput(linkName,linkNameLength);

				}else{
					// Error -> maybe permission denied
					struct stat statBuffer;
					switch(errno){
						case EACCES:
							// Get previous dir user
							if( stat(procPath.c_str(),&statBuffer) != -1 ){
								struct passwd* pw = getpwuid(statBuffer.st_uid);
								outputMap["USER"] = pw->pw_name;
							}else{
								outputMap["USER"] = "root";
							}
							outputMap["FD"] = "rtd";
							outputMap["TYPE"] = "unknown";
							outputMap["NODE"] = "";
							outputMap["NAME"] = procPathROOT + " (Permission denied)";
							break;
						default:
							// /proc/{pid} may die
							continue;
							break;
					}
				}
				traverseMap(outputMap);	
				/**********************************************************************/
				/***************************** EXE ***********************************/
				outputMap["FD"] = "exe";
				if( stat(procPathEXE.c_str(),&statBuffer) != -1 ){
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
					int linkNameLength =readlink(procPathEXE.c_str(),linkName,bufferSize);
					outputMap["NAME"] = extractInput(linkName,linkNameLength);

				}else{
					// Error -> maybe permission denied
					struct stat statBuffer;
					switch(errno){
						case EACCES:
							// Get previous dir user
							if( stat(procPath.c_str(),&statBuffer) != -1 ){
								struct passwd* pw = getpwuid(statBuffer.st_uid);
								outputMap["USER"] = pw->pw_name;
							}else{
								outputMap["USER"] = "root";
							}
							outputMap["FD"] = "exe";
							outputMap["TYPE"] = "unknown";
							outputMap["NODE"] = "";
							outputMap["NAME"] = procPathEXE + " (Permission denied)";
							break;
						default:
							// /proc/{pid} may die
							continue;
							break;
					}
				}
				traverseMap(outputMap);	
				/**********************************************************************/
				/***************************** MAPS ***********************************/
                int mapfd =0;
				if( (mapfd = open(procPathMAP.c_str(),O_RDONLY)) != -1 ){
                    char buf[bufferSize];
                    string temp ="";

                    while( read(mapfd,buf,bufferSize) !=0 ){
                        temp += string(buf);
                        memset(buf, 0 ,sizeof(buf));
                    }

                    vector<string> mVec;
                    stringstream ss;
                    ss << temp;
                    string aWord ="";
                    while(ss >> aWord){
                        mVec.push_back(aWord);
                    } 
                    ss.str("");
                    ss.clear();

                    // Handling meme result
                    vector<string> memResult = extractMEM(mVec);
                    
                    // Handle stat
				    if( stat(procPathMAP.c_str(),&statBuffer) != -1 ){
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
					    // Handle NODE && Handle Name
                        // Then output for loop
                        for(int i=0;i<memResult.size();){
                            if(memResult[i+1].find("deleted") != string ::npos){
                                outputMap["FD"] = "DEL";
                                // Handle memResult[i+1] 
                                memResult[i+1] = memResult[i+1].substr(0,memResult[i+1].find("(deleted)"));

                            }else{
                                outputMap["FD"] = "mem";
                            }
                            outputMap["NODE"] = memResult[i];
                            outputMap["NAME"] = memResult[i+1];
                            i = i+2;
                            traverseMap(outputMap);
                        }
				    }else{
						// Exclude permission denied -> /proc/{pid} may die;
						continue;
					}

                }else{
                    switch(errno){
                        // Permission denied -> Just Ignore
                        case EACCES: break;
                        default: continue; break;
                    }
                }
				/**********************************************************************/
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

								// Handling FD
								string fdInfoPath = procPath + "/fdinfo/" + dirp_fd->d_name;
								int fdInfoFd =0;
								if((fdInfoFd = open(fdInfoPath.c_str(),O_RDONLY)) != -1 ){
									char buf[bufferSize];
                    				string temp ="";

                    				while( read(fdInfoFd,buf,bufferSize) !=0 ){
                        				temp += string(buf);
                        				memset(buf, 0 ,sizeof(buf));
                    				}

                    				stringstream ss;
                    				ss << temp;
                    				string aWord ="";
									bool findFlags =false;
                    				while(ss >> aWord){
										if(findFlags == true){
											// aWord is flag now
											break;
										}else{
                        					if(aWord.find("flags") != string::npos){
												findFlags = true;
											}
										}
                    				} 
                    				ss.str("");
                    				ss.clear();
									
									// Mask judge aWord -> Append to outputMap["FD"]
									int iFlag = stoi(aWord);
									iFlag = iFlag & O_ACCMODE ;
									string mFlag ="";

									switch(iFlag){
										case O_RDONLY:
											mFlag = "r";
											break;
										case O_WRONLY:
											mFlag = "w";
											break;
										case O_RDWR:
											mFlag = "u";
											break;
									}									
									outputMap["FD"] = dirp_fd->d_name + mFlag; 
								}else{
									// maybe die
									continue;
								}
								close(fdInfoFd);
                            	traverseMap(outputMap);
				        	}
						}
					}
					if(closedir(dp_fd) == -1){
						cout << "closeDir error\n";
					}

				}else{
					// Can't access
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
							traverseMap(outputMap);
							break;
						
						case ENOTDIR:
							cout << procPathFD <<" is not a directory\n";
							break;

						case ENOENT:
							// directory does not exist;
							continue;
							break;
					}
				}
				/**********************************************************************/
			}
		}
		
		if(closedir(dp) ==-1){
			cout << "closeDir error\n";
		}

	}else{ //Can't access /proc/
		switch(errno){
			case EACCES:
				cout << "Permission denied\n";
				break;
			case ENOENT:
				cout << "Directory does not exist\n";
				break;
			case ENOTDIR:
				cout << argv[1] <<" is not a directory\n";
				break;
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

vector<string> extractMEM(vector<string> mVec){
    vector<string> result;
    regex  timeReg("^[0-9]{2}:[0-9][0-9e]$");	

    int index=0;
    // find index of [heap]
    for(int i=index;i<mVec.size();i++){
        if(mVec[i].find("[heap]") != string::npos){
            index =i+1;
            break;
        }
    }

    string preINode = "0";
    for(int i=index;i<mVec.size();i++){
        if( regex_search(mVec[i],timeReg) ){
            // Find xx:xx
            if( ( (i+2) < mVec.size() ) && (mVec[i+1] != "0") && (mVec[i+1] != preINode ) ){
                // new Inode && new fileName
                preINode = mVec[i+1];
                result.push_back(mVec[i+1]);

                // check (deleted)
                if((i+3) < mVec.size() && mVec[i+3].find("deleted") != string::npos){
                    string temp = mVec[i+2] + mVec[i+3];
                    result.push_back(temp);
                }else{
                    result.push_back(mVec[i+2]);
                }
            }
        }
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

