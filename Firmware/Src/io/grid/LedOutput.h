#pragma once

struct Coordinates;
struct Color;

namespace hardware
{
    namespace grid
    {
        class OutputInterface;
    }
}

namespace grid
{

class LedOutput
{
public:
    LedOutput( hardware::grid::OutputInterface& gridDriver );

    void set( const Coordinates& coordinates, const Color& color );
    void setAllOff();

private:
    Coordinates calculateHardwareCoordinates( const Coordinates& coordinates ) const;

    hardware::grid::OutputInterface& gridDriver_;
};

}