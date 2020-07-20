#include "lcd/image/Image.h"
#include "lcd/DriverInterface.h"
#include "types/Coordinates.h"
#include "types/Vector.h"

namespace lcd
{

// does not link without these definitions, not sure why
const uint16_t Image::maxWidth_;
const uint16_t Image::maxHeight_;

Image::Image( DriverInterface* driver ):
    driver_( *driver ),
    width_( 0 ),
    height_( 0 ),
    wordsPerColumn_( 0 ),
    data_(),
    palette_(),
    drawingInProgress_( false )
{
}

Image::~Image() = default;

void Image::createNew( const uint16_t width, const uint16_t height )
{
    initialize( width, height );
    clear();
}

void Image::createNew( const uint16_t width, const uint16_t height, const Color& fill )
{
    initialize( width, height );
    Image::fill( fill );
}

void Image::createNew( const Color& fill )
{
    createNew( maxWidth_, maxHeight_, fill );
}

void Image::createNew()
{
    createNew( maxWidth_, maxHeight_ );
}

void Image::clear()
{
    palette_.clear();
    backgroundColorIndex_ = 0;
    data_.assign( width_ * wordsPerColumn_, 0U );
}

void Image::fill( const Color& color )
{
    palette_.clear();
    palette_.emplace_back( color );  // add fill color to palette;
    backgroundColorIndex_ = 1;

    uint32_t filler = 0;
    for (uint8_t i = 0; i < pixelsPerWord_; i++)
    {
        filler |= 1U << (i * colorBits_);  // assign color index of 1 to all pixels
    }
    
    data_.assign( width_ * wordsPerColumn_, filler );
}

void Image::endDrawing()
{
    drawingInProgress_ = false;
}

bool Image::isBeingDrawn()
{
    return drawingInProgress_;
}

void Image::display( const Coordinates& coords )
{
    for (uint16_t x = 0; x < width_; x++)
    {
        for (uint16_t y = 0; y < height_; y++)
        {
            const uint8_t colorIndex = getColorIndex( {x, y} );
            if (0 != colorIndex)
            {
                driver_.putPixel( coords + Vector( x, y ), palette_.at(colorIndex - 1) );
            }
        }
    }
    driver_.flush();
}

void Image::display()
{
    display( {0, 0} );
}

uint16_t Image::width()
{
    return width_;
}

uint16_t Image::height()
{
    return height_;
}


bool Image::putPixel( const Coordinates& coords, uint8_t colorIndex )
{
    bool outOfRange = false;
    if ((coords.x < width_) && (coords.y < height_))
    {
        const uint16_t dataIndex = coords.x * wordsPerColumn_ + coords.y / pixelsPerWord_;
        const uint8_t shift = (coords.y % pixelsPerWord_) * colorBits_;
        data_.at(dataIndex) &= ~(pixelMask_ << shift);
        data_.at(dataIndex) |= (colorIndex & pixelMask_) << shift;
    }
    else
    {
        outOfRange = true;
    }
    return outOfRange;
}

uint8_t Image::assignColorIndex( const Color& color )
{
    uint8_t index = 0;

    for (uint32_t i = 0; i < palette_.size(); i++)
    {
        if (color == palette_.at( i ))
        {
            index = i + 1;
        }
    }

    if (0 == index)
    {
        // color not found in current palette
        if (false == palette_.full())
        {
            palette_.emplace_back( color );
            index = palette_.size();
        }
    }

    return index;
}

uint8_t Image::getBackgroundColorIndex() 
{
    return backgroundColorIndex_;
}

void Image::initialize( const uint16_t width, const uint16_t height )
{
    width_ = std::min( width, maxWidth_ );
    height_ = std::min( height, maxHeight_ );
    wordsPerColumn_ = (height_ + pixelsPerWord_ - 1) / pixelsPerWord_;
    drawingInProgress_ = true;
    palette_.clear();
    data_.clear();
}

uint8_t Image::getColorIndex( const Coordinates& pixel ) const
{
    const uint16_t dataIndex = pixel.x * wordsPerColumn_ + pixel.y / pixelsPerWord_;
    const uint8_t shift = (pixel.y % pixelsPerWord_) * colorBits_;
    return ((data_.at( dataIndex ) >> shift) & pixelMask_);
}

}  // namespace lcd
