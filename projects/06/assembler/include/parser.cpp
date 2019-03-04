#include "parser.h"

parser::parser (std::string path) {
    
    file.open (path);

    isFileFound = file.is_open();
}
