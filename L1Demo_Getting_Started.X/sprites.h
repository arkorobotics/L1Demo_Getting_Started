#ifndef SPRITES_H 
#define SPRITES_H 

#define MAX_PARTICLES 30

#define SPR_DATA_HEAD 5     // LEGACY
#define SPR_INFO_HEAD 16
#define SPR_OFF_COLOR 5

#define SPR_OFF_WIDTH 0
#define SPR_OFF_HEIGHT 1
#define SPR_OFF_BITRES 2
#define SPR_OFF_TRANS 3
#define SPR_OFF_ROTATE 4
#define SPR_COLOR_1 5
#define SPR_COLOR_2 6
#define SPR_COLOR_3 7
#define SPR_COLOR_4 8
#define SPR_COLOR_5 9
#define SPR_COLOR_6 10
#define SPR_COLOR_7 11
#define SPR_COLOR_8 12
#define SPR_COLOR_9 13
#define SPR_COLOR_10 14
#define SPR_COLOR_11 15

#define SPR_OFF_DATA 0

// SPRITE DEFINITIONS

#define MAX_SPRITES 11

#define SPR_FIRE_16_A 0
#define SPR_FIRE_16_B 1
#define SPR_FIRE_8_A 2
#define SPR_FIRE_8_B 3

#define SPR_INT_LAYER 4
#define SPR_INT_ONE 5
#define SPR_INT_BORD_A 6
#define SPR_INT_BORD_B 7


struct SpriteInfo
{
    uint16_t width;  // Width (in pixels)
    uint16_t height; // Height (in pixels)
    uint16_t bitres; // Bits per Pixel
    uint16_t trans;  // Transparency
    uint16_t rotate; // Rotation, 0: none, 1: 90 cw, 2: 180, 3: 90 ccw
    uint16_t color_array[11];   // Color Look Up Table Array
};

struct Sprite {
    struct SpriteInfo info;
	__prog__ uint8_t *data;  // Pointer to sprite pixel data
};

struct Particle
{
    uint16_t size;
    uint16_t posx;
    uint16_t posy;
    uint16_t speedx;
    uint16_t speedy;
    uint16_t color;
};

extern struct Sprite s[MAX_SPRITES];

//extern uint16_t color_array[8];

extern __eds__ struct Particle p[MAX_PARTICLES];
extern int numPart;

void inline loadAllSprites(void);
void inline loadSpriteCLUT(uint16_t id);
void inline drawSprite(uint16_t x, uint16_t y, uint16_t id, uint16_t rotation, uint8_t crt);
int inline nrange(double a, double b);
void drawSpriteRotation(uint16_t x, uint16_t y, uint16_t id, float rotation);
void addParticle(void);


#endif
