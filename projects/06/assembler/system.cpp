#include <iostream>
#include "include/parser.h"

int main (int argc, char** argv) {

    parser obj (argv[1]);

    if (obj.isFileFound) {
	std::cout<<"File Found!\n";
	std::string temp;
	std::getline(obj.file, temp);
	std::cout<<temp<<std::endl;
    }
    else  {
	std::cout<<"Not Found!\n";
    }
}
