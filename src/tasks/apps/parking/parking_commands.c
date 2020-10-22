#include <inttypes.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "pconfig.h"

int camera_set_res(char** args, size_t argc){
  if(argc != 7){
    printf("Invalid number of arguments.\n");
    return -1;
  }

  bool uxga = false;

  if(strcmp(args[0], "SVGA") == 0){
    uxga = false;
  }else if(strcmp(args[0], "UXGA") == 0){
    uxga = true;
  }else{
    printf("Capture resolution should be either SVGA or UXGA\n");
    return -1;
  }

  int parse_count = 0;
  uint16_t w_width, w_height, w_off_x, w_off_y, o_width, o_height;
  parse_count += sscanf(args[1], "%"SCNu16, &w_width);
  parse_count += sscanf(args[2], "%"SCNu16, &w_height);
  parse_count += sscanf(args[3], "%"SCNu16, &w_off_x);
  parse_count += sscanf(args[4], "%"SCNu16, &w_off_y);
  parse_count += sscanf(args[5], "%"SCNu16, &o_width);
  parse_count += sscanf(args[6], "%"SCNu16, &o_height);

  if(parse_count != 6){
    printf("Could not parse all arguments\n");
    return -1;
  }

  ov_2640_set_framesize(uxga, w_width, w_height, w_off_x, w_off_y, o_width, o_height);

}

#include <lcd.h>

extern bool usb_copy_flag;
extern uint8_t usb_copy[];

int camera_download(char** args, size_t argc){

  size_t offset = 0;
  size_t len = 0;

  if(argc >= 1){
    sscanf(args[0], "%u", &len);
  }

  if(argc >= 2){
    sscanf(args[1], "%u", &offset);
  }

  if(offset == 0 && len == 0){
    usb_copy_flag = 1;
    return 0;
  }

  uint8_t* copy = (uint8_t*)((size_t)usb_copy - 0x40000000);

  uart_send_data(2, copy + offset, len);

  return 0;
}

int parking_reset(char** args, size_t argc){
  pconfig_load();

  for(int i = 0; i < sizeof(pconfig.parking_slots) / sizeof(pconfig.parking_slots[0]); i++){
    // size of 0 means not in use
    pconfig.parking_slots[i].size = 0;
  }

  pconfig_save();

  return 0;
}

int parking_get(char** args, size_t argc){
  pconfig_load();

  for(int i = 0; i < sizeof(pconfig.parking_slots) / sizeof(pconfig.parking_slots[0]); i++){
    const parking_slot_t* ps = &pconfig.parking_slots[i];

    // size of zero means inactive slot data
    if(ps->size == 0) break;

    printf("%i:%d,%d,%d\n", i, ps->x, ps->y, ps->size);
  }
}


int parking_set(char** args, size_t argc){

  if(argc != 4){
    printf("Invalid number of arguments\n");
    return -1;
  }

  int parse_count = 0;
  uint16_t index, pos_x, pos_y, size;
  parse_count += sscanf(args[0], "%"SCNu16, &index);
  parse_count += sscanf(args[1], "%"SCNu16, &pos_x);
  parse_count += sscanf(args[2], "%"SCNu16, &pos_y);
  parse_count += sscanf(args[3], "%"SCNu16, &size);

  if(parse_count != 4){
    printf("Could not parse all arguments\n");
    return -1;
  }

  // TODO: validate coordinates and size
  if(index >= sizeof(pconfig.parking_slots) / sizeof(pconfig.parking_slots[0])){
    printf("Index is out of bounds.\n");
    return -1;
  }

  uint16_t max_x = 1600, max_y = 1200;
  uint16_t x1 = pos_x - size, x2 = pos_x + size, y1 = pos_y - size, y2 = pos_y + size;

  if(x1 > max_x || x2 > max_x || y1 > max_y || y2 > max_y){
    printf("Invalid coordinates.\n");
    return -1;
  }

  pconfig_load();

  parking_slot_t ps = {.x = pos_x, .y = pos_y, .size = size};
  pconfig.parking_slots[index] = ps;

  pconfig_save();

  printf("OK\n");

  return 0;
}
