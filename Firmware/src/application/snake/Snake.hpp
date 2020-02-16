#ifndef APPLICATION_SNAKE_HPP_
#define APPLICATION_SNAKE_HPP_

#include "application/Application.hpp"
#include "types/Color.h"
#include <freertos/thread.hpp>
#include <etl/array.h>

namespace grid
{
    class GridInterface;
    class AdditionalButtonsInterface;
}

namespace lcd
{
    class LcdInterface;
}

namespace application
{

enum Direction : uint8_t
{
    Direction_UP = 0,
    Direction_DOWN,
    Direction_LEFT,
    Direction_RIGHT,
    numberOfDirections
};

class Coordinates
{
public:
    int8_t x;
    int8_t y;

    Coordinates operator+(const Coordinates& coords) const
    {
        Coordinates sum;
        sum.x = this->x + coords.x;
        sum.y = this->y + coords.y;
        return sum;
    };

    bool operator==(const Coordinates& coords) const
    {
        bool equal = false;

        if ((this->x == coords.x) && (this->y == coords.y))
        {
            equal = true;
        }

        return equal;
    };
};

static const uint8_t gridHeigth = 8;
static const uint8_t gridWidth = 8;
static const uint8_t gridSize = gridHeigth * gridWidth;

class Snake : public Application
{
public:
    Snake( ApplicationController& applicationController, grid::GridInterface& grid, lcd::LcdInterface& lcd );

private:
    void run( ApplicationThread& thread );

    void handleAdditionalButtonEvent( const additional_buttons::Event& event );
    void handleGridButtonEvent( const grid::ButtonEvent& event );

    void advance();
    void blink() const;
    void updateGrid() const;
    void updateLcd() const;
    void feed( const Coordinates headCoords );
    void move( const Coordinates headCoords );
    void startNewGame();
    void placeFood();

    bool areCoordinatesValid( const Coordinates coords ) const;
    bool isSnakeInGivenCoordinates( const Coordinates coords ) const;
    uint8_t getScore() const;

    grid::GridInterface& grid_;
    lcd::LcdInterface& lcd_;

    bool applicationEnded_;
    bool gameInProgress_;

    Direction direction_;
    Direction directionCandidate_;
    etl::array<Coordinates, gridSize> snake_;
    uint8_t length_;
    uint8_t bestScore_;
    uint16_t stepPeriodMs_;
    Color bodyColor_;
    Coordinates food_;
};

} // namespace

#endif // APPLICATION_SNAKE_HPP_
