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
int main(int argc, char* argv[]){  
    string myTest = "abcdefg (deleted)";
    string subTest = myTest.substr(0,myTest.find("(deleted)"));
    cout << subTest <<"|\n";
}
