
#pragma once


#include "Core/Config/CVar.h"
#include "Core/Util/Types.h"

#include <boost/fiber/all.hpp>
#include <thread>


namespace Atuin {


class JobDecl {

};


class JobManager {

public:

    JobManager();
    ~JobManager();


    void StartUp();
    void ShutDown();

private:

    static CVar<U8>* pMaxFibersOnThread;

    // TODO Array of std::thread
    // TODO Queues of JobDecl
};

    
} // Atuin
 