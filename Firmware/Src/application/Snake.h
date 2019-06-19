#ifndef APPLICATION_SNAKE_H_
#define APPLICATION_SNAKE_H_

#include "application/Application.h"
#include "Types.h"
#include "thread.hpp"

namespace grid
{
    class Grid;
    class AdditionalButtons;
}

namespace lcd
{
    class Lcd;
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
    Snake( ApplicationController& applicationController, grid::Grid& grid, lcd::Lcd& lcd );

private:
    void run( ApplicationThread& thread );

    void handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event );
    void handleGridButtonEvent( const grid::Grid::ButtonEvent event );

    void advance();
    void blink();
    void displayGame();
    void feed( const Coordinates headCoords );
    void move( const Coordinates headCoords );
    void startNewGame();
    void placeFood();

    bool areCoordinatesValid( const Coordinates coords ) const;
    bool isSnakeInGivenCoordinates( const Coordinates coords ) const;

    grid::Grid& grid_;
    lcd::Lcd& lcd_;

    bool applicationEnded_;
    bool gameInProgress_;

    Direction direction_;
    Direction directionCandidate_;
    Coordinates snake_[gridSize];
    uint8_t length_;
    uint16_t stepPeriodMs_;
    Color bodyColor_;
    Coordinates food_;
};

} // namespace

#endif // APPLICATION_SNAKE_H_