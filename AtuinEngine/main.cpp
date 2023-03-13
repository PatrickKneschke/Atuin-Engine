
#include <cstdlib>
#include <iostream>


int main(int argc, char **argv) {

#ifdef NDEBUG
    std::cout << "release build of " << argv[0] << '\n';
#else
    std::cout << "debug build of " << argv[0] << '\n';
#endif

    return EXIT_SUCCESS;
}