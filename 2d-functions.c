#include <string.h>
#include <sparrow3d.h>
#include <math.h>
Sint32 rotation = 0;
SDL_Surface* screen;
spFontPointer font = NULL;

#define WINDOW_X 800
#define WINDOW_Y 600

#define alpha 0.5f
#define beta 0.7f
#define gamma 0.5f
#define epsilon 1.0f
#define function(x,y) (gamma*x*x+2.0f*alpha*x*y+beta*y*y-epsilon)
//#define function(x,y) (x*x+y*y-1.0f)

#define RASTER_X WINDOW_X
#define RASTER_Y WINDOW_Y

float marching_points[RASTER_X+1][RASTER_Y+1];

float zoom = 5.0f;

int interpolate(int x1,float v1,int x2,float v2)
{
	return x1+(int)(v1/(v1-v2)*((float)x2-(float)x1));
}

void drawMarchingLine(int x1,int y1,float v1,
                       int x2,int y2,float v2,
                       int x3,int y3,float v3,
                       int x4,int y4,float v4,Uint16 color)
{
	int bin = (v1 < 0.0f) | ((v2 < 0.0f) << 1) | ((v3 < 0.0f) << 2) | ((v4 < 0.0f) << 3);
	int a,b;
	switch (bin)
	{
// 1 +------+ 2
//   |      |
//   |      |
//   |      |
// 4 +------+ 3
		case 0: break; // empty
		//one point, three points
		case 1:
		case 2+4+8:
			a = interpolate(x1,v1,x2,v2);
			b = interpolate(y1,v1,y4,v4);
			spLine(a,y1,0,x1,b,0,color);
			break;
		case 2:
		case 1+4+8:
			a = interpolate(x2,v2,x1,v1);
			b = interpolate(y2,v2,y3,v3);
			spLine(a,y2,0,x2,b,0,color);
			break;
		case 4:
		case 1+2+8:
			a = interpolate(x3,v3,x4,v4);
			b = interpolate(y3,v3,y2,v2);
			spLine(a,y3,0,x3,b,0,color);
			break;
		case 8:
		case 1+2+4:
			a = interpolate(x4,v4,x3,v3);
			b = interpolate(y4,v4,y1,v1);
			spLine(a,y4,0,x4,b,0,color);
			break;
		//two points, one line
		case 1+2:
		case 4+8:
			a = interpolate(y1,v1,y4,v4);
			b = interpolate(y2,v2,y3,v3);
			spLine(x1,a,0,x2,b,0,color);
			break;
		case 2+4:
		case 8+1:
			a = interpolate(x2,v2,x1,v1);
			b = interpolate(x3,v3,x4,v4);
			spLine(a,y2,0,b,y3,0,color);
			break;
		//two points, two lines
		case 1+4:
			a = interpolate(x1,v1,x2,v2);
			b = interpolate(y1,v1,y4,v4);
			spLine(a,y1,0,x1,b,0,color);
			a = interpolate(x3,v3,x4,v4);
			b = interpolate(y3,v3,y2,v2);
			spLine(a,y3,0,x3,b,0,color);
			break;
		case 2+8:
			a = interpolate(x2,v2,x1,v1);
			b = interpolate(y2,v2,y3,v3);
			spLine(a,y2,0,x2,b,0,color);
			a = interpolate(x4,v4,x3,v3);
			b = interpolate(y4,v4,y1,v1);
			spLine(a,y4,0,x4,b,0,color);
			break;
		case 15: break; // full
	}
}

void draw(void)
{
	spClearTarget(spGetRGB(0,0,32));
	spSetAlphaTest(0);
	int x,y,w,h;
	w = screen->w/RASTER_X;
	h = screen->h/RASTER_Y;
	for (x = 0; x <= RASTER_X; x++)
		for (y = 0; y <= RASTER_Y; y++)
		{
			int X = x*screen->w/RASTER_X;
			int Y = y*screen->h/RASTER_Y;
			Uint16 color;
			if (marching_points[x][y] > 0.0f)
			{
				float value = 1.0f - 1.0f / ( 1.0f + marching_points[x][y] );
				color = spGetHSV((int)((float)SP_PI-(float)SP_PI*value),255,255);
			}
			else
			{
				float value = 1.0f - 1.0f / ( 1.0f + -marching_points[x][y] );
				color = spGetHSV((int)((float)SP_PI+(float)SP_PI*value),255,255);
			}
			spRectangle(X,Y,0,w,h,color);
		}
	//Marching squares
	for (x = 0; x < RASTER_X; x++)
		for (y = 0; y < RASTER_Y; y++)
		{
			int X_1 =  x   *screen->w/RASTER_X;
			int Y_1 =  y   *screen->h/RASTER_Y;
			int X_2 = (x+1)*screen->w/RASTER_X;
			int Y_2 = (y+1)*screen->h/RASTER_Y;
			drawMarchingLine(X_1,Y_1,marching_points[x  ][y  ],
			                 X_2,Y_1,marching_points[x+1][y  ],
			                 X_2,Y_2,marching_points[x+1][y+1],
			                 X_1,Y_2,marching_points[x  ][y+1],65535);
		}
	spLine(0,screen->h/2,0,screen->w,screen->h/2,0,65535);
	spLine(screen->w/2,0,0,screen->w/2,screen->h,0,65535);
	float phi = atan(2.0f*alpha/(gamma-beta))/2.0f*180.0f/M_PI;
	char buffer[256];
	sprintf(buffer,"%.2f°",phi);
	spSetAlphaTest(1);
	spFontDrawRight(screen->w-1,screen->h-font->maxheight,-1,buffer,font);
	spFlip();
}

int pause = 0;
int calc(Uint32 steps)
{
	//update the marching points
	int x,y;
	for (x = 0; x <= RASTER_X; x++)
		for (y = 0; y <= RASTER_Y; y++)
		{
			float f_x,f_y;
			if (screen->w < screen->h)
			{
				f_x = ((float)x / (float)RASTER_X - 0.5f) * zoom * 2.0f;
				f_y = ((float)y / (float)RASTER_Y - 0.5f) * zoom * 2.0f * (float)(screen->h)/(float)(screen->w);
			}
			else
			{
				f_x = ((float)x / (float)RASTER_X - 0.5f) * zoom * 2.0f * (float)(screen->w)/(float)(screen->h);
				f_y = ((float)y / (float)RASTER_Y - 0.5f) * zoom * 2.0f;
			}
			marching_points[x][y] = function(f_x,f_y);
		}
	
	PspInput engineInput = spGetInput();
	if (engineInput->button[SP_BUTTON_SELECT])
	{
		pause = 1-pause;
		engineInput->button[SP_BUTTON_SELECT] = 0;
	}

	if (!pause)
		rotation += steps*32;
	if (engineInput->button[SP_BUTTON_START])
		return 1;
	return 0;
}

void resize( Uint16 w, Uint16 h )
{
	spSelectRenderTarget(screen);
	//Font Loading
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./data/LondrinaOutline-Regular.ttf", 20 * spGetSizeFactor() >> SP_ACCURACY );
	spFontAdd( font, SP_FONT_GROUP_ASCII"°", 65535 ); //whole ASCII
	spFontAddButton( font, 'R', SP_BUTTON_START_NAME, 65535, SP_ALPHA_COLOR ); //Return == START
	spFontAddButton( font, 'B', SP_BUTTON_SELECT_NAME, 65535, SP_ALPHA_COLOR ); //Backspace == SELECT
	spFontAddButton( font, 'q', SP_BUTTON_L_NAME, 65535, SP_ALPHA_COLOR ); // q == L
	spFontAddButton( font, 'e', SP_BUTTON_R_NAME, 65535, SP_ALPHA_COLOR ); // e == R
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NAME, 65535, SP_ALPHA_COLOR ); //a == left button
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NAME, 65535, SP_ALPHA_COLOR ); // d == right button
	spFontAddButton( font, 'w', SP_BUTTON_UP_NAME, 65535, SP_ALPHA_COLOR ); // w == up button
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NAME, 65535, SP_ALPHA_COLOR ); // s == down button
	
	spSelectRenderTarget(screen);
}

int main(int argc, char **argv)
{
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
