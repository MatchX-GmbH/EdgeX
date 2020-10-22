#ifndef SPOT_ANALYSIS_H
#define SPOT_ANALYSIS_H

#include "pconfig.h"

#define IMAGE_SIZE 64

typedef struct parking_data_s{
  uint8_t spot_count;
  uint8_t images[MAX_PARKING_SPOTS][IMAGE_SIZE * IMAGE_SIZE] __attribute__((aligned(128)));
  float predictions[MAX_PARKING_SPOTS];
} parking_data_t;

static parking_data_t parking_data;
uint8_t scaled_image[320 * 240] __attribute__((aligned(128)));

typedef struct rect_s{
  uint16_t x;
  uint16_t x2;
  uint16_t y;
  uint16_t y2;
} rect_t;

static void resize_bilinear_part(uint8_t* dest, const uint8_t* src, rect_t v, rect_t s, rect_t o) {
    // compute absolute values
    uint16_t v_w = v.x2 - v.x;

    uint16_t s_w = s.x2 - s.x;
    uint16_t s_h = s.y2 - s.y;

    uint16_t o_w = o.x2 - o.x;
    uint16_t o_h = o.y2 - o.y;

    uint16_t o_x_off = o.x - s.x;
    uint16_t o_y_off = o.y - s.y;

    // take the resize into cons
    float x_ratio = ((float)(s_w-1))/IMAGE_SIZE;
    float y_ratio = ((float)(s_h-1))/IMAGE_SIZE;

    // use floats and round to avoid trimming pixels
    o_w = ((float)o_w / x_ratio);
    o_h = ((float)o_h / y_ratio);

    o_x_off = ((float)o_x_off / x_ratio);
    o_y_off = ((float)o_y_off / y_ratio);

    // shift the pointers to offset
    dest = dest + (o_y_off * IMAGE_SIZE + o_x_off);
    src = src + ((o.y - v.y) * v_w + (o.x - v.x));

    for (uint32_t i=0;i<o_h;i++) {
      for (uint32_t j=0;j<o_w;j++) {
            uint32_t x = x_ratio * j;
            uint32_t y = y_ratio * i;
            float x_diff = x_ratio * j - x;
            float y_diff = y_ratio * i - y;
            uint32_t index = y*v_w+x;

            uint8_t p1 = src[index];
            uint8_t p2 = src[index+1];
            uint8_t p3 = src[index+v_w];
            uint8_t p4 = src[index+v_w+1];

            dest[j] =
              p1 * (1 - x_diff) * (1 - y_diff) +
              p2 * (x_diff) * (1 - y_diff) +
              p3 * (y_diff) * (1 - x_diff) +
              p4 * (x_diff) * (y_diff);

        }
        dest += IMAGE_SIZE;
    }
}

// TODO: merge resizing
static void resize_bilinear_q(uint8_t* dest, const uint8_t* src, rect_t v, rect_t s, rect_t o) {
    // compute absolute values
    uint16_t v_w = v.x2 - v.x;

    uint16_t s_w = s.x2 - s.x;
    uint16_t s_h = s.y2 - s.y;

    uint16_t o_w = o.x2 - o.x;
    uint16_t o_h = o.y2 - o.y;

    uint16_t o_x_off = o.x - s.x;
    uint16_t o_y_off = o.y - s.y;

    // take the resize into cons
    float x_ratio = ((float)(s_w-1))/(320);
    float y_ratio = ((float)(s_h-1))/(240);

    // use floats and round to avoid trimming pixels
    o_w = ((float)o_w / x_ratio);
    o_h = ((float)o_h / y_ratio);

    o_x_off = ((float)o_x_off / x_ratio);
    o_y_off = ((float)o_y_off / y_ratio);

    // shift the pointers to offset
    dest = dest + (o_y_off * 320 + o_x_off);
    src = src + ((o.y - v.y) * v_w + (o.x - v.x));

    for (uint32_t i=0;i<o_h;i++) {
      for (uint32_t j=0;j<o_w;j++) {
            uint32_t x = x_ratio * j;
            uint32_t y = y_ratio * i;
            float x_diff = x_ratio * j - x;
            float y_diff = y_ratio * i - y;
            uint32_t index = y*v_w+x;

            uint8_t p1 = src[index];
            uint8_t p2 = src[index+1];
            uint8_t p3 = src[index+v_w];
            uint8_t p4 = src[index+v_w+1];

            dest[j] =
              p1 * (1 - x_diff) * (1 - y_diff) +
              p2 * (x_diff) * (1 - y_diff) +
              p3 * (y_diff) * (1 - x_diff) +
              p4 * (x_diff) * (y_diff);

        }
        dest += 320;
    }
}

static void copy_image_from_view(const uint8_t* view_data, rect_t v){
  pconfig_load();
  parking_slot_t* parking_slots = pconfig.parking_slots;

  for(int i = 0; i < MAX_PARKING_SPOTS; i++){
    parking_slot_t* slot = &parking_slots[i];

    if(slot->size == 0) break;

    // slot coords
    rect_t s = (rect_t){ .x = slot->x - slot->size, .y = slot->y - slot->size, .x2 = slot->x + slot->size, .y2 = slot->y + slot->size };
    // overlap coords
    rect_t o;

    // TODO: code below is naive impl, perhaps can be optimized

    // top left corner
    if( s.x >= v.x && s.x <= v.x2 &&
        s.y >= v.y && s.y <= v.y2 ){
      o.x = s.x;
      o.y = s.y;

      if(s.x2 < v.x2)
        o.x2 = s.x2;
      else
        o.x2 = v.x2;

      if(s.y2 < v.y2)
        o.y2 = s.y2;
      else
        o.y2 = v.y2;
    }
    // top right corner
    else if( s.x2 >= v.x && s.x2 <= v.x2 &&
             s.y >= v.y && s.y <= v.y2 ){
      o.x2 = s.x2;
      o.y = s.y;

      // previous case was untrue, so we can assume top left corner is not in range
      o.x = v.x;

      if(s.y2 < v.y2)
        o.y2 = s.y2;
      else
        o.y2 = v.y2;
    }
    // bottom left corner
    else if( s.x >= v.x && s.x <= v.x2 &&
             s.y2 >= v.y && s.y2 <= v.y2){
      o.x = s.x;
      o.y = v.y;

      // analogous to above case
      o.y2 = s.y2;

      if(s.x2 < v.x2)
        o.x2 = s.x2;
      else
        o.x2 = v.x2;
    }
    // botton right corner
    else if( s.x2 >= v.x && s.x2 <= v.x2 &&
             s.y2 >= v.y && s.y2 <= v.y2){
      // because previous cases failed
      // we know that this is the only vertex overlapping
      // which makes it a trivial case
      o.x = v.x;
      o.x2 = s.x2;
      o.y = v.y;
      o.y2 = s.y2;
    }
    // no match
    else{
      continue;
    }

    const uint16_t max_x = 1600;
    const uint16_t max_y = 1200;

    if(s.x >= max_x || s.x2 >= max_x || s.y >= max_y || s.y2 >= max_y){
      printf("coords out of bounds, skipping: xxyy %d %d %d %d\n", o.x, o.x2, o.y, o.y2);
      continue;
    }

    uint8_t* dest = parking_data.images[i];
    const uint8_t* src = view_data;

    resize_bilinear_part(dest, src, v, s, o);
  }
}

extern volatile uint8_t g_dvp_finish_flag;
extern uint8_t* ai_image;

static void process_camera(){

  int spot_count = 0;
  for(; spot_count < MAX_PARKING_SPOTS; spot_count++){
    parking_slot_t* slot = &pconfig.parking_slots[spot_count];
    if(slot->size == 0) break;
  }
  parking_data.spot_count = spot_count;

  ov_2640_set_framesize(true, 800, 600, 0, 0, 800, 600);
  g_dvp_finish_flag = 0;
  while (g_dvp_finish_flag == 0)
    ;
  copy_image_from_view(ai_image, (rect_t){.x = 0, .y = 0, .x2 = 800, .y2 = 600});
  resize_bilinear_q(scaled_image, ai_image,
                    (rect_t){.x = 0, .y = 0, .x2 = 800, .y2 = 600},
                    (rect_t){.x = 0, .y = 0, .x2 = 1600, .y2 = 1200},
                    (rect_t){.x = 0, .y = 0, .x2 = 800, .y2 = 600});

  ov_2640_set_framesize(true, 800, 600, 800, 0, 800, 600);
  g_dvp_finish_flag = 0;
  while (g_dvp_finish_flag == 0)
    ;
  copy_image_from_view(ai_image, (rect_t){.x = 800, .y = 0, .x2 = 1600, .y2 = 600});
  resize_bilinear_q(scaled_image, ai_image,
                    (rect_t){.x = 800, .y = 0, .x2 = 1600, .y2 = 600},
                    (rect_t){.x = 0, .y = 0, .x2 = 1600, .y2 = 1200},
                    (rect_t){.x = 800, .y = 0, .x2 = 1600, .y2 = 600});


  ov_2640_set_framesize(true, 800, 600, 0, 600, 800, 600);
  g_dvp_finish_flag = 0;
  while (g_dvp_finish_flag == 0)
    ;
  copy_image_from_view(ai_image, (rect_t){.x = 0, .y = 600, .x2 = 800, .y2 = 1200});
  resize_bilinear_q(scaled_image, ai_image,
                    (rect_t){.x = 0, .y = 600, .x2 = 800, .y2 = 1200},
                    (rect_t){.x = 0, .y = 0, .x2 = 1600, .y2 = 1200},
                    (rect_t){.x = 0, .y = 600, .x2 = 800, .y2 = 1200});

  ov_2640_set_framesize(true, 800, 600, 800, 600, 800, 600);
  g_dvp_finish_flag = 0;
  while (g_dvp_finish_flag == 0)
    ;
  copy_image_from_view(ai_image, (rect_t){.x = 800, .y = 600, .x2 = 1600, .y2 = 1200});
  resize_bilinear_q(scaled_image, ai_image,
                    (rect_t){.x = 800, .y = 600, .x2 = 1600, .y2 = 1200},
                    (rect_t){.x = 0, .y = 0, .x2 = 1600, .y2 = 1200},
                    (rect_t){.x = 800, .y = 600, .x2 = 1600, .y2 = 1200});
}

#endif
