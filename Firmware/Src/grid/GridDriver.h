#ifndef GRID_GRIDDRIVER_H_
#define GRID_GRIDDRIVER_H_

#include "Types.h"
#include <functional>

namespace grid
{

class GridDriver
{
public:

    GridDriver();
    ~GridDriver();

    void addNotificationCallback( std::function<void()> callback );

    bool getButtonInput( const uint8_t button ) const;
    uint8_t getGridButtonInput( const uint8_t column ) const;
    uint8_t getRotaryEncodersInput( const uint8_t encoder, const uint8_t timeStep ) const;
    void initialize();

    static inline void inputReadoutToMemory0CompleteCallback()
    {
        currentlyStableInputBufferIndex_ = 0;
        gridInputUpdated_ = true;
        switchInputUpdated_ = true;
        callNotifications();
    }

    static inline void inputReadoutToMemory1CompleteCallback()
    {
        currentlyStableInputBufferIndex_ = 1;
        gridInputUpdated_ = true;
        switchInputUpdated_ = true;
        callNotifications();
    }

    static inline void callNotifications()
    {
        for (uint8_t index = 0; index < numberOfNotifications_; index++)
        {
            notify_[index]();
        }
    }

    bool isButtonInputStable( const uint8_t button ) const;
    bool isGridVerticalSegmentInputStable( const uint8_t column ) const;

    bool isGridInputUpdated() const;
    bool isSwitchInputUpdated() const;
    void resetGridInputUpdatedFlag();
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
    static bool gridInputUpdated_;
    static bool switchInputUpdated_;

    static uint32_t buttonInput_[numberOfButtonDebouncingCycles][numberOfVerticalSegments];
    static uint32_t pwmOutputRed_[numberOfVerticalSegments][numberOfHorizontalSegments];
    static uint32_t pwmOutputGreen_[numberOfVerticalSegments][numberOfHorizontalSegments];
    static uint32_t pwmOutputBlue_[numberOfVerticalSegments][numberOfHorizontalSegments];

    static const uint8_t kMaximumNumberOfNotifications = 3;
    static std::function<void(void)> notify_[kMaximumNumberOfNotifications];
    static uint8_t numberOfNotifications_;
};

} // namespace grid
#endif // GRID_GRIDDRIVER_H_
