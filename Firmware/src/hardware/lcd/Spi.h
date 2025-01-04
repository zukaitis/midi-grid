#pragma once

#include "hardware/lcd/SpiInterface.h"

#include "stm32f4xx_hal.h"
#include <cstdint>

namespace hardware
{

namespace lcd
{

class Spi: public SpiInterface
{
public:
    Spi();
    ~Spi() override;

    void initialize() const override;
    void setBulkTransferStartAddressIncrement( uint16_t increment ) override;

    void reset() const override;
    void writeCommand( uint8_t command ) const override;
    void writeData( const uint8_t& data, uint32_t size ) const override;
    void writeData( const RawDataView& data ) const override;
    void writeData( const void* address, uint16_t size ) const override;
    void writeData( const void* startAddress, uint16_t segmentSize, uint16_t numberOfSegments ) const override;

private:
    static void bulkTransferPartComplete( __DMA_HandleTypeDef* unused );

    static void initializeDma();
    static void initializeGpio();
    static void initializeSpi();

    static void waitWhileSpiIsBusy();

    static void enableTransferCompleteCallback();
    static void disableTransferCompleteCallback();

    static bool bulkTransferInProgress_;
    static uint16_t bulkTransferStartAddressIncrement_;
    static const uint8_t* bulkTransferStartAddress_;
    static uint16_t bulkTransferSegmentSize_;
    static uint16_t bulkTransferSegmentsLeft_;
};

}  // namespace lcd

}  // namespace hardware
