#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "thread.hpp"
#include <initializer_list>

enum ApplicationSwitch
{
    ApplicationSwitch_UNSPECIFIED = 0,
    ApplicationSwitch_INTERNAL_MENU,
    ApplicationSwitch_LAUNCHPAD
};

class ApplicationBase
{
public:
    ApplicationBase():
        inputHandlerCount_( 0 )
    {
    }

    void initializeInputHandlers( std::initializer_list<freertos::Thread*> inputHandlerList )
    {
        inputHandlerCount_ = 0;

        for ( freertos::Thread* handler : inputHandlerList )
        {
            inputHandler_[inputHandlerCount_] = handler;
            inputHandlerCount_++;
        }

        for (uint8_t handlerIndex = 0; handlerIndex < inputHandlerCount_; handlerIndex++)
        {
            inputHandler_[handlerIndex]->Start();
            inputHandler_[handlerIndex]->Suspend();
        }
    }

    void openApplication()
    {
        initialize();

        for (uint8_t handlerIndex = 0; handlerIndex < inputHandlerCount_; handlerIndex++)
        {
            inputHandler_[handlerIndex]->Resume();
        }
    }

    void closeApplication()
    {
        deinitialize();

        for (uint8_t handlerIndex = 0; handlerIndex < inputHandlerCount_; handlerIndex++)
        {
            inputHandler_[handlerIndex]->Suspend();
        }
    }

    virtual void initialize()
    {
        // do nothing by default, this method is to be overriden
    }

    virtual void deinitialize()
    {
        // do nothing by default, this method is to be overriden
    }

private:
    static const uint8_t kMaximumInputHandlerCount = 4;
    freertos::Thread* inputHandler_[kMaximumInputHandlerCount];
    uint8_t inputHandlerCount_;
};

#endif // APPLICATION_H_