#include <string.h>
#include <sparrow3d.h>
#include <math.h>
#ifndef NO_BLAS
	#include <cblas.h>
#endif

#include "defines.h"

SDL_Surface* screen;
SDL_Surface* threeD = NULL;
spFontPointer font = NULL;
float zoom = 2.0f;
float s = 0.0f;
int one_step = 13;
int pause = 1;
int draw_field = 0;
Sint32 rotation[16];
int test_values = 1;


#include "helpers.c"
#include "phasenraum.c"
#include "all.c"

tPhasenraum X_Raum, Y_Raum, Z_Raum;

void draw(void)
{
	char buffer[256];
	drawAllAll(&X_Raum,&Y_Raum,&Z_Raum,screen->w/2+1,screen->h/2+1,screen->w-1,screen->h-1);
	spSetAlphaTest(1);
	spRotozoomSurface(screen->w*17/32,screen->h*18/32,0,spFontGetLetter(font,'3')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	spRotozoomSurface(screen->w*18/32,screen->h*18/32,0,spFontGetLetter(font,'D')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	drawPhasenraumAll(&X_Raum,0,0,screen->w/2,screen->h/2);
	spRotozoomSurface(screen->w*15/32,screen->h*14/32,0,spFontGetLetter(font,'X')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	drawPhasenraumAll(&Y_Raum,screen->w/2+1,0,screen->w-1,screen->h/2);
	spRotozoomSurface(screen->w*17/32,screen->h*14/32,0,spFontGetLetter(font,'Y')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	drawPhasenraumAll(&Z_Raum,0,screen->h/2+1,screen->w/2,screen->h-1);
	spRotozoomSurface(screen->w*15/32,screen->h*18/32,0,spFontGetLetter(font,'Z')->surface,spFloatToFixed(2.0f),spFloatToFixed(2.0f),0);
	line(0,screen->h/2,0,screen->w-1,screen->h/2,0,65535);
	line(screen->w/2,0,0,screen->w/2,screen->h-1,0,65535);

	spFontDrawMiddle(screen->w/2,font->maxheight*0,0,"[B] Exit",font);
	spFontDrawMiddle(screen->w/2,font->maxheight*1,0,"[R] Pause",font);
	spFontDrawMiddle(screen->w/2,font->maxheight*2,0,"[d] Restart",font);
	spFontDrawMiddle(screen->w/2,font->maxheight*3,0,"[a] Colorful!",font);
	sprintf(buffer,"s: %.3f",s);
	spFontDrawMiddle(screen->w/2,font->maxheight*4,0,buffer,font);

	spFontDrawMiddle(screen->w/2,screen->h-font->maxheight*4,0,"[s] Element!",font);
	spFontDrawMiddle(screen->w/2,screen->h-font->maxheight*3,0,"[w] Switch ELBE / Test values",font);
	if (one_step > 0)
		sprintf(buffer,SP_PAD_NAME": Speed: 2^%i=%.5f",-one_step,spFixedToFloat(SP_ONE>>one_step));
	else
		sprintf(buffer,SP_PAD_NAME": Speed: 2^%i=%.5f",-one_step,spFixedToFloat(SP_ONE<<-one_step));
	spFontDrawMiddle(screen->w/2,screen->h-font->maxheight*2,0,buffer,font);
	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDrawMiddle(screen->w/2,screen->h-font->maxheight*1,0,buffer,font);

	spFlip();
}

int calc(Uint32 steps)
{
	int x,y;
	int mouse = SDL_GetRelativeMouseState(&x,&y);
	if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT) || mouse & SDL_BUTTON(SDL_BUTTON_LEFT) || mouse & SDL_BUTTON(SDL_BUTTON_MIDDLE))
	{
		memcpy(spGetMatrix(),rotation,sizeof(Sint32)*16);
		if (x)
			spRotateY( -x*512 );
		if (y)
			spRotateX( -y*512 );
		memcpy(rotation,spGetMatrix(),sizeof(Sint32)*16);
	}
	if (spGetInput()->axis[0] > 0 && one_step>-14)
		one_step--;
	if (spGetInput()->axis[0] < 0 && one_step< 15)
		one_step++;
	spGetInput()->axis[0] = 0;
	if (spGetInput()->button[SP_BUTTON_UP_NOWASD])
	{
		test_values = 1-test_values;
		if (test_values)
		{
			one_step+=8;
			if (one_step>15)
				one_step = 15;
			zoom*=256.0f;
			s = 0.0f;
			initPhasenraum(&X_Raum,3.0f,5.0f,1.0f,spGetRGB(255,0,0),1);
			initPhasenraum(&Y_Raum,4.0f,4.0f,1.5f,spGetRGB(0,255,0),1);
			initPhasenraum(&Z_Raum,5.0f,3.0f,2.0f,spGetRGB(255,255,0),1);
		}
		else
		{
			one_step-=8;
			if (one_step<-14)
				one_step =-14;
			zoom/=256.0f;
			s = 0.0f;
			loadAll(&X_Raum,&Y_Raum,&Z_Raum,spGetRGB(255,0,0),spGetRGB(0,255,0),spGetRGB(255,255,0));
		}
		spGetInput()->button[SP_BUTTON_UP_NOWASD] = 0;
	}
	if (spGetInput()->button[SP_BUTTON_DOWN_NOWASD])
	{
		all_new_matrix(&X_Raum,&Y_Raum,&Z_Raum);
		spGetInput()->button[SP_BUTTON_DOWN_NOWASD] = 0;
	}
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
	calcPhasenraum(&X_Raum,0,0,screen->w/2,screen->h/2,steps);
	calcPhasenraum(&Y_Raum,screen->w/2+1,0,screen->w-1,screen->h/2,steps);
	calcPhasenraum(&Z_Raum,0,screen->h/2+1,screen->w/2,screen->h-1,steps);
	calcAll(&X_Raum,&Y_Raum,&Z_Raum,screen->w/2+1,screen->h/2+1,screen->w-1,screen->h-1,steps);
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
	updateThreeD(w/2,h/2);
}

void event(SDL_Event *e)
{
	if (e->type == SDL_MOUSEBUTTONDOWN)
	switch (e->button.button)
	{
		case SDL_BUTTON_WHEELUP:
			zoom*=0.95f;
			break;
		case SDL_BUTTON_WHEELDOWN:
			zoom*=1.0f/0.95f;
			break;
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	initPhasenraum(&X_Raum,3.0f,5.0f,1.0f,spGetRGB(255,0,0),1);
	initPhasenraum(&Y_Raum,4.0f,4.0f,1.5f,spGetRGB(0,255,0),1);
	initPhasenraum(&Z_Raum,5.0f,3.0f,2.0f,spGetRGB(255,255,0),1);
	spSetDefaultWindowSize( WINDOW_X, WINDOW_Y );
	spInitCore();
	screen = spCreateDefaultWindow();
	resize( screen->w, screen->h );
	spSetZSet(0);
	spSetZTest(0);
	spCollapsePrimitives(0);
	//Init the rotation matrix
	spIdentity();
	spRotateX(SP_PI/4);
	spRotateY(SP_PI/4);
	spRotateZ(SP_PI/4);
	memcpy(rotation,spGetMatrix(),sizeof(Sint32)*16);
	spLoop(draw,calc,10,resize,event);
	spDeleteSurface(threeD);
	spQuitCore();
	return 0;
}
