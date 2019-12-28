#include "application/snake/Snake.hpp"

#include "io/lcd/LcdInterface.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace application
{

static const uint16_t offGameBlinkPeriodMs = 300;
static const uint16_t initialStepPeriodMs = 1000;
static const uint16_t stepPeriodDecreaseMs = 16;

static const Coordinates initialSnake[2] = { {7, 1}, {7, 0} };
static const uint8_t initialSnakeLength = 2;

static const Color headColor = color::RED;
static const Color initialBodyColor = color::GREEN; 
static const Color foodColor = color::YELLOW;

static const Coordinates directionVector[numberOfDirections] = { {0, 1}, {0, -1}, {-1, 0}, {1, 0} };

Snake::Snake( ApplicationController& applicationController, grid::GridInterface& grid, lcd::LcdInterface& lcd ):
    Application( applicationController ),
    grid_( grid ),
    lcd_( lcd ),
    applicationEnded_( false ),
    gameInProgress_( false ),
    direction_( Direction_UP ),
    directionCandidate_( Direction_UP ),
    length_( initialSnakeLength ),
    bestScore_( getScore() ),
    stepPeriodMs_( initialStepPeriodMs ),
    bodyColor_( initialBodyColor )
{
}

void Snake::run( ApplicationThread& thread )
{
    enableAdditionalButtonInputHandler();
    enableGridInputHandler();

    // initialize all variables for a new game, but put the game on hold
    startNewGame();
    gameInProgress_ = false;

    applicationEnded_ = false;
    while (!applicationEnded_)
    {
        if (gameInProgress_)
        {
            thread.delay( stepPeriodMs_ );
            advance();
        }
        else
        {
            thread.delay( offGameBlinkPeriodMs );
            blink();
        }
    }
}

void Snake::handleGridButtonEvent( const grid::ButtonEvent event )
{
    if (gameInProgress_)
    {
        if (ButtonAction::PRESSED == event.action)
        {
            if ((grid::button::ARROW_UP == event.coordinates) && (Direction_DOWN != direction_))
            {
                directionCandidate_ = Direction_UP;
            }
            else if ((grid::button::ARROW_DOWN == event.coordinates) && (Direction_UP != direction_))
            {
                directionCandidate_ = Direction_DOWN;
            }
            else if ((grid::button::ARROW_LEFT == event.coordinates) && (Direction_RIGHT != direction_))
            {
                directionCandidate_ = Direction_LEFT;
            }
            else if ((grid::button::ARROW_RIGHT == event.coordinates) && (Direction_LEFT != direction_))
            {
                directionCandidate_ = Direction_RIGHT;
            }
        }
    }
    else
    {
        // only restart game, when non-arrow button is released
        if ((ButtonAction::RELEASED == event.action) && (event.coordinates.x < 9))
        {
            startNewGame();
        }
    }
}

void Snake::handleAdditionalButtonEvent( const additional_buttons::Event event )
{
    if ((additional_buttons::Button::internalMenu == event.button) && (ButtonAction::PRESSED == event.action))
    {
        applicationEnded_ = true;
        switchApplication( ApplicationIndex_INTERNAL_MENU );
    }
}

void Snake::advance()
{
    direction_ = directionCandidate_;
    const Coordinates head = snake_[0] + directionVector[direction_];

    if ((!areCoordinatesValid( head )) || isSnakeInGivenCoordinates( head ))
    {
        // game over
        gameInProgress_ = false;
        if (getScore() > bestScore_)
        {
            bestScore_ = getScore();
            updateLcd();
        }
    }
    else if (head == food_)
    {
        // feed
        feed( head );

        if (length_ < gridSize)
        {
            placeFood();
        }
        else
        {
            gameInProgress_ = false;
            bestScore_ = getScore();
        }
        updateLcd();
    }
    else
    {
        // simply move
        move( head );
    }

    updateGrid();
}

void Snake::blink() const
{
    static bool lightUp = true;

    if (lightUp)
    {
        updateGrid();
    }
    else
    {
        grid_.turnAllLedsOff();
    }
    
    lightUp = !lightUp;
}

void Snake::updateGrid() const
{
    grid_.turnAllLedsOff();
    grid_.setLed( {snake_[0].x, snake_[0].y}, headColor );
    for (uint8_t i = 1; i < length_; i++)
    {
        grid_.setLed( {snake_[i].x, snake_[i].y}, bodyColor_ );
    }

    grid_.setLed( {food_.x, food_.y}, foodColor );
}

void Snake::updateLcd() const
{
    lcd_.clear();
    lcd_.print( "Snake", lcd_.horizontalCenter(), 0, lcd::Justification::CENTER );
    lcd_.print( "Score:", 0, 16 );
    lcd_.printNumberInBigDigits( getScore(), lcd_.width() - 1, 16, lcd::Justification::RIGHT );
    
    char bestScoreString[16] = {};
    std::sprintf( bestScoreString, "Best: %i", bestScore_ );
    lcd_.print( bestScoreString, lcd_.horizontalCenter(), 40, lcd::Justification::CENTER );
}

void Snake::feed( const Coordinates headCoords )
{
    for (uint8_t i = length_; i > 0; i--)
    {
        snake_[i] = snake_[i - 1];
    }

    length_++;
    snake_[0] = headCoords;

    // increase speed and change color, when feeding
    stepPeriodMs_ = stepPeriodMs_ - stepPeriodDecreaseMs;

    bodyColor_ = bodyColor_ + Color( 0, 0, 1 );
}

void Snake::move( const Coordinates headCoords )
{
    for (uint8_t i = length_; i > 0; i--)
    {
        snake_[i] = snake_[i - 1];
    }

    snake_[0] = headCoords;
}

void Snake::startNewGame()
{
    std::memcpy( snake_, initialSnake, sizeof( Coordinates ) * initialSnakeLength );
    length_ = initialSnakeLength;
    bodyColor_ = initialBodyColor;
    direction_ = Direction_UP;
    directionCandidate_ = Direction_UP;
    stepPeriodMs_ = initialStepPeriodMs;

    placeFood();
    gameInProgress_ = true;
    updateGrid();
    updateLcd();
}

void Snake::placeFood()
{
    Coordinates candidateCoords = {};

    do
    {
        candidateCoords.x = std::rand() % gridWidth;
        candidateCoords.y = std::rand() % gridHeigth;
    } while (isSnakeInGivenCoordinates( candidateCoords ));

    food_ = candidateCoords;
}

bool Snake::areCoordinatesValid( const Coordinates coords ) const
{
    bool coordinatesAreValid = false;

    do
    {
        if ((coords.x < 0) || (coords.x >= gridWidth))
        {
            break;
        }
        if ((coords.y < 0) || (coords.y >= gridHeigth))
        {
            break;
        }

        coordinatesAreValid = true;
    } while (false);

    return coordinatesAreValid;
}

bool Snake::isSnakeInGivenCoordinates( const Coordinates coords ) const
{
    bool snakeIsInCoordinates = false;
    for (uint8_t i = 0; i < length_; i++)
    {
        if (coords == snake_[i])
        {
            snakeIsInCoordinates = true;
            break;
        }
    }

    return snakeIsInCoordinates;
}

uint8_t Snake::getScore() const
{
    uint8_t score = 0;
    if (length_ > 0)
    {
        score = length_ - 1;
    }

    return score;
}

} // namespace
