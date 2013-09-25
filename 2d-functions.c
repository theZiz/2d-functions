#include <string.h>
#include <sparrow3d.h>
#include <math.h>
float rotation = 0.0f;
SDL_Surface* screen;
spFontPointer font = NULL;

#define WINDOW_X 800
#define WINDOW_Y 480

typedef struct { float x,y;} tPoint;
#define PARTICLE_COUNT 500
tPoint start_particle[PARTICLE_COUNT];
tPoint particle[PARTICLE_COUNT];

#define calc_gamma(alpha,beta) ((1+alpha*alpha) / beta)
#define start_alpha 3.0f
#define start_beta 5.0f
float start_gamma = calc_gamma(start_alpha,start_beta);
float alpha;
float beta;
float Gamma;
float epsilon = 1.0f;
float EPSILON;
//#define function(x,y) (x*x+y*y-1.0f)
#define function(x,y) (Gamma*x*x+2.0f*alpha*x*y+beta*y*y-epsilon)

//Drift Strecke:
// | 1  -2*l   l*l |
// | 0     1    -l |
// | 0     0     1 |
//C function
#define Cf(s) 1
//C derivation
#define Cd(s) 0
//S function
#define Sf(s) s
//S derivation
#define Sd(s) 1

float s = 0.0f;

#define RASTER_X (WINDOW_X/2)
#define RASTER_Y (WINDOW_Y/2)

float marching_points[RASTER_X+1][RASTER_Y+1];
float zoom = 2.0f;

int pause = 1;
int draw_field = 0;

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

void line(int x1,int y1,int z1,int x2,int y2,int z2)
{
	spSetLineWidth(2);
	spLine(x1,y1,z1,x2,y2,z2,spGetRGB(128,128,128));
	spSetLineWidth(1);
	if (x1 == x2)
	{
		if (y1 < y2)
			spLine(x1,y1+1,z1,x2,y2-1,z2,65535);
		else
			spLine(x1,y1-1,z1,x2,y2+1,z2,65535);
	}
	else
	{
		if (x1 < x2)
			spLine(x1+1,y1,z1,x2-1,y2,z2,65535);
		else
			spLine(x1-1,y1,z1,x2+1,y2,z2,65535);
	}
}

void draw(void)
{
	spSetAlphaTest(0);
	if (!draw_field)
		spClearTarget(8);
	int one;
	if (screen->w < screen->h)
		one = (float)(screen->w)/zoom;
	else
		one = (float)(screen->h)/zoom;
	int x,y,w,h;
	w = screen->w/RASTER_X;
	h = screen->h/RASTER_Y;
	if (draw_field)
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
	line(0,screen->h/2,0,screen->w,screen->h/2,0);
	line(screen->w/2,0,0,screen->w/2,screen->h,0);
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
	//Drawing the particles:
	int i;
	for (i = 0; i < PARTICLE_COUNT; i++)
	{
		int x = (int)(particle[i].x*one/2.0f) + screen->w/2;
		int y = (int)(particle[i].y*one/2.0f) + screen->h/2;
		spEllipse(x,y,0,2,2,spGetRGB(0,255,127));
	}
	float ATAN = 2.0f*alpha/(Gamma-beta);
	float phi = atan(ATAN)/2.0f;
	if (ATAN < 0.0f)
		phi = -phi;
	float size_factor = spFixedToFloat(spGetSizeFactor());
	//marking some specific points:
	//Angle cross:
	spSetAlphaTest(1);
	char buffer[256];
	sprintf(buffer,"%.2f°",phi*180.0f/M_PI);
	spFontDrawRight(screen->w-1,screen->h-font->maxheight,-1,buffer,font);
	line(screen->w/2-(int)(cos(phi)*size_factor*30.0f),screen->h/2-(int)(sin(phi)*size_factor*30.0f),0,
	     screen->w/2+(int)(cos(phi)*size_factor*30.0f),screen->h/2+(int)(sin(phi)*size_factor*30.0f),0);
	line(screen->w/2-(int)(cos(phi+M_PI/2.0f)*size_factor*30.0f),screen->h/2-(int)(sin(phi+M_PI/2.0f)*size_factor*30.0f),0,
	     screen->w/2+(int)(cos(phi+M_PI/2.0f)*size_factor*30.0f),screen->h/2+(int)(sin(phi+M_PI/2.0f)*size_factor*30.0f),0);

	x = (int)(-alpha*sqrt(epsilon/Gamma)*(float)one)/2;
	y = (int)(sqrt(epsilon*Gamma)*(float)one)/2;
	line( screen->w*19/40+x, screen->h/2+y, 0,
	      screen->w*21/40+x, screen->h/2+y, 0);
	line( screen->w*19/40-x, screen->h/2-y, 0,
	      screen->w*21/40-x, screen->h/2-y, 0);

	x = (int)(sqrt(epsilon*beta)*(float)one)/2;
	y = (int)(-alpha*sqrt(epsilon/beta)*(float)one)/2;
	line( screen->w/2+x, screen->h*19/40+y, 0,
	      screen->w/2+x, screen->h*21/40+y, 0);
	line( screen->w/2-x, screen->h*19/40-y, 0,
	      screen->w/2-x, screen->h*21/40-y, 0);

	y = (int)(sqrt(epsilon/beta)*(float)one)/2;
	line(screen->w*19/40,screen->h/2-y,0,
	     screen->w*21/40,screen->h/2-y,0);
	sprintf(buffer,"sqrt(epsilon/beta)=%.2f",sqrt(epsilon/beta));
	spFontDrawMiddle(screen->w/2,screen->h/2-y-font->maxheight,0,buffer,font);

	x = (int)(sqrt(epsilon/Gamma)*(float)one)/2;
	line(screen->w/2+x,screen->h*19/40,0,
	     screen->w/2+x,screen->h*21/40,0);
	sprintf(buffer," sqrt(epsilon/gamma)=%.2f",sqrt(epsilon/Gamma));
	spFontDraw(screen->w/2+x,screen->h/2-font->maxheight/2,0,buffer,font);

	if (!pause)
	{
		sprintf(buffer,"alpha=%.2f",alpha);
		spFontDraw(2,screen->h-font->maxheight*5,0,buffer,font);
		sprintf(buffer,"beta=%.2f",beta);
		spFontDraw(2,screen->h-font->maxheight*4,0,buffer,font);
		sprintf(buffer,"epsilon=%.2f",epsilon);
		spFontDraw(2,screen->h-font->maxheight*2,0,buffer,font);
	}
	else
	{
		sprintf(buffer,"alpha=%.2f ("SP_PAD_NAME")",alpha);
		spFontDraw(2,screen->h-font->maxheight*5,0,buffer,font);
		sprintf(buffer,"beta=%.2f ("SP_PAD_NAME")",beta);
		spFontDraw(2,screen->h-font->maxheight*4,0,buffer,font);
		sprintf(buffer,"epsilon=%.2f ([q] & [e])",epsilon);
		spFontDraw(2,screen->h-font->maxheight*2,0,buffer,font);
	}	
	sprintf(buffer,"gamma=%.2f",Gamma);
	spFontDraw(2,screen->h-font->maxheight*3,0,buffer,font);
	sprintf(buffer,"epsilon (~)=%.2f",EPSILON);
	spFontDraw(2,screen->h-font->maxheight*1,0,buffer,font);

	float SIN = sin(phi);
	float COS = cos(phi);
	float a=sqrt(epsilon/(Gamma*COS*COS-2.0f*alpha*COS*SIN+beta*SIN*SIN));
	float b=sqrt(epsilon/(Gamma*SIN*SIN+2.0f*alpha*COS*SIN+beta*COS*COS));
	sprintf(buffer,"a=%.2f",a);
	spFontDrawRight(screen->w-1,screen->h-font->maxheight*5,-1,buffer,font);
	sprintf(buffer,"b=%.2f",b);
	spFontDrawRight(screen->w-1,screen->h-font->maxheight*4,-1,buffer,font);
	sprintf(buffer,"s=%.2f",s);
	spFontDrawRight(screen->w-1,screen->h-font->maxheight*3,-1,buffer,font);
	float s_w = start_alpha / start_gamma;
	sprintf(buffer,"s_w=%.2f",s_w);
	spFontDrawRight(screen->w-1,screen->h-font->maxheight*2,-1,buffer,font);

	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDraw(2,2,0,buffer,font);
	spFontDraw(2,2+font->maxheight  ,0,"[B] Exit",font);
	spFontDraw(2,2+font->maxheight*2,0,"[R] Pause",font);
	spFontDraw(2,2+font->maxheight*3,0,"[d] Restart",font);
	spFontDraw(2,2+font->maxheight*4,0,"[a] Colorful!",font);
	spFlip();
}

int calc(Uint32 steps)
{
	if (spGetInput()->button[SP_BUTTON_LEFT_NOWASD])
	{
		spGetInput()->button[SP_BUTTON_LEFT_NOWASD] = 0;
		draw_field = 1-draw_field;
	}
	if (spGetInput()->button[SP_BUTTON_RIGHT_NOWASD])
	{
		spGetInput()->button[SP_BUTTON_RIGHT_NOWASD] = 0;
		s = 0.0f;
	}
	if (pause)
	{
		if (spGetInput()->button[SP_BUTTON_L_NOWASD])
			epsilon -= (float)steps/2000.0f;
		if (spGetInput()->button[SP_BUTTON_R_NOWASD])
			epsilon += (float)steps/2000.0f;
		if (spGetInput()->axis[0] < 0)
			alpha -= (float)steps/2000.0f;
		if (spGetInput()->axis[0] > 0)
			alpha += (float)steps/2000.0f;
		if (spGetInput()->axis[1] < 0)
			beta -= (float)steps/2000.0f;
		if (spGetInput()->axis[1] > 0)
			beta += (float)steps/2000.0f;
		if (beta < 0.0f)
			beta = 0.0f;
		if (epsilon < 0.0f)
			epsilon = 0.0f;
		Gamma = calc_gamma(alpha,beta);
	}
	else
	{
		/*alpha = sin(rotation);
		beta = 1.5f+cos(rotation*1.5f);
		Gamma = calc_gamma(alpha,beta);*/
		s += (float)steps/5000.0f;
		beta = Cf(s)*Cf(s)*start_beta
		     + -2.0f*Sf(s)*Cf(s)*start_alpha
		     + Sf(s)*Sf(s)*start_gamma;
		alpha = -Cf(s)*Cd(s)*start_beta
		      + (Sd(s)*Cf(s)+Sf(s)*Cd(s))*start_alpha
		      + -Sf(s)*Sd(s)*start_gamma;
		Gamma = Cd(s)*Cd(s)*start_beta
		      + -2.0f*Sd(s)*Cd(s)*start_alpha
		      + Sd(s)*Sd(s)*start_gamma;
		/*alpha = start_alpha;
		beta = start_beta;
		Gamma = start_gamma;*/
		int i;
		for (i = 0; i < PARTICLE_COUNT; i++)
		{
			particle[i].x = Cf(s)*start_particle[i].x
			              + Sf(s)*start_particle[i].y;
			particle[i].y = Cd(s)*start_particle[i].x
			              + Sd(s)*start_particle[i].y;
		}
	}
	//update the marching points
	int x,y;
	float square_size;
	if (screen->w < screen->h)
	{
		square_size = (1.0f / (float)RASTER_X * zoom * 2.0f) * (1.0f / (float)RASTER_Y * zoom * 2.0f * (float)(screen->h)/(float)(screen->w));
		for (x = 0; x <= RASTER_X; x++)
			for (y = 0; y <= RASTER_Y; y++)
			{
				float f_x = ((float)x / (float)RASTER_X - 0.5f) * zoom * 2.0f;
				float f_y = ((float)y / (float)RASTER_Y - 0.5f) * zoom * 2.0f * (float)(screen->h)/(float)(screen->w);
				marching_points[x][y] = function(f_x,f_y);
			}
	}
	else
	{
		square_size = (1.0f / (float)RASTER_Y * zoom * 2.0f) * (1.0f / (float)RASTER_X * zoom * 2.0f * (float)(screen->w)/(float)(screen->h));
		for (x = 0; x <= RASTER_X; x++)
			for (y = 0; y <= RASTER_Y; y++)
			{
				float f_x = ((float)x / (float)RASTER_X - 0.5f) * zoom * 2.0f * (float)(screen->w)/(float)(screen->h);
				float f_y = ((float)y / (float)RASTER_Y - 0.5f) * zoom * 2.0f;
				marching_points[x][y] = function(f_x,f_y);
			}
	}
	EPSILON = 0.0f;
	for (x = 0; x <= RASTER_X; x++)
		for (y = 0; y <= RASTER_Y; y++)
			if (marching_points[x][y] <= 0.0f)
				EPSILON += square_size;
	EPSILON /= M_PI;
	if (spGetInput()->button[SP_BUTTON_START_NOWASD])
	{
		pause = 1-pause;
		spGetInput()->button[SP_BUTTON_START_NOWASD] = 0;
	}

	if (!pause)
		rotation += (float)steps/2000.0f;
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
	font = spFontLoad( "./data/CabinCondensed-Regular.ttf", 10 * spGetSizeFactor() >> SP_ACCURACY );
	spFontAdd( font, SP_FONT_GROUP_ASCII"°", 65535 ); //whole ASCII
	spFontAddButton( font, 'R', SP_BUTTON_START_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); //Return == START
	spFontAddButton( font, 'B', SP_BUTTON_SELECT_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); //Backspace == SELECT
	spFontAddButton( font, 'q', SP_BUTTON_L_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // q == L
	spFontAddButton( font, 'e', SP_BUTTON_R_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // e == R
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); //a == left button
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // d == right button
	spFontAddButton( font, 'w', SP_BUTTON_UP_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // w == up button
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NOWASD_NAME, 65535, SP_ALPHA_COLOR ); // s == down button
	spFontMulWidth(font,spFloatToFixed(0.85f));
	spFontAddBorder(font , spGetRGB(128,128,128));
	spSelectRenderTarget(screen);
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	alpha = start_alpha;
	beta = start_beta;
	Gamma = start_gamma;
	//dicing the particles positions:
	float ATAN = 2.0f*alpha/(Gamma-beta);
	float phi = atan(ATAN)/2.0f;
	if (ATAN < 0.0f)
		phi = -phi;
	float SIN = sin(phi);
	float COS = cos(phi);
	float a=sqrt(epsilon/(Gamma*COS*COS-2.0f*alpha*COS*SIN+beta*SIN*SIN));
	float b=sqrt(epsilon/(Gamma*SIN*SIN+2.0f*alpha*COS*SIN+beta*COS*COS));
	int i;
	for (i = 0; i < PARTICLE_COUNT; i++)
	{
		float u1,u2,q;
		do
		{
			u1 = ((float)(rand())/(float)RAND_MAX)*2.0f-1.0f;
			u2 = ((float)(rand())/(float)RAND_MAX)*2.0f-1.0f;
			q = u1*u1+u2*u2;
		}
		while (q == 0.0f || q > 1.0f);
		float p = sqrt(-2.0f*log(q)/q);
		float x = u1*p/3.0f*a;
		float y = u2*p/3.0f*b;
		particle[i].x = start_particle[i].x = x*COS+y*SIN;
		particle[i].y = start_particle[i].y = x*-SIN+y*COS;
	}
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
