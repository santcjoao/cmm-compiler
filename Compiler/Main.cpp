#include "include/Parser.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Uso: ./compiler nome_arquivo.cmm\n";
        return 1;
    }

    Parser parser(argv[1]);
    parser.run();

    return 0;
}