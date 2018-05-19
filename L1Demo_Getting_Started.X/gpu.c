#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include <math.h>
#include "fonts.h"
#include "gpu.h"
#include "alu.h"

#define  FCY    16000000UL    // Instruction cycle frequency, Hz
#include <libpic30.h>

volatile struct GFXConfig gfx;
volatile uint32_t frames = 0;
volatile static int vsync = 0;

__eds__ uint8_t GFXDisplayBuffer[GFX_MAX_BUFFER_SIZE] __attribute__((section("DISPLAY"),space(eds)));

void __attribute__((interrupt, auto_psv))_GFX1Interrupt(void) 
{
	volatile static int hlines = 0;
	
	if(_VMRGNIF)            // Vertical Sync Flag
    {
		hlines = 0;
		vsync = 0;
		_VMRGNIF = 0;
	} 
    else if(_HMRGNIF)       // Horizontal Sync Flag
    {
		hlines++;
		_HMRGNIF = 0;
	}
	_GFX1IF = 0;
}

void gpu_init(void)
{
    _VMRGNIF = 0;
    _VMRGNIE = 1;
    _GFX1IE = 1;
}

uint8_t gpu_set_res(resolution res, framebuffers fb, colordepth bpp)
{
    switch(res)
    {
        case RES_320x240:
            gfx.clock_div = 58;
            gfx.hres = 320;
            gfx.vres = 240;
            gfx.hfp = 32;
            gfx.hpw = 16;
            gfx.hbp = 32;
            gfx.vfp = 5;
            gfx.vpw = 3;
            gfx.vbp = 5;
            break;
        case RES_80x480:
            gfx.clock_div = 69;
            gfx.hres = 80;
            gfx.vres = 480;
            gfx.hfp = 32;
            gfx.hpw = 16;
            gfx.hbp = 32;
            gfx.vfp = 10;
            gfx.vpw = 5;
            gfx.vbp = 10;
            break;
        case RES_160x480:
            gfx.clock_div = 47;
            gfx.hres = 160;
            gfx.vres = 480;
            gfx.hfp = 32;
            gfx.hpw = 24;
            gfx.hbp = 32;
            gfx.vfp = 10;
            gfx.vpw = 5;
            gfx.vbp = 10;
            break;
        case RES_320x480:
            gfx.clock_div = 25;
            gfx.hres = 320;
            gfx.vres = 480;
            gfx.hfp = 32;
            gfx.hpw = 48;
            gfx.hbp = 32;
            gfx.vfp = 10;
            gfx.vpw = 5;
            gfx.vbp = 10;
            break;
        case RES_480x480:
            gfx.clock_div = 17;
            gfx.hres = 480;
            gfx.vres = 480;
            gfx.hfp = 32;
            gfx.hpw = 64;
            gfx.hbp = 32;
            gfx.vfp = 10;
            gfx.vpw = 5;
            gfx.vbp = 10;
            break;
        case RES_640x480:
            gfx.clock_div = 11;
            gfx.hres = 640;
            gfx.vres = 480;
            gfx.hfp = 16;
            gfx.hpw = 96;
            gfx.hbp = 48;
            gfx.vfp = 10;
            gfx.vpw = 2;
            gfx.vbp = 33;
            break;
    }

    gfx.frame_buffers = fb;
    gfx.bpp = bpp;
    gfx.hscale = gfx.vres / gfx.hres;
    
    if(fb == DOUBLEBUFFERED)
    {
        gfx.buffer_size = 2 * ((gfx.hres * gfx.vres) / (8/gfx.bpp));
        gfx.fb_offset = gfx.buffer_size / 2;
    }
    else
    {
        gfx.buffer_size = ((gfx.hres * gfx.vres) / (8/gfx.bpp));
        gfx.fb_offset = 0;
    }
    
    if(gfx.buffer_size <= GFX_MAX_BUFFER_SIZE)
    {
        gpu_config();
        return 1;
    }

    return 0;
}

void gpu_config(void) 
{
    _G1CLKSEL = 1;
    _GCLKDIV = gfx.clock_div;

    // Display buffer:
    G1DPADRL = (uint32_t)(&GFXDisplayBuffer[0]) & 0xFFFF;
    G1DPADRH = (uint32_t)(&GFXDisplayBuffer[0]) >>16 & 0xFF;

    // Work area 1
    G1W1ADRL = (uint32_t)(&GFXDisplayBuffer[0]) & 0xFFFF;
    G1W1ADRH = (uint32_t)(&GFXDisplayBuffer[0]) >>16 & 0xFF;

    // Work area 2
    G1W2ADRL = (uint32_t)(&GFXDisplayBuffer[0]) & 0xFFFF;
    G1W2ADRH = (uint32_t)(&GFXDisplayBuffer[0]) >>16 & 0xFF;
    
    // Horizontal and Vertical Resolution
    G1PUW = gfx.hres;
    G1PUH = gfx.vres;

    // Using PIC24F manual section 43 page 37-38
    _DPMODE = 1;            // TFT
    _GDBEN = 0xFFFF;
    _DPW = _PUW = gfx.hres; // Work area and FB size so GPU works
    _DPH = _PUH = gfx.vres;
    _DPWT = gfx.hfp + gfx.hpw + gfx.hbp + gfx.hres;

    // _DPHT may need to be adjusted for vertical centering
    _DPHT = (gfx.vfp + gfx.vpw + gfx.vbp)*2 + gfx.vres;
    _DPCLKPOL = 0;
    _DPENOE = 0;
    _DPENPOL = 0;
    _DPVSOE = 1;          // use VSYNC
    _DPHSOE = 1;          // use HSYNC
    _DPVSPOL = VSPOL;     // VSYNC negative polarity (if VSPOL = 0)
    _DPHSPOL = HSPOL;     // HSYNC negative polarity (if HSPOL = 0)

    // _ACTLINE may need to be adjusted for vertical centering
    _ACTLINE = _VENST = gfx.vfp + gfx.vpw + gfx.vbp - VENST_FUDGE;
    _ACTPIX = _HENST = gfx.hfp + gfx.hpw + gfx.hbp - HENST_FUDGE;
    _VSST = gfx.vfp;
    _HSST = gfx.hfp;
    _VSLEN = gfx.vpw;
    _HSLEN = gfx.hpw;
    _DPPWROE = 0;
    _DPPINOE = 1;
    _DPPOWER = 1;

    int logc=0;
    while (gfx.bpp>>logc > 1) { logc++; }
    _DPBPP = _PUBPP = logc;

    _VMRGNIE = 1;
    _HMRGNIE = 1;

    _G1EN = 1;
    __delay_ms(1);
}

void gpu_set_fb(__eds__ uint8_t *buf) 
{
    G1DPADRL = (uint32_t)(buf);
    G1DPADRH = (uint32_t)(buf);
}

void gpu_flip_fb() 
{
    static uint8_t fb_index = 1;

    while((!_CMDMPT) | _IPUBUSY | _RCCBUSY | _CHRBUSY) continue; // Wait for IPU, RCC, and CHR GPUs to finish drawing
    
    vsync = 1;

    while(vsync) continue;                                       // Wait for Vsync to clear

    if(gfx.frame_buffers == DOUBLEBUFFERED)
    {
        if(fb_index)
        {
            gpu_set_fb(&GFXDisplayBuffer[0]);
            rcc_set_fb_dest(&GFXDisplayBuffer[gfx.fb_offset]);
        }
        else
        {
            gpu_set_fb(&GFXDisplayBuffer[gfx.fb_offset]);
            rcc_set_fb_dest(&GFXDisplayBuffer[0]);
        }
        
        fb_index = !fb_index;
    }
}

void gpu_clear_fb(void) 
{
    rcc_color(0);
    rcc_rec(0, 0, gfx.hres, gfx.vres);
}

void gpu_clear_all_fb(void)
{
    // Clear frame buffers

    // If double buffered, clear the second frame first
    if(gfx.frame_buffers == DOUBLEBUFFERED)
    {
        rcc_set_fb_dest(&GFXDisplayBuffer[0]+gfx.fb_offset);
        gpu_clear_fb();
    }

    // Clear the first frame
    rcc_set_fb_dest(&GFXDisplayBuffer[0]);
    gpu_clear_fb();
}

void gpu_draw_border(uint16_t c) 
{
    // Screen border
    rcc_color(c);
    rcc_rec(0,0, 1, gfx.vres);                              // left
    rcc_rec(gfx.hres-1,0,1,gfx.vres);                       // right
    rcc_rec(0,0, gfx.hres, gfx.hscale);                     // top
    rcc_rec(0,gfx.vres-gfx.hscale,gfx.hres-1,gfx.hscale);   // bottom
}

void gpu_config_clut() 
{
    _CLUTEN = 1; 
}

void gpu_clut_set(uint8_t index, uint16_t color) 
{
    _CLUTADR = index;
    _CLUTRWEN = 1;
    G1CLUTWR = color;
    while(_CLUTBUSY) continue;
    _CLUTRWEN = 0;
}

void gpu_config_chr(void) 
{
    while(_CMDFUL) continue;
    G1CMDL = 0xFFFF;
    G1CMDH = CHR_FGCOLOR;
    Nop();
 
    while(_CMDFUL) continue;
    G1CMDL = 0;
    G1CMDH = CHR_BGCOLOR;
    Nop();
 
    while(_CMDFUL) continue;
    G1CMDL = (uint32_t)(&FontStart[0]) & 0xFFFF;
    G1CMDH = CHR_FONTBASE | (((uint32_t)(&FontStart[0])>>16) & 0xFF);
    Nop();
 
    while(_CMDFUL) continue;
    G1CMDL = 0;
    G1CMDH = CHR_TXTAREASTART;
    Nop();
 
    while(_CMDFUL) continue;
    G1CMDL = ((gfx.hres & 0xF) << 12) | gfx.vres;
    G1CMDH = CHR_TXTAREAEND | (gfx.hres >> 4);
    Nop();
}

void gpu_chr_print(char *c, uint16_t x, uint16_t y, uint8_t transparent) 
{
    // Credits: Jamis
    int maxCharHeight = ((int)gfx.vres)-21;

    if (y > maxCharHeight) 
    {
        y = maxCharHeight;
    }

    while(_CMDFUL) continue;
    G1CMDL = x<<12 | y;
    G1CMDH = CHR_PRINTPOS | (x>>4);
    Nop();

    while(*c != NULL) 
    {
        while(_CMDFUL) continue;
        G1CMDL = *c;
        if(transparent == 0)
        {
            G1CMDH = CHR_PRINTCHAR;
        }
        else
        {
            G1CMDH = CHR_PRINTCHARTRANS; // transparent
        }
        Nop();

        c++;
    }
}

void gpu_chr_fg_color(unsigned int color) 
{
    G1CMDL = color;
    G1CMDH = CHR_FGCOLOR;
}
 
void gpu_chr_bg_color(unsigned int color) 
{
    G1CMDL = color;
    G1CMDH = CHR_BGCOLOR;
}

void rcc_set_fb_dest(__eds__ uint8_t *buf) 
{
    while(!_CMDMPT) continue;   // Wait for GPU to finish drawing
    G1W1ADRL = (uint32_t)(buf);
    G1W1ADRH = (uint32_t)(buf);
    G1W2ADRL = (uint32_t)(buf);
    G1W2ADRH = (uint32_t)(buf);
}

void rcc_color(unsigned int color) 
{
	G1CMDL = color;
	G1CMDH = RCC_COLOR;
}

void rcc_rec(uint16_t x, uint16_t y, uint16_t w, uint16_t h) 
{
	// destination
	while(_CMDFUL) continue;
	G1CMDL = x + (y * gfx.hres);
	G1CMDH = RCC_DESTADDR | (((x + (y * (uint32_t)gfx.hres))>>16) & 0xFF);
	Nop();
 
	// size
	while(_CMDFUL) continue;
	G1CMDL = (w<<12) | h;
	G1CMDH = RCC_RECTSIZE | (w>>4);
	Nop();
 
	// go!
	while(_CMDFUL) continue;
	G1CMDL = 0x60;
	G1CMDH = RCC_STARTCOPY;
	Nop();
}

void rcc_pixel(unsigned long ax, unsigned long ay) 
{
	ax += ay * gfx.hres;
	G1CMDL = ax;
	G1CMDH = RCC_DESTADDR | ((ax>>16) & 0xFF);

	G1CMDL = 0x1000 | gfx.hscale;
	G1CMDH = RCC_RECTSIZE;

	while(_CMDFUL) continue;
	G1CMDL = 0x60;
	G1CMDH = RCC_STARTCOPY;
	Nop();
}

// TODO: Slow function, speed it up by changing datatypes and do approx
void rcc_line(float x1, float y1, float x2, float y2, uint8_t color) 
{
    unsigned int i;
    double hl=fabs(x2-x1), vl=fabs(y2-y1), length=(hl>vl)?hl:vl;
    float deltax=(x2-x1)/(float)length, deltay=(y2-y1)/(float)length;
    for (i=0; i<(int)length; i++) 
    {
        unsigned long x=(int)(x1+=deltax), y=(int)(y1+=deltay);
        if ((x<gfx.hres)&&(y<gfx.vres)) 
        {
            rcc_color(color);
            rcc_pixel(x,y);
        }
    }
}
