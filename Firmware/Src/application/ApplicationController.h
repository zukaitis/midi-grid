#ifndef APPLICATION_CONTROLLER_H_
#define APPLICATION_CONTROLLER_H_

#include "thread.hpp"

class ApplicationController : freertos::Thread
{
public:
    void Run();

private:
    Application* application_[kNumberOfApplications];
    Application* currentlyOpenApplication_;
};

#endif // APPLICATION_CONTROLLER_H_