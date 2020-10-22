/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: SX126x driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "board.h"
#include "../radio.h"
#include "sx126x.h"
#include "sx126x-board.h"
#include "../../config.h"

void SX126xIoInit( void )
{
    GpioMode( ANT_SWITCH_POWER, GPIO_DM_OUTPUT );
    GpioWrite( ANT_SWITCH_POWER, GPIO_PV_HIGH );

    GpioMode( RADIO_NSS, GPIO_DM_OUTPUT );
    GpioWrite( RADIO_NSS, GPIO_PV_HIGH );

    GpioMode( RADIO_RESET, GPIO_DM_OUTPUT );
    GpioWrite( RADIO_RESET, GPIO_PV_HIGH );

    GpioMode( RADIO_BUSY, GPIO_DM_INPUT );
    GpioMode( RADIO_DIO1, GPIO_DM_INPUT );

}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
    GpioSetInterrupt( RADIO_DIO1, GPIO_PE_RISING, dioIrq );
}

void SX126xIoDeInit( void )
{
    /* GpioInit( &SX126x.Spi.Nss, RADIO_NSS, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_PULL_UP, 1 ); */
    /* GpioInit( &SX126x.BUSY, RADIO_BUSY, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 ); */
    /* GpioInit( &SX126x.DIO1, RADIO_DIO_1, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 ); */
}

void SX126xReset( void )
{
    DelayMs( 10 );
    GpioWrite(RADIO_RESET, GPIO_PV_LOW);
    DelayMs( 20 );
    GpioWrite(RADIO_RESET, GPIO_PV_HIGH);
    DelayMs( 10 );
}

void SX126xWaitOnBusy( void )
{
  while( GpioRead( RADIO_BUSY ) == 1 ){
    /* printf("busy \n"); */
    /* sleep(1); */
  }
  /* printf("\nnot busy \n"); */
}

void SX126xWakeup( void )
{
    BoardDisableIrq( );

    GpioWrite( RADIO_NSS, 0 );

    SpiInOut( &SX126x.Spi, RADIO_GET_STATUS );
    SpiInOut( &SX126x.Spi, 0x00 );

    GpioWrite( RADIO_NSS, 1 );

    // Wait for chip to be ready.
    SX126xWaitOnBusy( );

    BoardEnableIrq( );
}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    GpioWrite( RADIO_NSS, 0 );

    SpiInOut( &SX126x.Spi, ( uint8_t )command );

    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( &SX126x.Spi, buffer[i] );
    }

    GpioWrite( RADIO_NSS, 1 );

    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }
}

void SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    GpioWrite( RADIO_NSS, 0 );

    SpiInOut( &SX126x.Spi, ( uint8_t )command );
    SpiInOut( &SX126x.Spi, 0x00 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    }

    GpioWrite( RADIO_NSS, 1 );

    SX126xWaitOnBusy( );
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    GpioWrite( RADIO_NSS, 0 );

    SpiInOut( &SX126x.Spi, RADIO_WRITE_REGISTER );
    SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
    SpiInOut( &SX126x.Spi, address & 0x00FF );

    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( &SX126x.Spi, buffer[i] );
    }

    GpioWrite( RADIO_NSS, 1 );

    SX126xWaitOnBusy( );
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    GpioWrite( RADIO_NSS, 0 );

    SpiInOut( &SX126x.Spi, RADIO_READ_REGISTER );
    SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
    SpiInOut( &SX126x.Spi, address & 0x00FF );
    SpiInOut( &SX126x.Spi, 0 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    }
    GpioWrite( RADIO_NSS, 1 );

    SX126xWaitOnBusy( );
}

uint8_t SX126xReadRegister( uint16_t address )
{
    uint8_t data;
    SX126xReadRegisters( address, &data, 1 );
    return data;
}

void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    GpioWrite( RADIO_NSS, 0 );

    SpiInOut( &SX126x.Spi, RADIO_WRITE_BUFFER );
    SpiInOut( &SX126x.Spi, offset );
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOut( &SX126x.Spi, buffer[i] );
    }
    GpioWrite( RADIO_NSS, 1 );

    SX126xWaitOnBusy( );
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    GpioWrite( RADIO_NSS, 0 );

    SpiInOut( &SX126x.Spi, RADIO_READ_BUFFER );
    SpiInOut( &SX126x.Spi, offset );
    SpiInOut( &SX126x.Spi, 0 );
    for( uint16_t i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    }
    GpioWrite( RADIO_NSS, 1 );

    SX126xWaitOnBusy( );
}

void SX126xSetRfTxPower( int8_t power )
{
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
}

uint8_t SX126xGetPaSelect( uint32_t channel )
{
    return SX1261;
    /* if( GpioRead( &DeviceSel ) == 1 ) */
    /* { */
    /*     return SX1261; */
    /* } */
    /* else */
    /* { */
    /*     return SX1262; */
    /* } */
}

void SX126xAntSwOn( void )
{
    GpioWrite( ANT_SWITCH_POWER, GPIO_PV_HIGH );
}

void SX126xAntSwOff( void )
{
    GpioWrite( ANT_SWITCH_POWER, GPIO_PV_LOW );
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}
