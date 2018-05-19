#ifndef GPU_H 
#define GPU_H 


// Available GFX Configurations:

// RES_320x240 (QVGA) IS NOT SUPPORTED FOR THE DEMO PARTY
// IF YOU WISH TO USE THIS SETTING, PLEASE BRING YOUR OWN PROJECTOR

// Single Buffered:
// RES_80x480, SINGLEBUFFERED, BPP_1 = 4800KB
// RES_80x480, SINGLEBUFFERED, BPP_2 = 9600KB
// RES_80x480, SINGLEBUFFERED, BPP_4 = 19200KB
// RES_80x480, SINGLEBUFFERED, BPP_8 = 38400KB
// RES_80x480, SINGLEBUFFERED, BPP_16 = 76800KB

// RES_160x480, SINGLEBUFFERED, BPP_1 = 9600KB
// RES_160x480, SINGLEBUFFERED, BPP_2 = 19200KB
// RES_160x480, SINGLEBUFFERED, BPP_4 = 38400KB
// RES_160x480, SINGLEBUFFERED, BPP_8 = 76800KB

// RES_320x480, SINGLEBUFFERED, BPP_1 = 19200KB
// RES_320x480, SINGLEBUFFERED, BPP_2 = 38400KB
// RES_320x480, SINGLEBUFFERED, BPP_4 = 76800KB

// RES_480x480, SINGLEBUFFERED, BPP_1 = 28800KB
// RES_480x480, SINGLEBUFFERED, BPP_2 = 57600KB

// RES_640x480, SINGLEBUFFERED, BPP_1 = 38400KB
// RES_640x480, SINGLEBUFFERED, BPP_2 = 76800KB

// Double Buffered:
//    For double buffering, only combinations with 
//    total buffer size greater than 32KB work.
//    Not sure why... let us know if you figure it out.

// RES_80x480, DOUBLEBUFFERED, BPP_4 = 38400KB
// RES_80x480, DOUBLEBUFFERED, BPP_8 = 76800KB

// RES_160x480, DOUBLEBUFFERED, BPP_2 = 38400KB
// RES_160x480, DOUBLEBUFFERED, BPP_4 = 76800KB

// RES_320x480, DOUBLEBUFFERED, BPP_1 = 38400KB
// RES_320x480, DOUBLEBUFFERED, BPP_2 = 76800KB

// RES_480x480, DOUBLEBUFFERED, BPP_1 = 57600KB

// RES_640x480, DOUBLEBUFFERED, BPP_1 = 76800KB


struct GFXConfig
{
    uint16_t frame_buffers;

    uint16_t clock_div;

    uint32_t hres;
    uint32_t vres;

    uint32_t bpp;

    uint32_t hfp;
    uint32_t hpw;
    uint32_t hbp;

    uint32_t vfp;
    uint32_t vpw;
    uint32_t vbp;

    uint32_t hscale;

    uint32_t buffer_size;

    uint32_t fb_offset;
};

typedef enum 
{ 
    RES_320x240,
    RES_80x480,
    RES_160x480,
    RES_320x480,
    RES_480x480,
    RES_640x480
} resolution;

typedef enum
{
    SINGLEBUFFERED = 1,
    DOUBLEBUFFERED = 2
} framebuffers;

typedef enum
{
    BPP_1 = 1,
    BPP_2 = 2,
    BPP_4 = 4,
    BPP_8 = 8,
    BPP_16 = 16
} colordepth;

extern volatile struct GFXConfig gfx;  

#define VENST_FUDGE 0
#define HENST_FUDGE 0
#define VSPOL 0                 // sync polarities
#define HSPOL 0

#define CHR_FGCOLOR	     	    0x5000
#define CHR_BGCOLOR	     	    0x5100
#define CHR_FONTBASE            0x5200
#define CHR_PRINTCHAR           0x5300
#define CHR_PRINTCHARTRANS	    0x5380
#define CHR_TXTAREASTART        0x5800
#define CHR_TXTAREAEND          0x5900
#define CHR_PRINTPOS	        0x5A00
#define RCC_SRCADDR	     	    0x6200
#define RCC_DESTADDR	        0x6300
#define RCC_RECTSIZE	        0x6400
#define RCC_COLOR	     	    0x6600
#define RCC_STARTCOPY	        0x6700
#define IPU_SRCADDR	     	    0x7100
#define IPU_DESTADDR            0x7200
#define IPU_DECOMPRESS          0x7400

// MAX BUFFER SIZE MUST BE GREATER THAN 32KB IF DOUBLE BUFFERING
#define GFX_MAX_BUFFER_SIZE 76800

extern volatile uint32_t frames;

extern __eds__ uint8_t GFXDisplayBuffer[GFX_MAX_BUFFER_SIZE] __attribute__((section("DISPLAY"),space(eds)));

void __attribute__((interrupt, auto_psv))_GFX1Interrupt(void);

void gpu_init(void);

uint8_t gpu_set_res(resolution res, framebuffers fb, colordepth bpp);
void gpu_config(void);
void gpu_set_fb(__eds__ uint8_t *buf);
void gpu_flip_fb(void);
void gpu_clear_fb(void);
void gpu_clear_all_fb(void);
void gpu_draw_border(uint16_t c);

void gpu_config_clut();
void gpu_clut_set(uint8_t index, uint16_t color);

void gpu_config_chr(void);
void gpu_chr_print(char *c, uint16_t x, uint16_t y, uint8_t transparent);
void gpu_chr_fg_color(unsigned int color);
void gpu_chr_bg_color(unsigned int color);

void rcc_set_fb_dest(__eds__ uint8_t *buf);
void rcc_color(unsigned int color);
void rcc_rec(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void rcc_pixel(unsigned long ax, unsigned long ay);
void rcc_line(float x1, float y1, float x2, float y2, uint8_t color);

#endif
