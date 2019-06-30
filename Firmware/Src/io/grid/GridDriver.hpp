#ifndef GRID_GRIDDRIVER_HPP_
#define GRID_GRIDDRIVER_HPP_

#include "Types.h"
#include "thread.hpp"

namespace grid
{

class GridDriver
{
public:

    GridDriver();
    ~GridDriver();

    void addThreadToNotify( freertos::Thread* const thread );

    bool getButtonInput( const uint8_t button ) const;
    uint8_t getGridButtonInput( const uint8_t column ) const;
    uint8_t getRotaryEncodersInput( const uint8_t encoder, const uint8_t timeStep ) const;
    void initialize();

    static inline void inputReadoutToMemory0CompleteCallback()
    {
        currentlyStableInputBufferIndex_ = 0;
        switchInputUpdated_ = true;
        callNotifications();
    }

    static inline void inputReadoutToMemory1CompleteCallback()
    {
        currentlyStableInputBufferIndex_ = 1;
        switchInputUpdated_ = true;
        callNotifications();
    }

    static inline void notifyOnBufferChange()
    {
        static uint32_t previousChecksum = 0xFFFFFFFF; 
        const uint32_t currentChecksum = getInputBufferChecksum();
        if (currentChecksum != previousChecksum)
        {
            previousChecksum = currentChecksum;
            callNotifications();
        }
    }

    static inline uint32_t getInputBufferChecksum()
    {
        static const uint32_t numberOfElements = sizeof( buttonInput_ ) / sizeof( uint32_t );
        static const uint32_t* const element = &buttonInput_[0][0];
        uint32_t checksum = 0;

        for (uint8_t index = 0; index < numberOfElements; index++)
        {
            checksum ^= element[index];
        }

        return checksum;
    }

    static inline void callNotifications()
    {
        for (uint8_t index = 0; index < numberOfThreadsToNotify_; index++)
        {
            threadToNotify_[index]->NotifyFromISR();
        }
    }

    bool isButtonInputStable( const uint8_t button ) const;
    bool isGridVerticalSegmentInputStable( const uint8_t column ) const;

    bool isSwitchInputUpdated() const;
    void resetSwitchInputUpdatedFlag();

    void setLedColor( uint8_t ledPositionX, const uint8_t ledPositionY, const bool directLed, const Color color );
    void start();
    void turnAllLedsOff();

    static const uint8_t numberOfHorizontalSegments = 4;
    static const uint8_t numberOfVerticalSegments = 20;
    static const uint8_t numberOfButtonDebouncingCycles = 2;

    static const uint8_t ledColorIntensityMaximum = 64;
    static const uint8_t ledColorIntensityOff = 0;
private:

    void initializeBaseTimer();
    void initializeDma();
    void initializeGpio();
    void initializePwmGpio();
    void initializePwmTimers();

    static uint8_t currentlyStableInputBufferIndex_;
    static bool switchInputUpdated_;

    static uint32_t buttonInput_[numberOfButtonDebouncingCycles][numberOfVerticalSegments];
    static uint32_t pwmOutputRed_[numberOfVerticalSegments][numberOfHorizontalSegments];
    static uint32_t pwmOutputGreen_[numberOfVerticalSegments][numberOfHorizontalSegments];
    static uint32_t pwmOutputBlue_[numberOfVerticalSegments][numberOfHorizontalSegments];

    static const uint8_t kMaximumNumberOfThreadsToNotify = 3;
    static freertos::Thread* threadToNotify_[kMaximumNumberOfThreadsToNotify];
    static uint8_t numberOfThreadsToNotify_;
};

} // namespace grid
#endif // GRID_GRIDDRIVER_HPP_
