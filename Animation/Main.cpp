// Termm--Winter 2021

#include <iostream>
#include <sys/resource.h>
#include "scene_lua.hpp"

int main(int argc, char** argv)
{
    rlimit lim;
    getrlimit(RLIMIT_CPU, &lim);
    lim.rlim_cur *= 10;
    lim.rlim_max *= 10;
    setrlimit(RLIMIT_CPU, &lim);
    std::cout << "limits: " << lim.rlim_max  << std::endl;


    std::string filename = "Assets/simple.lua";
    if (argc >= 2) {
        filename = argv[1];
    }

    if (!run_lua(filename)) {
        std::cerr << "Could not open " << filename <<
            ". Try running the executable from inside of" <<
            " the Assets/ directory" << std::endl;
        return 1;
    }
}
