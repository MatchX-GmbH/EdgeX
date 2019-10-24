#include <stddef.h>
#include <printf.h>
#include <string.h>
#include <uart.h>
#include <sysctl.h>
#include "console.h"

#define BUFFER_SIZE 127
#define MAX_ARGS 16
static const char command_delim = ',';

// last char for null termination
static char buffer[BUFFER_SIZE + 1];
static size_t buffer_count = 0;
static char* args[MAX_ARGS];

#define DECLARE_COMMAND(x) int x(char** , size_t);

typedef int(*command_handler)(char**, size_t);

typedef struct command_s {
  const char * invocation;
  command_handler handler;
  const char * desc;
} command_t;


// If new command needs to be added, it needs to be registered here:

DECLARE_COMMAND(console_help);
DECLARE_COMMAND(system_reboot);

DECLARE_COMMAND(set_dev_eui);
DECLARE_COMMAND(set_app_eui);
DECLARE_COMMAND(set_app_key);
DECLARE_COMMAND(get_dev_eui);
DECLARE_COMMAND(get_app_eui);
DECLARE_COMMAND(get_app_key);

DECLARE_COMMAND(lora_cnt);
DECLARE_COMMAND(lora_tx);
DECLARE_COMMAND(lora_rx);
DECLARE_COMMAND(lora_stop);

DECLARE_COMMAND(dbprint_enable);
DECLARE_COMMAND(dbprint_disable);


static command_t commands[] =
{
 {"help", console_help,
  "Display command help"},
 {"reboot", system_reboot,
  "Reboot module"},

 {"set dev eui", set_dev_eui,
  "Set the EUI of the device in format x:x: ... :x:x"},
 {"set app eui", set_app_eui,
  "Set the EUI of the application in format x:x: ... :x:x"},
 {"set app key", set_app_key,
  "Set the application key in format x:x: ... :x:x"},
 {"get dev eui", get_dev_eui,
  "Print device EUI"},
 {"get app eui", get_app_eui,
  "Print application EUI"},
 {"get app key", get_app_key,
  "Print application key"},

 {"debug print enable", dbprint_enable,
  "Enable additional printouts"},
 {"debug print disable", dbprint_disable,
  "Disable additional printouts"},

 {"lora cnt", lora_cnt,
  "Start continous wave with params:\n"
  "\tlora cnt,<freq Hz>,<power dB>"},
 {"lora tx", lora_tx,
  "Start transmission of packets with params:\n"
  "\tlora tx,<freq Hz>,<bandwidth>,<power dB>,<spreading factor>,<interval msec>\n"
  "\tBandwidth value: 0 - 125Hz, 1 - 250Hz, 2 - 500Hz"},
 {"lora rx", lora_rx,
  "Start receiving packets with params:\n"
  "\tlora cnt tx,<freq Hz>,<bandwidth>,<spreading factor>\n"
  "\tBandwidth value: 0 - 125Hz, 1 - 250Hz, 2 - 500Hz"},
 {"lora stop", lora_stop,
  "Stop transmission and receiving"},
};

// registry end



static void invoke_command(const char* command, char** args, size_t count){
  for(int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++){
      if(strcmp(command, commands[i].invocation) == 0){
          commands[i].handler(args, count);
          return;
      }
  }

  printk("No registered command for invocation \"%s\"\n", command);
}

static void process_buffer(void){
  if(buffer_count == 0) return;
  buffer[buffer_count] = 0; // terminate input

  volatile int block = 0;
  while(block);

  // Find the command
  char* cmd_end = strchr(buffer, command_delim);
  // No arguments - delimiter not found, or nothing after delimiter
  if(cmd_end == NULL){
      invoke_command(buffer, NULL, 0);
      return;
  }else if(cmd_end == buffer + buffer_count - 1){
      *cmd_end = 0; // terminate command
      invoke_command(buffer, NULL, 0);
      return;
  }

  // Parse arguments
  size_t arg_index = 0;
  char* arg_pos = cmd_end;
  do
  {
      *arg_pos = 0; // terminate previous expr
      arg_pos++;

      if(*arg_pos == 0) // empty expr, we reached the end of input
        break;

      args[arg_index++] = arg_pos;
      arg_pos = strchr(arg_pos, command_delim);
  }
  while(arg_pos);

  invoke_command(buffer, args, arg_index);
}

static int console_uart_handle(void* ctx){
  char in;
  int ret = uart_receive_data(UART_DEVICE_3, &in, 1);
  if(ret){
    if(in >= 'a' && in <= 'z' ||
       in >= 'A' && in <= 'Z' ||
       in >= '0' && in <= '9' ||
       in == ' ' || in == ',' || in == ':')
    {
        if(buffer_count < BUFFER_SIZE){
          printk("%c", in);
          buffer[buffer_count++] = in;
        }
    }
    else if(in == '\n' || in == '\r')
    {
        printk("\n");
        process_buffer();
        buffer_count = 0;
    }
    else if(in == '\r')
    {}
    else if(in == '\b')
    {
        if(buffer_count != 0){
            printk("\b \b", in);
            buffer_count--;
        }
    }
    else
    {
        printk("?");
    }
  }
}

void console_register(void){
  uart_irq_register(UART_DEVICE_3, UART_RECEIVE, console_uart_handle, NULL, 5);
}

int console_help(char** args, size_t count){
  printf("List of commands:\n\n");
  for(int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++){
      printf("\"%s\"\n"
             "\t%s\n", commands[i].invocation, commands[i].desc);

  }
}

int system_reboot(char** args, size_t count){
  sysctl_reset(SYSCTL_RESET_SOC);
}
