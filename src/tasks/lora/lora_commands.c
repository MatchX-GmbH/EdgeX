#include <stddef.h>
#include <stdio.h>
#include "pconfig.h"
#include "sx126x/sx126x.h"
#include "radio.h"
#include "lora_task.h"


//
// Device and application settings
//


int get_dev_eui(char** args, size_t count){
  pconfig_load();
  uint8_t* eui = pconfig.dev_eui;
  printf("Device EUI: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X \n", eui[0], eui[1], eui[2], eui[3], eui[4], eui[5], eui[6], eui[7]);
}

int get_app_eui(char** args, size_t count){
  pconfig_load();
  uint8_t* eui = pconfig.app_eui;
  printf("Application EUI: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X \n", eui[0], eui[1], eui[2], eui[3], eui[4], eui[5], eui[6], eui[7]);
}

int get_app_key(char** args, size_t count){
  pconfig_load();
  uint8_t* key = pconfig.app_key;
  printf(
         "Application key: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:"
         "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X \n",
         key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
         key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]);
}

int set_dev_eui(char** args, size_t count){
  if(count < 1){
    printf("No input!\n");
    return 1;
  }

  // we scan into uint32 and not 8 to avoid misaligned stores
  uint32_t eui[8];
  int ret = sscanf(args[0], "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", &eui[0], &eui[1], &eui[2], &eui[3], &eui[4], &eui[5], &eui[6], &eui[7]);

  if(ret != 8){
    printf("Invalid input! \n");
    return 2;
  }

  pconfig_load();
  for(int i = 0; i < 8; i++){
    pconfig.dev_eui[i] = eui[i];
  }
  pconfig_save();

  printf("Saved device EUI: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X \n", eui[0], eui[1], eui[2], eui[3], eui[4], eui[5], eui[6], eui[7]);

  return 0;
}

int set_app_eui(char** args, size_t count){
  if(count < 1){
    printf("No input!\n");
    return 1;
  }

  // we scan into uint32 and not 8 to avoid misaligned stores
  uint32_t eui[8];
  int ret = sscanf(args[0], "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", &eui[0], &eui[1], &eui[2], &eui[3], &eui[4], &eui[5], &eui[6], &eui[7]);

  if(ret != 8){
    printf("Invalid input! \n");
    return 2;
  }

  pconfig_load();
  for(int i = 0; i < 8; i++){
    pconfig.app_eui[i] = eui[i];
  }
  pconfig_save();

  printf("Saved app EUI: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X \n", eui[0], eui[1], eui[2], eui[3], eui[4], eui[5], eui[6], eui[7]);

  return 0;
}

int set_app_key(char** args, size_t count){
  if(count < 1){
    printf("No input!\n");
    return 1;
  }

  // we scan into uint32 and not 8 to avoid misaligned stores
  uint32_t key[16];
  int ret = sscanf(
        args[0],
       "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
        &key[0], &key[1], &key[2], &key[3], &key[4], &key[5], &key[6], &key[7],
        &key[8], &key[9], &key[10], &key[11], &key[12], &key[13], &key[14], &key[15]);

  if(ret != 16){
    printf("Invalid input! \n");
    return 2;
  }

  pconfig_load();
  for(int i = 0; i < 16; i++){
    pconfig.app_key[i] = (uint8_t)key[i];
  }
  pconfig_save();

  printf(
      "Saved app key: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:"
      "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X \n",
         key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
         key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]);

  return 0;
}





//
// Radio testing commands
//


#define LORA_IQ_INVERSION_ON                        false
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
static RadioEvents_t RadioEvents;
static uint8_t RadioRxPayload[255];
static PacketStatus_t RadioPktStatus;
static uint32_t PacketCounter;
static uint32_t TimeInterval;


int lora_cnt(char** args, size_t count)
{
  if(count != 2){
    printf("Invalid number of parameters!\n");
    return 1;
  }

  uint32_t freq, power;
  if(!sscanf(args[0], "%u", &freq) ||
     !sscanf(args[1], "%u", &power))
    {
      printf("Invalid format of parameters!\n");
      return 2;
    }

  mxtimer_clear_all();
  DeviceState = DEVICE_STATE_TEST;

  Radio.Init(&RadioEvents);

  SX126xSetRfFrequency(freq);
  SX126xSetRfTxPower(power);
  SX126xSetTxContinuousWave();

  printf("Continous wave transmission! \n");
  return 0;
}


static void start_tx(void* v){
  printf(".");
  fflush(stdout);

  uint32_t count = PacketCounter++;
  uint8_t payload[4];
  payload[0] = PacketCounter;
  payload[1] = PacketCounter >> 8;
  payload[2] = PacketCounter >> 16;
  payload[3] = PacketCounter >> 24;

  Radio.Send( payload, 4 );
}

static void start_rx(){
  printf("Start receiving!\n");

  Radio.Rx( 0xFFFFFF );
}

static void on_irq()
{
  uint16_t irqRegs = SX126xGetIrqStatus( );
  SX126xClearIrqStatus( IRQ_RADIO_ALL );

  if( ( irqRegs & IRQ_TX_DONE ) == IRQ_TX_DONE )
  {
      mxtimer_start(TimeInterval, start_tx, "");
  }

  if( ( irqRegs & IRQ_RX_DONE ) == IRQ_RX_DONE )
  {
    uint8_t size;
    SX126xGetPayload( RadioRxPayload, &size , 255 );
    SX126xGetPacketStatus( &RadioPktStatus );

    printf("\nPacket received\n");

    printf("Payload size: %u \n", size);

    if(size == 4){
      uint32_t count = RadioRxPayload[0];
      count |= RadioRxPayload[1] << 8;
      count |= RadioRxPayload[2] << 16;
      count |= RadioRxPayload[3] << 24;
      printf("Counter: %u \n", count);
    }

    printf("RSSI: %i \n", RadioPktStatus.Params.LoRa.RssiPkt);
    printf("SNR: %i \n", RadioPktStatus.Params.LoRa.SnrPkt);
    printf("Signal RSSI: %i \n", RadioPktStatus.Params.LoRa.SignalRssiPkt);
    printf("FreqError: %u \n", RadioPktStatus.Params.LoRa.FreqError);

  }
}

int lora_tx(char** args, size_t count)
{
  if(count != 5){
    printf("Invalid number of parameters!\n");
    return 1;
  }

  uint32_t freq, band, power, sf, time;
  if(!sscanf(args[0], "%u", &freq) ||
     !sscanf(args[1], "%u", &band) ||
     !sscanf(args[2], "%u", &power) ||
     !sscanf(args[3], "%u", &sf) ||
     !sscanf(args[4], "%u", &time))
    {
      printf("Invalid format of parameters!\n");
      return 2;
    }

  mxtimer_clear_all();
  DeviceState = DEVICE_STATE_TEST;
  TimeInterval = time;

  Radio.Init(&RadioEvents);
  SX126xIoIrqInit(on_irq);
  Radio.SetChannel(freq);
  Radio.SetTxConfig( MODEM_LORA, power, 0, band,
                     sf, LORA_CODINGRATE,
                     LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                     true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
  SX126xSetRfTxPower(power);

  start_tx(0);
}

int lora_rx(char** args, size_t count)
{
  if(count != 3){
    printf("Invalid number of parameters!\n");
    return 1;
  }

  uint32_t freq, band, sf;
  if(!sscanf(args[0], "%u", &freq) ||
     !sscanf(args[1], "%u", &band) ||
     !sscanf(args[2], "%u", &sf))
  {
      printf("Invalid format of parameters!\n");
      return 2;
  }

  mxtimer_clear_all();
  DeviceState = DEVICE_STATE_TEST;

  Radio.Init( &RadioEvents );
  SX126xIoIrqInit( on_irq );
  Radio.SetChannel( freq );
  Radio.SetRxConfig( MODEM_LORA, band, sf,
                     LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                     LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                     0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

  start_rx();
}

int lora_stop(char** args, size_t count){
  SX126xReset();
}
