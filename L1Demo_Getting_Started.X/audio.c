#include "xc.h"
#include "audio.h"
#include "music.h"

void config_timer(void) 
{
	PR1 = 0xFF;
	_T1IP = 5;	// set interrupt priority
	_TON  = 1;	// turn on the timer
	_T1IF = 0;	// reset interrupt flag
	_T1IE = 1;	// turn on the timer1 interrupt
}

//_T1Interrupt() is the T1 interrupt service routine (ISR).
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    // Song index
	static unsigned int idx = 0;

	static unsigned short duration = 0;
    static unsigned int ch1_ncount = 0;
    static unsigned int ch2_ncount = 0;
    static unsigned int ch3_ncount = 0;

    static unsigned short ch1_val = 0;
    static unsigned short ch2_val = 0;
    static unsigned short ch3_val = 0;

    // Do Channel 1 Stuff
    if (ch1_ncount < 0x7F80) {
        ch1_ncount += song_ch1f[idx];
    }
    else
    {
        ch1_ncount = 0;
    }
    ch1_val = sinetable[ch1_ncount>>6];

    // Do Channel 2 Stuff
    if (ch2_ncount < 0x7F80) {
        ch2_ncount+=song_ch8f[idx];
    }
    else
    {
        ch2_ncount = 0;
    }
    ch2_val = sinetable[ch2_ncount>>6];

    // Do Channel 3 Stuff
    if (ch3_ncount < 0x7F80) {
        ch3_ncount+=song_ch15f[idx];
    }
    else
    {
        ch3_ncount = 0;
    }
    ch3_val = sinetable[ch3_ncount>>6];

    
	// DURATION
	if(duration < 0x7A1)
	{
	 	duration++;
	}
    else
    {
	 	idx++;

		if(idx == sizeof(song_ch15f) / sizeof(song_ch15f[0]) ) /* loop it! */
		{
			idx = 0x36c;
		}

		duration = 0;
	}
	
	// MIX AND SET AUDIO OUTPUT
	//PORTB = (sample_1>>2)+(sample_2>>2)+(sample_3>>2); //+(sample_4>>3);
    
    PORTB = (ch1_val<<6) + (ch2_val<<6) + (ch3_val<<6);

	_T1IF = 0;
}

