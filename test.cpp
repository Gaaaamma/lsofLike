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
string extractInput(char buffer[bufferSize],int readCount);
vector<string> extractMEM(vector<string> mVec);
int main(int argc, char* argv[]){
    int fd =0;
    if( (fd = open(argv[1],O_RDONLY)) != -1 ){
        char buf[bufferSize];
        int counter = 1;
        int length = 0;
        string temp ="";

        while( (length =read(fd,buf,bufferSize)) !=0 ){
            temp += string(buf);
            memset(buf, 0 ,sizeof(buf));
        }
        //cout << temp <<endl ;
        // stringstream
        vector<string> mVec;
        stringstream ss;
        ss << temp;
        string aWord ="";
        while(ss >> aWord){
            mVec.push_back(aWord);
            counter ++;
        } 
        ss.str("");
        ss.clear();

        // Handling
        vector<string> result = extractMEM(mVec);
        for(int i=0;i<result.size();){
            cout << result[i] <<" : " << result[i+1] <<endl;
            i+=2;
        }
    }else{
        cout << "Error: ";
        switch(errno){
            case EACCES:
                cout << "Permission denied\n";
                break;
            default:
                cout << "others\n";
                break;
        }
    }

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
        if(mVec[i].find(":") != string::npos){
            // Find xx:xx
            if( ( (i+2) < mVec.size() ) && (mVec[i+1] != "0") && (mVec[i+1] != preINode ) ){
                // new Inode && new fileName
                preINode = mVec[i+1];
                result.push_back(mVec[i+1]);
                result.push_back(mVec[i+2]);
            }
        }
    }
    return result;
}
