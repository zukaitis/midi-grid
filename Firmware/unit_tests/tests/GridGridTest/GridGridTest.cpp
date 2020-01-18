#include <gtest/gtest.h>

#include "grid/Grid.h"

#include "grid/MockButtonInput.h"
#include "grid/MockLedOutput.h"
#include "grid/MockFlashingLeds.h"
#include "grid/MockPulsingLeds.h"

#include "types/Color.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( GridConstructor, Create )
{
    grid::MockButtonInput mockButtonInput;
    grid::MockLedOutput mockLedOutput;
    grid::MockFlashingLeds mockFlashingLeds;
    grid::MockPulsingLeds mockPulsingLeds;
    grid::Grid grid( mockButtonInput, mockLedOutput, mockFlashingLeds, mockPulsingLeds );

    SUCCEED();
}

TEST( waitForInput, Call )
{
    grid::MockButtonInput mockButtonInput;
    grid::MockLedOutput mockLedOutput;
    grid::MockFlashingLeds mockFlashingLeds;
    grid::MockPulsingLeds mockPulsingLeds;
    grid::Grid grid( mockButtonInput, mockLedOutput, mockFlashingLeds, mockPulsingLeds );

    grid::ButtonEvent event;
    EXPECT_CALL( mockButtonInput, waitForEvent( testing::_ ) ).Times( 1 );
    grid.waitForInput( event );
}

TEST( discardPendingInput, Call )
{
    grid::MockButtonInput mockButtonInput;
    grid::MockLedOutput mockLedOutput;
    grid::MockFlashingLeds mockFlashingLeds;
    grid::MockPulsingLeds mockPulsingLeds;
    grid::Grid grid( mockButtonInput, mockLedOutput, mockFlashingLeds, mockPulsingLeds );

    EXPECT_CALL( mockButtonInput, discardPendingEvents ).Times( 1 );
    grid.discardPendingInput();
}

TEST( getLedColor, CheckDefaultValues )
{
    grid::MockButtonInput mockButtonInput;
    grid::MockLedOutput mockLedOutput;
    grid::MockFlashingLeds mockFlashingLeds;
    grid::MockPulsingLeds mockPulsingLeds;
    grid::Grid grid( mockButtonInput, mockLedOutput, mockFlashingLeds, mockPulsingLeds );

    for (uint8_t x = 0; x < 10; x++)
    {
        for (uint8_t y = 0; y < 8; y++)
        {
            EXPECT_EQ( Color(0, 0, 0), grid.getLedColor( { x, y } ) );
        }
    }
}

TEST( setLed, DefaultLightingType )
{
    grid::MockButtonInput mockButtonInput;
    grid::MockLedOutput mockLedOutput;
    grid::MockFlashingLeds mockFlashingLeds;
    grid::MockPulsingLeds mockPulsingLeds;
    grid::Grid grid( mockButtonInput, mockLedOutput, mockFlashingLeds, mockPulsingLeds );

    Coordinates coords = { 2, 2 };
    Color color = { 9, 9, 9 };

    EXPECT_CALL( mockLedOutput, set( coords, color ) ).Times( 1 );
    grid.setLed( coords, color );

    EXPECT_EQ( color, grid.getLedColor( coords ) );
}

TEST( setLed, OutOfBounds )
{
    grid::MockButtonInput mockButtonInput;
    grid::MockLedOutput mockLedOutput;
    grid::MockFlashingLeds mockFlashingLeds;
    grid::MockPulsingLeds mockPulsingLeds;
    grid::Grid grid( mockButtonInput, mockLedOutput, mockFlashingLeds, mockPulsingLeds );

    Coordinates coords = { 11, 11 };
    Color color = { 8, 8, 8 };

    EXPECT_CALL( mockLedOutput, set( coords, color ) ).Times( 0 );
    grid.setLed( coords, color );

    EXPECT_THROW( grid.getLedColor( coords ), etl::array_out_of_range );
}

TEST( setLed, Flashing )
{
    grid::MockButtonInput mockButtonInput;
    grid::MockLedOutput mockLedOutput;
    grid::MockFlashingLeds mockFlashingLeds;
    grid::MockPulsingLeds mockPulsingLeds;
    grid::Grid grid( mockButtonInput, mockLedOutput, mockFlashingLeds, mockPulsingLeds );

    Coordinates coords = { 3, 3 };
    Color color = { 9, 02, 10 };

    grid::FlashingColors expectedColors = { Color(0, 0, 0), color };
    EXPECT_CALL( mockFlashingLeds, add( coords, expectedColors ) ).Times( 1 );
    grid.setLed( coords, color, grid::LedLightingType::FLASH );

    expectedColors = { color, color };
    EXPECT_CALL( mockFlashingLeds, remove( coords ) ).Times( 1 );
    EXPECT_CALL( mockFlashingLeds, add( coords, expectedColors ) ).Times( 1 );
    grid.setLed( coords, color, grid::LedLightingType::FLASH  );

    EXPECT_CALL( mockFlashingLeds, remove( coords ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( coords, color ) ).Times( 1 );
    grid.setLed( coords, color );
}

TEST( setLed, Pulsing )
{
    grid::MockButtonInput mockButtonInput;
    grid::MockLedOutput mockLedOutput;
    grid::MockFlashingLeds mockFlashingLeds;
    grid::MockPulsingLeds mockPulsingLeds;
    grid::Grid grid( mockButtonInput, mockLedOutput, mockFlashingLeds, mockPulsingLeds );

    Coordinates coords = { 7, 7 };
    Color color = { 7, 7, 7 };

    EXPECT_CALL( mockPulsingLeds, add( coords, color ) ).Times( 1 );
    grid.setLed( coords, color, grid::LedLightingType::PULSE );

    EXPECT_CALL( mockPulsingLeds, remove( coords ) ).Times( 1 );
    EXPECT_CALL( mockPulsingLeds, add( coords, color ) ).Times( 1 );
    grid.setLed( coords, color, grid::LedLightingType::PULSE  );

    EXPECT_CALL( mockPulsingLeds, remove( coords ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( coords, color ) ).Times( 1 );
    grid.setLed( coords, color );
}

TEST( turnAllLedsOff, Call )
{
    grid::MockButtonInput mockButtonInput;
    grid::MockLedOutput mockLedOutput;
    grid::MockFlashingLeds mockFlashingLeds;
    grid::MockPulsingLeds mockPulsingLeds;
    grid::Grid grid( mockButtonInput, mockLedOutput, mockFlashingLeds, mockPulsingLeds );

    Color color = { 19, 20, 21 };

    for (uint8_t i = 0; i < 8; i++)
    {
        const Coordinates coords = {0, i};
        EXPECT_CALL( mockPulsingLeds, add( coords, color ) ).Times( 1 );
        grid.setLed( coords, color, grid::LedLightingType::PULSE );
        EXPECT_EQ( color, grid.getLedColor( coords ) );
    }
    
    for (uint8_t i = 0; i < 8; i++)
    {
        const Coordinates coords = {1, i};
        const grid::FlashingColors expectedColors = { Color(0, 0, 0), color };
        EXPECT_CALL( mockFlashingLeds, add( coords, expectedColors ) ).Times( 1 );
        grid.setLed( coords, color, grid::LedLightingType::FLASH );
        EXPECT_EQ( color, grid.getLedColor( coords ) );
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        const Coordinates coords = {2, i};
        EXPECT_CALL( mockLedOutput, set( coords, color ) ).Times( 1 );
        grid.setLed( coords, color );
        EXPECT_EQ( color, grid.getLedColor( coords ) );
    }

    EXPECT_CALL( mockPulsingLeds, removeAll ).Times( 1 );
    EXPECT_CALL( mockFlashingLeds, removeAll ).Times( 1 );
    EXPECT_CALL( mockLedOutput, setAllOff ).Times( 1 );
    grid.turnAllLedsOff();

    for (uint8_t x = 0; x < 3; x++)
    {
        for (uint8_t y = 0; y < 8; y++)
        {
            EXPECT_EQ( Color(0, 0, 0), grid.getLedColor( { x, y } ) );
        }
    }
}
