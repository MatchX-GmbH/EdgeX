#ifndef LORA_TASK
#define LORA_TASK

/*!
 * Device states
 */
enum eDeviceState
{
   DEVICE_STATE_RESTORE,
   DEVICE_STATE_START,
   DEVICE_STATE_JOIN,
   DEVICE_STATE_SEND,
   DEVICE_STATE_CYCLE,
   DEVICE_STATE_SLEEP,
   DEVICE_STATE_TEST
};

extern enum eDeviceState DeviceState;

int lora_task(void);

#endif
