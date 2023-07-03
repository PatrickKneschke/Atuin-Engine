
#include "App.h"

#include <cstdlib>
#include <iostream>


int main() {

    try
    {
        Atuin::App::Get().Start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return EXIT_SUCCESS;
}