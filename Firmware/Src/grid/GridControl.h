#ifndef GRID_BUTTONS_GRIDCONTROL_H_
#define GRID_BUTTONS_GRIDCONTROL_H_

#include "Types.h"

namespace grid
{

class GridControl
{
public:

    GridControl();
    ~GridControl();

    bool getButtonInput( const uint8_t button ) const;
    uint8_t getGridButtonInput( const uint8_t column ) const;
    uint8_t getRotaryEncodersInput( const uint8_t encoder, const uint8_t timeStep ) const;
    void initialize();

    static inline void inputReadoutToMemory0CompleteCallback()
    {
        currentlyStableInputBufferIndex_ = 0;
        gridInputUpdated_ = true;
        switchInputUpdated_ = true;
    }

    static inline void inputReadoutToMemory1CompleteCallback()
    {
        currentlyStableInputBufferIndex_ = 1;
        gridInputUpdated_ = true;
        switchInputUpdated_ = true;
    }

    bool isButtonInputStable( const uint8_t button ) const;
    bool isGridVerticalSegmentInputStable( const uint8_t column ) const;

    bool isGridInputUpdated() const;
    bool isSwitchInputUpdated() const;
    void resetGridInputUpdatedFlag();
    void resetSwitchInputUpdatedFlag();

    void setLedColour( uint8_t ledPositionX, const uint8_t ledPositionY, const bool directLed, const Colour colour );
    void start();
    void turnAllLedsOff();

    static const uint8_t numberOfHorizontalSegments = 4;
    static const uint8_t numberOfVerticalSegments = 20;
    static const uint8_t nNumberOfButtonDebouncingCycles = 2;

    static const uint8_t ledColourIntensityMaximum = 64;
    static const uint8_t ledColourIntensityOff = 0;
private:

    void initializeBaseTimer();
    void initializeDma();
    void initializeGpio();
    void initializePwmGpio();
    void initializePwmTimers();

    static uint8_t currentlyStableInputBufferIndex_;
    static bool gridInputUpdated_;
    static bool switchInputUpdated_;

    static uint32_t buttonInput_[nNumberOfButtonDebouncingCycles][numberOfVerticalSegments];
    static uint32_t pwmOutputRed_[numberOfVerticalSegments][numberOfHorizontalSegments];
    static uint32_t pwmOutputGreen_[numberOfVerticalSegments][numberOfHorizontalSegments];
    static uint32_t pwmOutputBlue_[numberOfVerticalSegments][numberOfHorizontalSegments];
};

} // namespace grid
#endif // GRID_BUTTONS_GRIDCONTROL_H_
