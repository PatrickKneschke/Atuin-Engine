
#include "Application.h"

#include <cstdlib>
#include <iostream>


int main() {

    try
    {
        Atuin::Application::Start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return EXIT_SUCCESS;
}