/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include "ov2640.h"
#include "ov2640_regs.h"
#include "dvp.h"
#include "plic.h"
#include "board_config.h"

const uint8_t ov2640_config[][2]=
{
        {0xff, 0x01},
    {COM7, 0x80},
    {0xff, 0x00},
    {0x2c, 0xff},
    {0x2e, 0xdf},
        {0xff, 0x01}, // FF 1
    {0x3c, 0x32},
    {0x11, 0x00},
    {0x09, 0x02},
#if BOARD_LICHEEDAN
    {0x04, 0x88},
#else
    {0x04, 0xD8},
#endif
    {0x13, 0xe5},
    {0x14, 0x48},
    {0x2c, 0x0c},
    {0x33, 0x78},
    {0x3a, 0x33},
    {0x3b, 0xfb},
    {0x3e, 0x00},
    {0x43, 0x11},
    {0x16, 0x10},
    {0x39, 0x92},
    {0x35, 0xda},
    {0x22, 0x1a},
    {0x37, 0xc3},
    {0x23, 0x00},
    {0x34, 0xc0},
    {0x36, 0x1a},
    {0x06, 0x88},
    {0x07, 0xc0},
    {0x0d, 0x87},
    {0x0e, 0x41},
    {0x4c, 0x00},
    {0x48, 0x00},
    {0x5b, 0x00},
    {0x42, 0x03},
    {0x4a, 0x81},
    {0x21, 0x99},
    {0x24, 0x40},
    {0x25, 0x38},
    {0x26, 0x82},
    {0x5c, 0x00},
    {0x63, 0x00},
    {0x46, 0x22},
    {0x0c, 0x3c},
    {0x61, 0x70},
    {0x62, 0x80},
    {0x7c, 0x05},
    {0x20, 0x80},
    {0x28, 0x30},
    {0x6c, 0x00},
    {0x6d, 0x80},
    {0x6e, 0x00},
    {0x70, 0x02},
    {0x71, 0x94},
    {0x73, 0xc1},
    {0x3d, 0x34},
    {0x5a, 0x57},
    {COM7, 0x40},
    {0x17, 0x11},
    {0x18, 0x43},
    {0x19, 0x00},
    {0x1a, 0x4b},
    {0x32, 0x09},
    {0x37, 0xc0},
    {0x4f, 0xca},
    {0x50, 0xa8},
    {0x5a, 0x23},
    {0x6d, 0x00},
    {0x3d, 0x38},
        {0xff, 0x00}, // FF 0
    {0xe5, 0x7f},
    {0xf9, 0xc0},
    {0x41, 0x24},
    {0xe0, 0x14},
    {0x76, 0xff},
    {0x33, 0xa0},
    {0x42, 0x20},
    {0x43, 0x18},
    {0x4c, 0x00},
    {0x87, 0xd5},
    {0x88, 0x3f},
    {0xd7, 0x03},
    {0xd9, 0x10},
    {0xd3, 0x82},
    {0xc8, 0x08},
    {0xc9, 0x80},
    {0x7c, 0x00},
    {0x7d, 0x00},
    {0x7c, 0x03},
    {0x7d, 0x48},
    {0x7d, 0x48},
    {0x7c, 0x08},
    {0x7d, 0x20},
    {0x7d, 0x10},
    {0x7d, 0x0e},
    {0x90, 0x00},
    {0x91, 0x0e},
    {0x91, 0x1a},
    {0x91, 0x31},
    {0x91, 0x5a},
    {0x91, 0x69},
    {0x91, 0x75},
    {0x91, 0x7e},
    {0x91, 0x88},
    {0x91, 0x8f},
    {0x91, 0x96},
    {0x91, 0xa3},
    {0x91, 0xaf},
    {0x91, 0xc4},
    {0x91, 0xd7},
    {0x91, 0xe8},
    {0x91, 0x20},
    {0x92, 0x00},
    {0x93, 0x06},
    {0x93, 0xe3},
    {0x93, 0x05},
    {0x93, 0x05},
    {0x93, 0x00},
    {0x93, 0x04},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x96, 0x00},
    {0x97, 0x08},
    {0x97, 0x19},
    {0x97, 0x02},
    {0x97, 0x0c},
    {0x97, 0x24},
    {0x97, 0x30},
    {0x97, 0x28},
    {0x97, 0x26},
    {0x97, 0x02},
    {0x97, 0x98},
    {0x97, 0x80},
    {0x97, 0x00},
    {0x97, 0x00},
    {0xc3, 0xed},
    {0xa4, 0x00},
    {0xa8, 0x00},
    {0xc5, 0x11},
    {0xc6, 0x51},
    {0xbf, 0x80},
    {0xc7, 0x10},
    {0xb6, 0x66},
    {0xb8, 0xa5},
    {0xb7, 0x64},
    {0xb9, 0x7c},
    {0xb3, 0xaf},
    {0xb4, 0x97},
    {0xb5, 0xff},
    {0xb0, 0xc5},
    {0xb1, 0x94},
    {0xb2, 0x0f},
    {0xc4, 0x5c},
    {0xc0, 0x64},
    {0xc1, 0x4b},
    {0x8c, 0x00},
    {0x86, 0x3d},
    {0x50, 0x00},
    {0x51, 0xc8},
    {0x52, 0x96},
    {0x53, 0x00},
    {0x54, 0x00},
    {0x55, 0x00},
    {0x5a, 0xc8},
    {0x5b, 0x96},
    {0x5c, 0x00},
    {0xd3, 0x02},
    {0xc2, 0x0c},
    {0xc3, 0xed},
    {0x7f, 0x00},
    {0xda, 0x08},
    {0xe5, 0x1f},
    {0xe1, 0x67},
    {0xe0, 0x00},
    {0xdd, 0x7f},
    {0x05, 0x00},
        {0xff, 0x00}, // FF 0
    {0xe0, 0x04},
    {0x5a, 0x50},
    {0x5b, 0x3c},
    {0x5c, 0x00},
    {0xe0, 0x00},
    {0x00, 0x00}
};


static const uint8_t default_regs[][2] = {

// From Linux Driver.

    {BANK_SEL,      BANK_SEL_DSP},
    {0x2c,          0xff},
    {0x2e,          0xdf},
    {BANK_SEL,      BANK_SEL_SENSOR},
    {0x3c,          0x32},
    {CLKRC,         0x00}, // CLKRC_DOUBLE
    {COM2,          COM2_OUT_DRIVE_4x},
    {REG04,         REG04_SET(REG04_HFLIP_IMG | 0)},
    //{COM8,          COM8_SET(COM8_BNDF_EN | COM8_AGC_EN | COM8_AEC_EN)},
    //{COM9,          COM9_AGC_SET(COM9_AGC_GAIN_8x)},
    {0x2c,          0x0c},
    {0x33,          0x78},
    {0x3a,          0x33},
    {0x3b,          0xfb},
    {0x3e,          0x00},
    {0x43,          0x11},
    {0x16,          0x10},
    {0x39,          0x02},
    {0x35,          0x88},
    {0x22,          0x0a},
    {0x37,          0x40},
    {0x23,          0x00},
    {ARCOM2,        0xa0},
    {0x06,          0x02},
    {0x06,          0x88},
    {0x07,          0xc0},
    {0x0d,          0xb7},
    {0x0e,          0x01},
    {0x4c,          0x00},
    {0x4a,          0x81},
    {0x21,          0x99},
    {AEW,           0x40},
    {AEB,           0x38},
    {VV,            VV_AGC_TH_SET(0x08, 0x02)},
    {0x5c,          0x00},
    {0x63,          0x00},
    {FLL,           0x22},
    {COM3,          COM3_BAND_SET(COM3_BAND_AUTO)},
    {REG5D,         0x55},
    {REG5E,         0x7d},
    {REG5F,         0x7d},
    {REG60,         0x55},
    {HISTO_LOW,     0x70},
    {HISTO_HIGH,    0x80},
    {0x7c,          0x05},
    {0x20,          0x80},
    {0x28,          0x30},
    {0x6c,          0x00},
    {0x6d,          0x80},
    {0x6e,          0x00},
    {0x70,          0x02},
    {0x71,          0x94},
    {0x73,          0xc1},
    {0x3d,          0x34},
    {COM7,          COM7_RES_UXGA | COM7_ZOOM_EN},
    {0x5a,          0x57},
    {COM25,         0x00},
    {BD50,          0xbb},
    {BD60,          0x9c},
    {BANK_SEL,      BANK_SEL_DSP},
    {0xe5,          0x7f},
    {MC_BIST,       MC_BIST_RESET | MC_BIST_BOOT_ROM_SEL},
    {0x41,          0x24},
    {RESET,         RESET_JPEG | RESET_DVP},
    {0x76,          0xff},
    {0x33,          0xa0},
    {0x42,          0x20},
    {0x43,          0x18},
    {0x4c,          0x00},
    {CTRL3,         CTRL3_BPC_EN | CTRL3_WPC_EN | 0x10},
    {0x88,          0x3f},
    {0xd7,          0x03},
    {0xd9,          0x10},
    {R_DVP_SP,      R_DVP_SP_AUTO_MODE | 0x2},
    {0xc8,          0x08},
    {0xc9,          0x80},
    {BPADDR,        0x00},
    {BPDATA,        0x00},
    {BPADDR,        0x03},
    {BPDATA,        0x48},
    {BPDATA,        0x48},
    {BPADDR,        0x08},
    {BPDATA,        0x20},
    {BPDATA,        0x10},
    {BPDATA,        0x0e},
    {0x90,          0x00},
    {0x91,          0x0e},
    {0x91,          0x1a},
    {0x91,          0x31},
    {0x91,          0x5a},
    {0x91,          0x69},
    {0x91,          0x75},
    {0x91,          0x7e},
    {0x91,          0x88},
    {0x91,          0x8f},
    {0x91,          0x96},
    {0x91,          0xa3},
    {0x91,          0xaf},
    {0x91,          0xc4},
    {0x91,          0xd7},
    {0x91,          0xe8},
    {0x91,          0x20},
    {0x92,          0x00},
    {0x93,          0x06},
    {0x93,          0xe3},
    {0x93,          0x03},
    {0x93,          0x03},
    {0x93,          0x00},
    {0x93,          0x02},
    {0x93,          0x00},
    {0x93,          0x00},
    {0x93,          0x00},
    {0x93,          0x00},
    {0x93,          0x00},
    {0x93,          0x00},
    {0x93,          0x00},
    {0x96,          0x00},
    {0x97,          0x08},
    {0x97,          0x19},
    {0x97,          0x02},
    {0x97,          0x0c},
    {0x97,          0x24},
    {0x97,          0x30},
    {0x97,          0x28},
    {0x97,          0x26},
    {0x97,          0x02},
    {0x97,          0x98},
    {0x97,          0x80},
    {0x97,          0x00},
    {0x97,          0x00},
    {0xa4,          0x00},
    {0xa8,          0x00},
    {0xc5,          0x11},
    {0xc6,          0x51},
    {0xbf,          0x80},
    {0xc7,          0x10},  /* simple AWB */
    {0xb6,          0x66},
    {0xb8,          0xA5},
    {0xb7,          0x64},
    {0xb9,          0x7C},
    {0xb3,          0xaf},
    {0xb4,          0x97},
    {0xb5,          0xFF},
    {0xb0,          0xC5},
    {0xb1,          0x94},
    {0xb2,          0x0f},
    {0xc4,          0x5c},
    {0xa6,          0x00},
    {0xa7,          0x20},
    {0xa7,          0xd8},
    {0xa7,          0x1b},
    {0xa7,          0x31},
    {0xa7,          0x00},
    {0xa7,          0x18},
    {0xa7,          0x20},
    {0xa7,          0xd8},
    {0xa7,          0x19},
    {0xa7,          0x31},
    {0xa7,          0x00},
    {0xa7,          0x18},
    {0xa7,          0x20},
    {0xa7,          0xd8},
    {0xa7,          0x19},
    {0xa7,          0x31},
    {0xa7,          0x00},
    {0xa7,          0x18},
    {0x7f,          0x00},
    {0xe5,          0x1f},
    {0xe1,          0x77},
    {0xdd,          0x7f},
    {CTRL0,          CTRL0_YUV_EN | CTRL0_RGB_EN },

// OpenMV Custom.
     {BANK_SEL,      BANK_SEL_DSP},
     {R_BYPASS,      R_BYPASS_DSP_EN},
     {IMAGE_MODE,    IMAGE_MODE_RGB565},
    //{0xd7,          0x03},
    {RESET,         0x00},
     {R_BYPASS,      R_BYPASS_DSP_EN},

    // {BANK_SEL,      BANK_SEL_SENSOR},
    // {0x0f,          0x4b},
    // {COM1,          0x8f},

// End.

    {0x00,          0x00},
};


int ov2640_init(void)
{
    uint16_t v_manuf_id;
    uint16_t v_device_id;
    ov2640_read_id(&v_manuf_id, &v_device_id);
    printf("ov2640_init: manuf_id:0x%04x,device_id:0x%04x\r\n", v_manuf_id, v_device_id);
    uint16_t index = 0;
    for (index = 0; ov2640_config[index][0]; index++)
      dvp_sccb_send_data(OV2640_ADDR, ov2640_config[index][0], ov2640_config[index][1]);
    return 0;
}

int ov2640_init2(void)
{
  uint16_t v_manuf_id;
  uint16_t v_device_id;
  ov2640_read_id(&v_manuf_id, &v_device_id);
  printf("ov2640_init2: manuf_id:0x%04x,device_id:0x%04x\r\n", v_manuf_id, v_device_id);
  uint16_t index = 0;
  for (index = 0; default_regs[index][0]; index++)
    dvp_sccb_send_data(OV2640_ADDR, default_regs[index][0], default_regs[index][1]);

  return 0;
}

int ov2640_read_id(uint16_t *manuf_id, uint16_t *device_id)
{
    dvp_sccb_send_data(OV2640_ADDR, 0xFF, 0x01);
    *manuf_id = (dvp_sccb_receive_data(OV2640_ADDR, 0x1C) << 8) | dvp_sccb_receive_data(OV2640_ADDR, 0x1D);
    *device_id = (dvp_sccb_receive_data(OV2640_ADDR, 0x0A) << 8) | dvp_sccb_receive_data(OV2640_ADDR, 0x0B);
    return 0;
}

typedef enum {
    FRAMESIZE_INVALID = 0,
    // C/SIF Resolutions
    FRAMESIZE_QQCIF,    // 88x72
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_CIF,      // 352x288
    FRAMESIZE_QQSIF,    // 88x60
    FRAMESIZE_QSIF,     // 176x120
    FRAMESIZE_SIF,      // 352x240
    // VGA Resolutions
    FRAMESIZE_QQQQVGA,  // 40x30
    FRAMESIZE_QQQVGA,   // 80x60
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_HQQQVGA,  // 60x40
    FRAMESIZE_HQQVGA,   // 120x80
    FRAMESIZE_HQVGA,    // 240x160
    // FFT Resolutions
    FRAMESIZE_64X32,    // 64x32
    FRAMESIZE_64X64,    // 64x64
    FRAMESIZE_128X64,   // 128x64
    FRAMESIZE_128X128,  // 128x128
    // Other
    FRAMESIZE_LCD,      // 128x160
    FRAMESIZE_QQVGA2,   // 128x160
    FRAMESIZE_WVGA,     // 720x480
    FRAMESIZE_WVGA2,    // 752x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
} framesize_t;

const int resolution[][2] = {
    {0,    0   },
    // C/SIF Resolutions
    {88,   72  },    /* QQCIF     */
    {176,  144 },    /* QCIF      */
    {352,  288 },    /* CIF       */
    {88,   60  },    /* QQSIF     */
    {176,  120 },    /* QSIF      */
    {352,  240 },    /* SIF       */
    // VGA Resolutions
    {40,   30  },    /* QQQQVGA   */
    {80,   60  },    /* QQQVGA    */
    {160,  120 },    /* QQVGA     */
    {320,  240 },    /* QVGA      */
    {640,  480 },    /* VGA       */
    {60,   40  },    /* HQQQVGA   */
    {120,  80  },    /* HQQVGA    */
    {240,  160 },    /* HQVGA     */
    // FFT Resolutions
    {64,   32  },    /* 64x32     */
    {64,   64  },    /* 64x64     */
    {128,  64  },    /* 128x64    */
    {128,  128 },    /* 128x64    */
    // Other
    {128,  160 },    /* LCD       */
    {128,  160 },    /* QQVGA2    */
    {720,  480 },    /* WVGA      */
    {752,  480 },    /* WVGA2     */
    {800,  600 },    /* SVGA      */
    {1024, 768 },    /* XGA       */
    {1280, 1024},    /* SXGA      */
    {1600, 1200},    /* UXGA      */
};



#define IM_LOG2_2(x)    (((x) &                0x2ULL) ? ( 2                        ) :             1) // NO ({ ... }) !
#define IM_LOG2_4(x)    (((x) &                0xCULL) ? ( 2 +  IM_LOG2_2((x) >>  2)) :  IM_LOG2_2(x)) // NO ({ ... }) !
#define IM_LOG2_8(x)    (((x) &               0xF0ULL) ? ( 4 +  IM_LOG2_4((x) >>  4)) :  IM_LOG2_4(x)) // NO ({ ... }) !
#define IM_LOG2_16(x)   (((x) &             0xFF00ULL) ? ( 8 +  IM_LOG2_8((x) >>  8)) :  IM_LOG2_8(x)) // NO ({ ... }) !
#define IM_LOG2_32(x)   (((x) &         0xFFFF0000ULL) ? (16 + IM_LOG2_16((x) >> 16)) : IM_LOG2_16(x)) // NO ({ ... }) !
#define IM_LOG2(x)      (((x) & 0xFFFFFFFF00000000ULL) ? (32 + IM_LOG2_32((x) >> 32)) : IM_LOG2_32(x)) // NO ({ ... }) !

#define IM_MAX(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define IM_MIN(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define IM_DIV(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a / _b) : 0; })
#define IM_MOD(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a % _b) : 0; })


#define CIF_WIDTH       (400)
#define CIF_HEIGHT      (296)

#define SVGA_WIDTH      (800)
#define SVGA_HEIGHT     (600)

#define UXGA_WIDTH      (1600)
#define UXGA_HEIGHT     (1200)


static const uint8_t svga_regs[][2] = {
    {BANK_SEL,  BANK_SEL_SENSOR},
    {COM7,      COM7_RES_SVGA},
    {COM1,      0x0A | 0x40},
    {HSTART,    0x11},
    {HSTOP,     0x43},
    {VSTART,    0x01}, // 0x01 fixes issue with garbage pixels in the image...
    {VSTOP,     0x97},
    {REG32,     0x09},
    {BANK_SEL,  BANK_SEL_DSP},
    //{RESET,     RESET_DVP},
    {SIZEL,     SIZEL_HSIZE8_11_SET(SVGA_WIDTH) | SIZEL_HSIZE8_SET(SVGA_WIDTH) | SIZEL_VSIZE8_SET(SVGA_HEIGHT)},
    {HSIZE8,    HSIZE8_SET(SVGA_WIDTH)},
    {VSIZE8,    VSIZE8_SET(SVGA_HEIGHT)},
    //{CTRL2,     CTRL2_DCW_EN | CTRL2_SDE_EN | CTRL2_UV_AVG_EN | CTRL2_CMX_EN | CTRL2_UV_ADJ_EN},
    {0,         0},
};

static const uint8_t uxga_regs[][2] = {
  {BANK_SEL,  BANK_SEL_SENSOR},
  {COM7,      COM7_RES_UXGA},
  {COM1,      0x0F | 0x80},
  {HSTART,    0x11},
  {HSTOP,     0x75},
  {VSTART,    0x01},
  {VSTOP,     0x97},
  {REG32,     0x36},
  {BANK_SEL,  BANK_SEL_DSP},
  {RESET,     RESET_DVP},
  {SIZEL,     SIZEL_HSIZE8_11_SET(UXGA_WIDTH) | SIZEL_HSIZE8_SET(UXGA_WIDTH) | SIZEL_VSIZE8_SET(UXGA_HEIGHT)},
  {HSIZE8,    HSIZE8_SET(UXGA_WIDTH)},
  {VSIZE8,    VSIZE8_SET(UXGA_HEIGHT)},
  {CTRL2,     CTRL2_DCW_EN | CTRL2_SDE_EN | CTRL2_UV_AVG_EN | CTRL2_CMX_EN | CTRL2_UV_ADJ_EN},
  {0,0}
};

void ov_2640_set_framesize(bool uxga, uint16_t width, uint16_t height,
                           uint16_t x_offset, uint16_t y_offset,
                           uint16_t out_width, uint16_t out_height)
{
  framesize_t framesize = FRAMESIZE_UXGA;

    const uint8_t (*regs)[2];
    uint16_t sensor_w = 0;
    uint16_t sensor_h = 0;
    uint16_t w = resolution[framesize][0];
    uint16_t h = resolution[framesize][1];

    if (!uxga) {
        regs = svga_regs;
        sensor_w = SVGA_WIDTH;
        sensor_h = SVGA_HEIGHT;
    } else {
        regs = uxga_regs;
        sensor_w = UXGA_WIDTH;
        sensor_h = UXGA_HEIGHT;
    }

    // Write setup regsiters
    for (int i = 0; regs[i][0]; i++) {
      dvp_sccb_send_data(OV2640_ADDR, regs[i][0], regs[i][1]);
    }

    //uint64_t tmp_div = IM_MIN(sensor_w / w, sensor_h / h);
    //uint16_t log_div = IM_MIN(IM_LOG2(tmp_div) - 1, 3);
    //uint16_t div = 1 << log_div;
    //uint16_t w_mul = w * div;
    //uint16_t h_mul = h * div;
    //uint16_t x_off = (sensor_w - w_mul) / 2;
    //uint16_t y_off = (sensor_h - h_mul) / 2;

    uint16_t w_z = out_width;
    uint16_t h_z = out_height;

    dvp_sccb_send_data(OV2640_ADDR, CTRLI, CTRLI_LP_DP);
    // CTRLI_V_DIV_SET(log_div) | CTRLI_H_DIV_SET(log_div)
    dvp_sccb_send_data(OV2640_ADDR, HSIZE, HSIZE_SET(width));
    dvp_sccb_send_data(OV2640_ADDR, VSIZE, VSIZE_SET(height));
    dvp_sccb_send_data(OV2640_ADDR, XOFFL, XOFFL_SET(x_offset));
    dvp_sccb_send_data(OV2640_ADDR, YOFFL, YOFFL_SET(y_offset));
    dvp_sccb_send_data(OV2640_ADDR, VHYX, VHYX_HSIZE_SET(width) | VHYX_VSIZE_SET(height) | VHYX_XOFF_SET(x_offset) | VHYX_YOFF_SET(y_offset));
    dvp_sccb_send_data(OV2640_ADDR, TEST, TEST_HSIZE_SET(width));
    dvp_sccb_send_data(OV2640_ADDR, ZMOW, ZMOW_OUTW_SET(w_z));
    dvp_sccb_send_data(OV2640_ADDR, ZMOH, ZMOH_OUTH_SET(h_z));
    dvp_sccb_send_data(OV2640_ADDR, ZMHH, ZMHH_OUTW_SET(w_z) | ZMHH_OUTH_SET(h_z));
    //dvp_sccb_send_data(OV2640_ADDR, R_DVP_SP, div);
    dvp_sccb_send_data(OV2640_ADDR, RESET, 0x00);

    // Delay 300 ms
    //msleep(300);

}
