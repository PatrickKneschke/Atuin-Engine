
#include "App.h"

#include <cstdlib>
#include <iostream>


int main() {

    try
    {
        Atuin::App app;
        app.StartUp();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return EXIT_SUCCESS;
}