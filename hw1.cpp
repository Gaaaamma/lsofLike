#include <iostream>
#include <regex>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string>

using namespace std;

string typeCheck(dirent* dirp);

int main(int argc, char* argv[]){
	string defPath = "/proc/";

	DIR* dp;
	struct dirent* dirp;
	int counter =0;
	regex  procReg("^[0-9]+$");	

	cout << "COMMAND\t\tPID\t\tUSER\t\tFD\t\tTYPE\t\tNODE\t\tNAME\n";

	if((dp = opendir(defPath.c_str())) != NULL){
		// Got something
		while((dirp = readdir(dp)) != NULL){
			// only DIR && number can to go ahead
			if(typeCheck(dirp) == "DIR" && regex_search(dirp->d_name,procReg)){
				// Handle command
				string procPath = defPath + dirp->d_name ;

				// Handle PID
				cout << temp << "\t\t" << dirp->d_name <<endl;

				// Handle USER

				// Handle FD

				// Handle TYPE

				// Handle NODE

				// Handle NAME

				counter ++;
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
	cout << counter <<endl;
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
