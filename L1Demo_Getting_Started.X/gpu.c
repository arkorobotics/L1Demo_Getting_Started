#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include <math.h>
#include "fonts.h"
#include "gpu.h"

#define  FCY    16000000UL    // Instruction cycle frequency, Hz
#include <libpic30.h>

uint16_t frames = 0;

#ifdef DOUBLE_BUFFERED
__eds__ uint8_t GFXDisplayBuffer[2][GFX_BUFFER_SIZE] __attribute__((section("DISPLAY"),space(eds)));
#else
__eds__ uint8_t GFXDisplayBuffer[GFX_BUFFER_SIZE] __attribute__((section("DISPLAY"),space(eds)));
#endif

volatile int fb_ready = 0;
volatile int vsync = 0;
int next_fb = 0;

#ifdef DOUBLE_BUFFERED
void __attribute__((interrupt, auto_psv))_GFX1Interrupt(void) 
{
	static int lines = 0;
	static int syncs = 0;
	static int next_fb = 1;
	if(_VMRGNIF) { /* on a vertical sync, flip buffers if it's ready */
		lines = 0;
		syncs++;
		if(fb_ready) {
			gpu_setfb(GFXDisplayBuffer[next_fb]);
			next_fb = !next_fb;
		}
		//Nop();Nop();
		fb_ready = 0;
		_VMRGNIF = 0;
	} else if(_HMRGNIF) { /* on each horizontal sync, ...? */
		lines++;
		_HMRGNIF = 0;
	}
	_GFX1IF = 0;
}
#else
void __attribute__((interrupt, auto_psv))_GFX1Interrupt(void) 
{
    // Wait until the vertical sync
    if(_VMRGNIF) {
        vsync = 0;
        _VMRGNIF = 0;
    }
    _GFX1IF = 0;
}
#endif

void config_graphics(void) 
{
    _G1CLKSEL = 1;
    _GCLKDIV = CLOCKDIV;

    // Display buffer:
    G1DPADRL = (unsigned long)(GFXDisplayBuffer) & 0xFFFF;

    // Work area 1
    G1W1ADRL = (unsigned long)(GFXDisplayBuffer) & 0xFFFF;

    // Work area 2
    G1W2ADRL = (unsigned long)(GFXDisplayBuffer) & 0xFFFF;

    #ifdef DOUBLE_BUFFERED
    G1DPADRH = (unsigned long)(GFXDisplayBuffer) >>16 & 0xFF;
    G1W1ADRH = (unsigned long)(GFXDisplayBuffer) >>16 & 0xFF;
    G1W2ADRH = (unsigned long)(GFXDisplayBuffer) >>16 & 0xFF;
    #endif


    G1PUW = HOR_RES;
    G1PUH = VER_RES;

    // Using PIC24F manual section 43 page 37-38
    _DPMODE = 1;      /* TFT */
    _GDBEN = 0xFFFF;
    _DPW = _PUW = HOR_RES; // Work area and FB size so GPU works
    _DPH = _PUH = VER_RES;
    _DPWT = HOR_FRONT_PORCH + HOR_PULSE_WIDTH + HOR_BACK_PORCH + HOR_RES;

    // _DPHT may need to be adjusted for vertical centering
    _DPHT = (VER_FRONT_PORCH + VER_PULSE_WIDTH + VER_BACK_PORCH)*2 + VER_RES;
    _DPCLKPOL = 0;
    _DPENOE = 0;
    _DPENPOL = 0;
    _DPVSOE = 1;          /* use VSYNC */
    _DPHSOE = 1;          /* use HSYNC */
    _DPVSPOL = VSPOL;     /* VSYNC negative polarity (if VSPOL = 0)*/
    _DPHSPOL = HSPOL;     /* HSYNC negative polarity (if HSPOL = 0)*/

    // _ACTLINE may need to be adjusted for vertical centering
    _ACTLINE = _VENST = VER_FRONT_PORCH + VER_PULSE_WIDTH + VER_BACK_PORCH - VENST_FUDGE;
    _ACTPIX = _HENST = HOR_FRONT_PORCH + HOR_PULSE_WIDTH + HOR_BACK_PORCH - HENST_FUDGE;
    _VSST = VER_FRONT_PORCH;
    _HSST = HOR_FRONT_PORCH;
    _VSLEN = VER_PULSE_WIDTH;
    _HSLEN = HOR_PULSE_WIDTH;
    _DPPWROE = 0;
    _DPPINOE = 1;
    _DPPOWER = 1;

    int logc=0;
    while (BPP>>logc > 1) { logc++; }
    _DPBPP = _PUBPP = logc;

    _G1EN = 1;
    __delay_ms(1);
}

void config_clut() 
{
    _CLUTEN = 1; 
}

void clut_set(uint8_t index, uint16_t color) 
{
    _CLUTADR = index;
    _CLUTRWEN = 1;
    G1CLUTWR = color;
    while(_CLUTBUSY) continue;
    _CLUTRWEN = 0;
}

void config_chr(void) 
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
    G1CMDL = (uint16_t)(FontStart) & 0xFFFF;
    G1CMDH = CHR_FONTBASE;
    Nop();
 
    while(_CMDFUL) continue;
    G1CMDL = 0;
    G1CMDH = CHR_TXTAREASTART;
    Nop();
 
    while(_CMDFUL) continue;
    G1CMDL = (HOR_RES & 0xF)<<12 | VER_RES;
    G1CMDH = CHR_TXTAREAEND | (HOR_RES >>4);
    Nop();
}

// Thanks Jamis for this function :P
int maxCharHeight = ((int)VER_RES)-21;
void chr_print(char *c, uint16_t x, uint16_t y, uint8_t transparent) 
{

    if (y > maxCharHeight) {
        y = maxCharHeight;
    }

    while(_CMDFUL) continue;
    G1CMDL = x<<12 | y;
    G1CMDH = CHR_PRINTPOS | (x>>4); // CHR_PRINTPOS = 0x5A00
    Nop();

    while(*c != NULL) {
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

void rcc_color(unsigned int color) 
{
	G1CMDL = color;
	G1CMDH = RCC_COLOR;
}

void chr_fg_color(unsigned int color) 
{
    G1CMDL = color;
    G1CMDH = CHR_FGCOLOR;
}
 
void chr_bg_color(unsigned int color) 
{
    G1CMDL = color;
    G1CMDH = CHR_BGCOLOR;
}

void rcc_setdest(__eds__ uint8_t *buf) 
{
	while(!_CMDMPT) continue; // Wait for GPU to finish drawing
	G1W1ADRL = (unsigned long)(buf);
	G1W1ADRH = (unsigned long)(buf);
	G1W2ADRL = (unsigned long)(buf);
	G1W2ADRH = (unsigned long)(buf);
}
 
void gpu_setfb(__eds__ uint8_t *buf) 
{
	G1DPADRL = (unsigned long)(buf);
	G1DPADRH = (unsigned long)(buf);
}

void waitForBufferFlip() 
{
//    while(!_CMDMPT) continue; // Wait for GPU to finish drawing
    while((!_CMDMPT) | _IPUBUSY | _RCCBUSY | _CHRBUSY) continue; // Wait for IPU, RCC, and CHR GPUs to finish drawing
    #ifdef DOUBLE_BUFFERED
        fb_ready = 1;
        while(fb_ready) continue; // wait for vsync
    #else
        vsync = 1;
        while(vsync) continue; // wait for vsync
    #endif
}

void swapBuffers() 
{
    rcc_setdest(GFXDisplayBuffer[next_fb]);
    next_fb = !next_fb;
    blank_background();
}

void rcc_draw(uint16_t x, uint16_t y, uint16_t w, uint16_t h) 
{
	// destination
	while(_CMDFUL) continue;
	G1CMDL = x + y*HOR_RES;
	G1CMDH = RCC_DESTADDR | (x + y*HOR_RES)>>16;
	Nop();
 
	// size
	while(_CMDFUL) continue;
	G1CMDL = (w<<12) | h;
	G1CMDH = RCC_RECTSIZE | (w>>4);
	Nop();
 
	// go!
	while(_CMDFUL) continue;
	G1CMDL = 0xC<<3;
	G1CMDH = RCC_STARTCOPY;
	Nop();
}

void fast_pixel(unsigned long ax, unsigned long ay) 
{
	//ax += (ay << 9) + (ay << 7);
	ax += ay*HOR_RES;
	G1CMDL = ax;
	G1CMDH = RCC_DESTADDR | ax>>16;

	G1CMDL = 0x1001; // This needs to be changed for non 80x
	G1CMDH = RCC_RECTSIZE;

	while(_CMDFUL) continue;
	G1CMDL = 0x60;
	G1CMDH = RCC_STARTCOPY;
	Nop();
}
 
void blank_background(void) 
{
	//rcc_color(0xff);
	//rcc_color(0x0);
    rcc_color(15);
	rcc_draw(0, 0, HOR_RES-1, VER_RES);
}

void drawBorder(uint16_t c) 
{
	// screen border
	rcc_color(c);
	rcc_draw(0,0, 1, VER_RES); // left
	rcc_draw(HOR_RES-2,0,1,VER_RES); // right
	rcc_draw(0,0, HOR_RES-2, PIX_H); // top
	rcc_draw(0,VER_RES-PIX_H,HOR_RES-2,PIX_H); // bottom
}

void cleanup(void) 
{ // TODO: custom colors
	// Clean right column
	//rcc_color(0xe0); // neat effect
	//rcc_color(0xff); // white
	//rcc_color(0x00);
	rcc_color(15);
    rcc_draw(HOR_RES-1,0, 1,VER_RES);
}

void clearbuffers(void)
{
    // clear buffers
    #ifdef DOUBLE_BUFFERED
    rcc_setdest(GFXDisplayBuffer[0]);
    blank_background();
    rcc_setdest(GFXDisplayBuffer[1]);
    #endif
    blank_background();
}


float radians(uint16_t angle) 
{
	return ((angle*3.14159)/180.0);
}

int realtoint(float oldval, float oldmin, float oldmax, float newmin, float newmax) 
{
	return (int)((((oldval - oldmin) * (newmax - newmin)) / (oldmax - oldmin)) + newmin);
}

void drawLineS(float x1, float y1, float x2, float y2) 
{
	return;
}

void line (float x1, float y1, float x2, float y2, uint8_t color) 
{
        unsigned int i;
        double hl=fabs(x2-x1), vl=fabs(y2-y1), length=(hl>vl)?hl:vl;
        float deltax=(x2-x1)/(float)length, deltay=(y2-y1)/(float)length;
        for (i=0; i<(int)length; i++) {
                unsigned long x=(int)(x1+=deltax), y=(int)(y1+=deltay);
                if ((x<HOR_RES)&&(y<VER_RES)) {
                        rcc_color(color);
                        //rcc_color(0x3);
                        //rcc_draw(x,y, PIX_W,PIX_H);
                        // TODO: fix y displacement
			fast_pixel(x,y+6);// + i*PIX_H);
                        }
                }
}

void render (float xa, float ya, float za) 
{
        int i;
        float mat[4][4];            // Determine rotation matrix
        float xdeg=xa*3.1416f/180, ydeg=ya*3.1416f/180, zdeg=za*3.1416f/180;
        float sx=(float)sin(xdeg), sy=(float)sin(ydeg), sz=(float)sin(zdeg);
        float cx=(float)cos(xdeg), cy=(float)cos(ydeg), cz=(float)cos(zdeg);
        float x[8], y[8], z[8], rx[8], ry[8], rz[8], scrx[8], scry[8];

        mat[0][0]=cx*cz+sx*sy*sz, mat[1][0]=-cx*sz+cz*sx*sy, mat[2][0]=cy*sx;
        mat[0][1]=cy*sz, mat[1][1]=cy*cz, mat[2][1]=-sy;
        mat[0][2]=-cz*sx+cx*sy*sz, mat[1][2]=sx*sz+cx*cz*sy, mat[2][2]=cx*cy;
        for (i=0; i<8; i++) {
                rx[i]=x[i]*mat[0][0]+y[i]*mat[1][0]+z[i]*mat[2][0];
                ry[i]=x[i]*mat[0][1]+y[i]*mat[1][1]+z[i]*mat[2][1];
                rz[i]=x[i]*mat[0][2]+y[i]*mat[1][2]+z[i]*mat[2][2]+100;
                scrx[i]=(rx[i]*10)/rz[i]+(HOR_RES/2), scry[i]=(ry[i]*60)/rz[i]+(VER_RES/2);
        }
        for (i=0; i<4; i++) {
                line (scrx[i], scry[i], scrx[i+4], scry[i+4],rand()%16);
                line (scrx[i], scry[i], scrx[(i+1)%4], scry[(i+1)%4],rand()%16);
                line (scrx[i+4], scry[i+4], scrx[((i+1)%4)+4], scry[((i+1)%4)+4],rand()%16);
        }
}