
#include "JobManager.h"
#include "Core/Config/ConfigManager.h"


namespace Atuin {
 

CVar<U8>* JobManager::pMaxFibersOnThread = ConfigManager::RegisterCVar("Multithreading", "MAX_FIBERS_ON_THREAD", (U8)16);


JobManager::JobManager() {

}


JobManager::~JobManager() {

}


void JobManager::StartUp() {

}


void JobManager::ShutDown() {

}


} // Atuin
