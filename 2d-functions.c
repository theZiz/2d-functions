#include <string.h>
#include <sparrow3d.h>
#include <math.h>
#ifndef NO_BLAS
	#include <cblas.h>
#endif

#include "defines.h"

SDL_Surface* screen;
spFontPointer font = NULL;
float zoom = 2.0f;
float s = 0.0f;
int one_step = 4096;
int pause = 1;
int draw_field = 0;

#include "helpers.c"
#include "phasenraum.c"
#include "all.c"

tPhasenraum X_Raum, Y_Raum, Z_Raum;

void draw(void)
{
	char buffer[256];
	spSetAlphaTest(0);
	if (!draw_field)
		spClearTarget(8);
	drawPhasenraumAll(&X_Raum,0,0,screen->w/2,screen->h/2);
	spRotozoomSurface(screen->w*15/32,screen->h*14/32,0,spFontGetLetter(font,'X')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	drawPhasenraumAll(&Y_Raum,screen->w/2+1,0,screen->w-1,screen->h/2);
	spRotozoomSurface(screen->w*17/32,screen->h*14/32,0,spFontGetLetter(font,'Y')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	drawPhasenraumAll(&Z_Raum,0,screen->h/2+1,screen->w/2,screen->h-1);
	spRotozoomSurface(screen->w*15/32,screen->h*18/32,0,spFontGetLetter(font,'Z')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	drawAllAll(&X_Raum,&Y_Raum,&Z_Raum,screen->w/2+1,screen->h/2+1,screen->w-1,screen->h-1);
	spRotozoomSurface(screen->w*17/32,screen->h*18/32,0,spFontGetLetter(font,'3')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	spRotozoomSurface(screen->w*18/32,screen->h*18/32,0,spFontGetLetter(font,'D')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	line(0,screen->h/2,0,screen->w-1,screen->h/2,0);
	line(screen->w/2,0,0,screen->w/2,screen->h-1,0);

	spFontDrawMiddle(screen->w/2,font->maxheight*0,0,"[B] Exit",font);
	spFontDrawMiddle(screen->w/2,font->maxheight*1,0,"[R] Pause",font);
	spFontDrawMiddle(screen->w/2,font->maxheight*2,0,"[d] Restart",font);

	spFontDrawMiddle(screen->w/2,screen->h-font->maxheight*2,0,"[a] Colorful!",font);
	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDrawMiddle(screen->w/2,screen->h-font->maxheight*1,0,buffer,font);

	spFlip();
}

int calc(Uint32 steps)
{
	calcPhasenraum(&X_Raum,0,0,screen->w/2,screen->h/2,steps);
	calcPhasenraum(&Y_Raum,screen->w/2+1,0,screen->w-1,screen->h/2,steps);
	calcPhasenraum(&Z_Raum,0,screen->h/2+1,screen->w/2,screen->h-1,steps);
	if (spGetInput()->button[SP_BUTTON_LEFT_NOWASD])
	{
		draw_field = 1-draw_field;
		spGetInput()->button[SP_BUTTON_LEFT_NOWASD] = 0;
	}
	if (spGetInput()->button[SP_BUTTON_RIGHT_NOWASD])
	{
		s = 0.0f;
		spGetInput()->button[SP_BUTTON_RIGHT_NOWASD] = 0;
	}
	if (spGetInput()->button[SP_BUTTON_START_NOWASD])
	{
		pause = 1-pause;
		spGetInput()->button[SP_BUTTON_START_NOWASD] = 0;
	}
	if (spGetInput()->button[SP_BUTTON_SELECT_NOWASD])
		return 1;
	return 0;
}

void resize( Uint16 w, Uint16 h )
{
	spSelectRenderTarget(screen);
	//Font Loading
	if ( font )
		spFontDelete( font );
	spFontSetShadeColor(0);
	font = spFontLoad( "./data/DejaVuSans-Bold.ttf", 6 * spGetSizeFactor() >> SP_ACCURACY );
	spFontAdd( font, SP_FONT_GROUP_ASCII"°αβγεφσ√ᵥ₁₂₃", 65535 ); //whole ASCII
	spFontAddButton( font, 'R', SP_BUTTON_START_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); //Return == START
	spFontAddButton( font, 'B', SP_BUTTON_SELECT_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); //Backspace == SELECT
	spFontAddButton( font, 'q', SP_BUTTON_L_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // q == L
	spFontAddButton( font, 'e', SP_BUTTON_R_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // e == R
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); //a == left button
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // d == right button
	spFontAddButton( font, 'w', SP_BUTTON_UP_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // w == up button
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // s == down button
	spFontMulWidth(font,spFloatToFixed(0.85f));
	spFontAddBorder(font , 0);//spGetRGB(128,128,128));
	spSelectRenderTarget(screen);
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	initPhasenraum(&X_Raum,3.0f,5.0f,1.0f);
	initPhasenraum(&Y_Raum,4.0f,4.0f,1.5f);
	initPhasenraum(&Z_Raum,5.0f,3.0f,2.0f);
	spSetDefaultWindowSize( WINDOW_X, WINDOW_Y );
	spInitCore();
	screen = spCreateDefaultWindow();
	resize( screen->w, screen->h );
	spSetZSet(0);
	spSetZTest(0);
	spLoop(draw,calc,10,resize,NULL);
	spQuitCore();
	return 0;
}
