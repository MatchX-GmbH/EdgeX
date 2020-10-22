#include "font.h"

static void convert_to_rgb(uint8_t * src, uint8_t * dest){
  for(int j = 0; j < 64; j++){
    for(int i = 0; i < 64; i++){
      int i_ = i + (i % 2 ? -1 : 1);
      uint8_t color = src[i] / 8;
      dest[i_*2+1] = color << 3 | color >> 2;
      dest[i_*2] = color << 6 | color;
    }
    dest += 64 * 2;
    src += 64;
  }
}

static void convert_to_rgb_big(uint8_t * src, uint8_t * dest){
  for(int j = 0; j < 240; j++){
    for(int i = 0; i < 320; i++){
      int i_ = i + (i % 2 ? -1 : 1);
      uint8_t color = src[i] / 8;
      dest[i_*2+1] = color << 3 | color >> 2;
      dest[i_*2] = color << 6 | color;
    }
    dest += 320 * 2;
    src += 320;
  }
}

static void downscale(const uint16_t* src, uint16_t* dest){
  for(int y = 0; y < 240; y++){
    for(int x = 0; x < 320; x++){
      dest[y * 320 + x] = src[y * 4 * 320 + x * 2];
    }
  }
}

static void downscale2(const uint16_t* src, uint16_t* dest){

  for(int y = 0; y < 150; y++){
    for(int x = 0; x < 200; x++){
      dest[y * 200 + x] =  src[y * 4 * (800) + x * 4];
    }
  }

}

static void copy_greyscale_from_rgb888(const uint8_t* src, uint8_t* dest, uint16_t src_width, uint16_t dest_width, uint16_t dest_height, uint16_t x_offset, uint16_t y_offset, uint8_t scaling_factor)
{
  for(uint16_t y = 0; y < dest_height; y++){
    for(uint16_t x = 0; x < dest_width; x++){
      uint16_t y_coord = (y*scaling_factor + y_offset) * src_width;
      uint16_t x_coord = x*scaling_factor + x_offset;

      x_coord += x%2 ? -1 : 1;

      *dest = src[y_coord + x_coord];

      dest++;
    }
  }
}

static void ram_draw_string(const char *str, uint16_t *dest, uint16_t x, uint16_t y, uint16_t dest_width ,uint16_t font_color, uint16_t bg_color)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t data = 0;
    uint8_t *pdata = NULL;
    uint16_t width = 0;
    uint16_t *pixel = NULL;

    width = 4 * strlen(str);
    dest += x + y * dest_width;

    while (*str != 0)
    {
        pdata = (uint8_t *)&ascii0816[(*str) * 16];
        for (i = 0; i < 16; i++)
        {
            data = *pdata++;
            pixel = dest + i * dest_width;

            for (j = 0; j < 4; j++)
            {
                if(data & 0x80){
                  *(pixel+1) = font_color;
                }else{
                  *(pixel+1) = bg_color;
                }
                if(data & 0x40){
                  *pixel = font_color;
                }else{
                  *pixel = bg_color;
                }
                data <<= 2;
                pixel += 2;
            }
        }
        str++;
        dest += 8;
    }
}

/* static void ram_draw_box(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t width, uint16_t color){ */

/*   uint16_t * lcd_gram = g_ram_mux ? g_lcd_gram0 : g_lcd_gram1; */

/*   for(int x = x1; x < x2; x++){ */
/*     int x_ = x + (x%2 ? -1 : 1); */
/*     lcd_gram[y1 * 320 + x_] = color; */
/*   } */
/*   for(int x = x1; x < x2; x++){ */
/*     int x_ = x + (x%2 ? -1 : 1); */
/*     lcd_gram[y2 * 320 + x_] = color; */
/*   } */

/*   x1 += x1%2 ? -1 : 1; */
/*   x2 += x2%2 ? -1 : 1; */

/*   for(int y = y1; y < y2; y++){ */
/*     lcd_gram[y * 320 + x1] = color; */
/*   } */
/*   for(int y = y1; y < y2; y++){ */
/*     lcd_gram[y * 320 + x2] = color; */
/*   } */

/* } */

static void drawboxes(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t class, float prob)
{
  if (x1 >= 320)
    x1 = 319;
  if (x2 >= 320)
    x2 = 319;
  if (y1 >= 240)
    y1 = 239;
  if (y2 >= 240)
    y2 = 239;

  ram_draw_box(x1, y1, x2, y2, 2, RED);
}
