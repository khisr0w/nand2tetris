#include <iostream>
#include <fstream>

class parser {

    enum class ECommandType {
	A_COMMAND,
	C_COMMAND,
	L_COMMAND
    };

public:

    parser (std::string);

    bool hasMoreCommands ();
    void advance ();
    ECommandType commandType ();
    std::string symbol ();
    std::string dest ();
    std::string comp ();
    std::string jump ();

    bool isFileFound;
    std::ifstream file;
};
