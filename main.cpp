#include <iostream>
#include "filterFramework.h"

using namespace std;

int main(int argc, char* argv[])
{
    Options opts;
    if (opts.parse(argc, argv) != 0)
    {
        return -1;
    }
    
    FilterFramework framework(opts);
    if (framework.init() != 0)
    {
        return -1;
    }

    framework.run();
	return 0;
}

