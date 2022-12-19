#include "node.h"
using namespace std;
FILE *p;
bool debug = 0;

int main(int argc, char *argv[])
{

    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") == 0 ||
            strcmp(argv[1], "-h") == 0)
            cout << "usage: turing [-v|--verbose] [-h | --help]<tm><input> " << endl;    
                return 0;
    }
    //  cout << "This is for testing" << endl;
    else if (argc == 3)
    {
        p = fopen(argv[1], "r");
        read();
        run(argv[2]);
    }
    else if (argc == 4)
    {
        if (strcmp(argv[1], "--verbose") == 0 ||
            strcmp(argv[1], "-v") == 0)
            debug = true;
        p = fopen(argv[2], "r");
        read();
        run(argv[3]);
    }
    return 0;
}