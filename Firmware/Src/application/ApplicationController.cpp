#include "application/ApplicationController.h"

ApplicationController::Run()
{
    while (true)
    {
        // block until notification
        const uint32_t applicationIndex = TakeNotification();

        if (applicationIndex < kNumberOfApplications)
        {
            const Application* const applicationBeingClosed = currentlyOpenApplication_;
            currentlyOpenApplication_ = application_[applicationIndex];
            application_[ApplicationIndex_PREVIOUS] = applicationBeingClosed;
            currentlyOpenApplication_->open();
        }
    }

}