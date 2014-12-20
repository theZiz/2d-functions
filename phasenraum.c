typedef struct {
	float marching_points[RASTER_X+1][RASTER_Y+1];
	float start_particle[2][PARTICLE_COUNT];
	float particle[2][PARTICLE_COUNT];
	float start_alpha;
	float start_beta;
	float start_gamma;
	float epsilon;
	float alpha;
	float beta;
	float gamma;
	Uint16 color;
} tPhasenraum;

float calc_phi(float alpha,float beta,float gamma)
{
	float ATAN = 2.0f*alpha/(gamma-beta);
	float phi = atan(ATAN)/2.0f;
	if (gamma > beta)
	{
		if (alpha < 0.0f)
			phi-=M_PI/2.0f;
		else
			phi+=M_PI/2.0f;
	}
	return phi;
}

float gauss(tPhasenraum* raum,float x,float y)
{
	float phi = calc_phi(raum->alpha,raum->beta,raum->gamma);
	float SIN = -sin(phi);
	float COS = cos(phi);
	float a=sqrt(raum->epsilon/(raum->gamma*COS*COS-2.0f*raum->alpha*COS*SIN+raum->beta*SIN*SIN));
	float b=sqrt(raum->epsilon/(raum->gamma*SIN*SIN+2.0f*raum->alpha*COS*SIN+raum->beta*COS*COS));
	float X = (x*COS/a+y*-SIN/a);
	float Y = (x*SIN/b+y*COS/b);
	return 1.0f/(2*M_PI)*exp(-0.5f*(X*X+Y*Y));
}

float distribution(tPhasenraum* raum,float x,float y)
{
	return gauss(raum,x,y)-exp(-0.5f)/(2*M_PI);
}

float erf_positive(float x)
{
	float devisor = 1.0+0.278393*x+0.230389*x*x+0.000972*x*x*x+0.078108*x*x*x*x;
	devisor *= devisor; //²
	devisor *= devisor; //²
	return 1.0f-1.0f/devisor;
}

float erf_(float x)
{
	if (x < 0.0f) 
		return -erf_positive(-x);
	return erf_positive(x);
}

float GAUSS(tPhasenraum* raum,float x)
{
	float phi = calc_phi(raum->alpha,raum->beta,raum->gamma);
	float SIN = -sin(phi);
	float COS = cos(phi);
	float A=sqrt(raum->epsilon/(raum->gamma*COS*COS-2.0f*raum->alpha*COS*SIN+raum->beta*SIN*SIN));
	float B=sqrt(raum->epsilon/(raum->gamma*SIN*SIN+2.0f*raum->alpha*COS*SIN+raum->beta*COS*COS));
	float R11 = +COS/A;
	float R12 = +SIN/B;
	float R21 = -SIN/A;
	float R22 = +COS/B;
	float a = (R11*R11+R12*R12)/2.0f;
	float b =  R12*R22+R11*R21;
	float c = (R21*R21+R22*R22)/2.0f;
	float k = sqrt(1.0f/(16.0f*M_PI*c));
	float l = b*b/(4.0f*c)-a;
	float m = (b*raum->beta-2.0f*c*raum->alpha)/(2.0f*sqrt(c)*raum->beta);
	float n = c;
	float part = sqrt(n*(raum->beta*raum->epsilon-x*x))/raum->beta;
	float result = fabs(k*exp(l*4.0f*x*x)*(erf_(m*2.0f*x+part)-erf_(m*2.0f*x-part)));
	return result;
}

void drawPhasenraumIntegral(tPhasenraum* raum,int x1,int y1,int x2,int y2)
{
	int mx = x1+x2 >> 1;
	int my = y1+y2 >> 1;
	int w = x2-x1;
	int h = y2-y1;
	int one;
	if (x2-x1 < y2-y1)
		one = (float)(x2-x1)/zoom;
	else
		one = (float)(y2-y1)/zoom;
	
	int x = (int)(sqrt(raum->epsilon*raum->beta)*(float)one)/2;
	int lx = mx-x;
	int rx = mx+x;
	if (lx < x1)
		lx = x1;
	if (rx > x2)
		rx = x2;
	float highest = GAUSS(raum,0.0f);
	for (x = lx; x < rx; x++)
	{
		float X = (float)((x-mx)*2)/(float)one;
		float I = GAUSS(raum,X)/highest;
		int r = I*255.0f;
		spEllipse(x,my,0,2,2,spGetRGB(r,r,r));
	}
}

void dicePhasenraumParticles(tPhasenraum* raum)
{
	//dicing the particles positions:
	float phi = calc_phi(raum->alpha,raum->beta,raum->gamma);
	float SIN = -sin(phi);
	float COS = cos(phi);
	float a=sqrt(raum->epsilon/(raum->gamma*COS*COS-2.0f*raum->alpha*COS*SIN+raum->beta*SIN*SIN));
	float b=sqrt(raum->epsilon/(raum->gamma*SIN*SIN+2.0f*raum->alpha*COS*SIN+raum->beta*COS*COS));
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
		raum->particle[0][i] = raum->start_particle[0][i] = x*COS+y*SIN;
		raum->particle[1][i] = raum->start_particle[1][i] = x*-SIN+y*COS;
	}	
}

void initPhasenraum(tPhasenraum* raum,float start_alpha,float start_beta,float epsilon,Uint16 color,int dice)
{
	raum->epsilon = epsilon;
	raum->start_alpha = start_alpha;
	raum->start_beta = start_beta;
	raum->start_gamma = calc_gamma(raum->start_alpha,raum->start_beta);
	raum->alpha = raum->start_alpha;
	raum->beta = raum->start_beta;
	raum->gamma = raum->start_gamma;
	raum->color = color;
	if (dice)
		dicePhasenraumParticles(raum);
}

void printPhasenraum(tPhasenraum* raum)
{
	printf("function:\n");
	printf("(%f*x*x+2.0*%f*x*y+%f*y*y-%f) == 0\n",raum->gamma,raum->alpha,raum->beta,raum->epsilon);
	printf("particles:\n");
	printf("X\tY\n");
	int i;
	for (i = 0; i < PARTICLE_COUNT; i++)
	{
		printf("%f\t%f\n",raum->particle[0][i],raum->particle[1][i]);
	}
}

void drawPhasenraumEllipse(tPhasenraum* raum,int x1,int y1,int x2,int y2)
{
	spSetAlphaTest(0);
	int one;
	if (x2-x1 < y2-y1)
		one = (float)(x2-x1)/zoom;
	else
		one = (float)(y2-y1)/zoom;
	int mx = x1+x2 >> 1;
	int my = y1+y2 >> 1;
	int x,y,w,h;
	w = (x2-x1)/RASTER_X;
	h = (y2-y1)/RASTER_Y;
	if (draw_field)
		for (x = 0; x <= RASTER_X; x++)
			for (y = 0; y <= RASTER_Y; y++)
			{
				int X = x*(x2-x1)/RASTER_X;
				int Y = y*(y2-y1)/RASTER_Y;
				Uint16 color;
				if (raum->marching_points[x][y] > 0.0f)
				{
					float value = 1.0f - 1.0f / ( 1.0f + raum->marching_points[x][y] );
					color = spGetHSV((int)((float)SP_PI-(float)SP_PI*value),255,255);
				}
				else
				{
					float value = 1.0f - 1.0f / ( 1.0f + -raum->marching_points[x][y] );
					color = spGetHSV((int)((float)SP_PI+(float)SP_PI*value),255,255);
				}
				spRectangle(x1+X,y1+Y,0,w,h,color);
			}
	line(x1,my,0,x2,my,0,raum->color);
	line(mx,y1,0,mx,y2,0,raum->color);
	//Marching squares
	for (x = 0; x < RASTER_X; x++)
	{
		int X_1 = x1+x   *(x2-x1)/RASTER_X;
		int X_2 = x1+(x+1)*(x2-x1)/RASTER_X;
		for (y = 0; y < RASTER_Y; y++)
		{
			int Y_1 = y1+y   *(y2-y1)/RASTER_Y;
			int Y_2 = y1+(y+1)*(y2-y1)/RASTER_Y;
			drawMarchingLine(X_1,Y_1,raum->marching_points[x  ][y  ],
			                 X_2,Y_1,raum->marching_points[x+1][y  ],
			                 X_2,Y_2,raum->marching_points[x+1][y+1],
							 X_1,Y_2,raum->marching_points[x  ][y+1],raum->color);
		}
	}
}

void drawPhasenraumParticles(tPhasenraum* raum,int x1,int y1,int x2,int y2)
{
	if (draw_field)
		return;
	int one;
	if (x2-x1 < y2-y1)
		one = (float)(x2-x1)/zoom;
	else
		one = (float)(y2-y1)/zoom;
	int i;
	int mx = x1+x2 >> 1;
	int my = y1+y2 >> 1;
	for (i = 0; i < PARTICLE_COUNT; i+=PARTICLE_NEXT_DRAW)
	{
		int x = (int)(raum->particle[0][i]*one/2.0f) + mx;
		int y = (int)(raum->particle[1][i]*one/2.0f) + my;
		if (x > x1 && x < x2 && y > y1 && y < y2)
			spEllipse(x,y,0,2,2,raum->color);
	}
}

void drawPhasenraumInformation(tPhasenraum* raum,int x1,int y1,int x2,int y2)
{
	int mx = x1+x2 >> 1;
	int my = y1+y2 >> 1;
	int w = x2-x1;
	int h = y2-y1;
	float phi = calc_phi(raum->alpha,raum->beta,raum->gamma);
	float size_factor = spFixedToFloat(spGetSizeFactor());
	spSetAlphaTest(1);
	char buffer[256];
	
	line(mx-(int)(cos(phi)*size_factor*30.0f),my-(int)(sin(phi)*size_factor*30.0f),0,
	     mx+(int)(cos(phi)*size_factor*30.0f),my+(int)(sin(phi)*size_factor*30.0f),0,raum->color);
	line(mx-(int)(cos(phi+M_PI/2.0f)*size_factor*30.0f),my-(int)(sin(phi+M_PI/2.0f)*size_factor*30.0f),0,
	     mx+(int)(cos(phi+M_PI/2.0f)*size_factor*30.0f),my+(int)(sin(phi+M_PI/2.0f)*size_factor*30.0f),0,raum->color);

	int one;
	if (x2-x1 < y2-y1)
		one = (float)(x2-x1)/zoom;
	else
		one = (float)(y2-y1)/zoom;

	int x = (int)(-raum->alpha*sqrt(raum->epsilon/raum->gamma)*(float)one)/2;
	int y = (int)(sqrt(raum->epsilon*raum->gamma)*(float)one)/2;
	line( x1+w*19/40+x, y1+h/2+y, 0,
	      x1+w*21/40+x, y1+h/2+y, 0,raum->color);
	line( x1+w*19/40-x, y1+h/2-y, 0,
	      x1+w*21/40-x, y1+h/2-y, 0,raum->color);

	x = (int)(sqrt(raum->epsilon*raum->beta)*(float)one)/2;
	y = (int)(-raum->alpha*sqrt(raum->epsilon/raum->beta)*(float)one)/2;
	line( x1+w/2+x, y1+h*19/40+y, 0,
	      x1+w/2+x, y1+h*21/40+y, 0,raum->color);
	line( x1+w/2-x, y1+h*19/40-y, 0,
	      x1+w/2-x, y1+h*21/40-y, 0,raum->color);

	y = (int)(sqrt(raum->epsilon/raum->beta)*(float)one)/2;
	line(x1+w*19/40,my-y,0,
	     x1+w*21/40,my-y,0,raum->color); 
	sprintf(buffer,"√(ε/β)=%.3f",sqrt(raum->epsilon/raum->beta));
	spFontDrawMiddle(mx,my-y-font->maxheight,0,buffer,font);

	x = (int)(sqrt(raum->epsilon/raum->gamma)*(float)one)/2;
	line(mx+x,y1+h*19/40,0,
	     mx+x,y1+h*21/40,0,raum->color);
	sprintf(buffer," √(ε/γ)=%.3f",sqrt(raum->epsilon/raum->gamma));
	spFontDraw(mx+x,my-font->maxheight/2,0,buffer,font);
	
	int next_line = font->maxheight*3/4;
	
	int tx = x1;
	int ty = y1;
	int (*draw_text) ( Sint32 x, Sint32 y, Sint32 z, const char* text, spFontPointer font ) = spFontDraw;
	if (x1 > 0)
	{
		tx = x2-1;
		draw_text = spFontDrawRight;
	}
	if (y1 > 0)
		ty = y2-next_line*7-font->maxheight;

	sprintf(buffer,"φ=%.3f°",phi*180.0f/M_PI);
	draw_text(tx,ty            ,0,buffer,font);
	sprintf(buffer,"α=%.3f",raum->alpha);
	draw_text(tx,ty+next_line*1,0,buffer,font);
	sprintf(buffer,"β=%.3f",raum->beta);
	draw_text(tx,ty+next_line*2,0,buffer,font);
	sprintf(buffer,"γ=%.3f",raum->gamma);
	draw_text(tx,ty+next_line*3,0,buffer,font);
	sprintf(buffer,"ε=%.3f",raum->epsilon);
	draw_text(tx,ty+next_line*4,0,buffer,font);

	float SIN = sin(phi);
	float COS = cos(phi);
	float a=sqrt(raum->epsilon/(raum->gamma*COS*COS-2.0f*raum->alpha*COS*SIN+raum->beta*SIN*SIN));
	float b=sqrt(raum->epsilon/(raum->gamma*SIN*SIN+2.0f*raum->alpha*COS*SIN+raum->beta*COS*COS));
	sprintf(buffer,"a=%.3f",a);
	draw_text(tx,ty+next_line*5,-1,buffer,font);
	sprintf(buffer,"b=%.3f",b);
	draw_text(tx,ty+next_line*6,-1,buffer,font);
	float s_w = raum->start_alpha / raum->start_gamma;
	sprintf(buffer,"sᵥᵥ=%.3f",s_w);
	draw_text(tx,ty+next_line*7,-1,buffer,font);
}

void multiplyMatrixPhasenraum(tPhasenraum* raum,float a,float b,float c,float d)
{
	raum->beta = a*a*raum->start_beta
		       - 2.0f*a*b*raum->start_alpha
		       + b*b*raum->start_gamma;
	raum->alpha = -a*c*raum->start_beta
		        + (a*d+b*c)*raum->start_alpha
		        - b*d*raum->start_gamma;
	raum->gamma = c*c*raum->start_beta
		        - 2.0f*c*d*raum->start_alpha
		        + d*d*raum->start_gamma;
	#ifdef NO_BLAS
		int i;
		for (i = 0; i < PARTICLE_COUNT; i++)
		{
			raum->particle[0][i] = a*raum->start_particle[0][i]
						         + b*raum->start_particle[1][i];
			raum->particle[1][i] = c*raum->start_particle[0][i]
			                     + d*raum->start_particle[1][i];
		}
	#else
		float matrix[2][2] = {{a,b},{c,d}};
		cblas_sgemm( CblasRowMajor, CblasNoTrans, CblasNoTrans, 2, PARTICLE_COUNT, 2,
					 1.0f, (float*)matrix, 2, (float*)raum->start_particle, PARTICLE_COUNT, 0.0f, (float*)raum->particle, PARTICLE_COUNT);
	#endif	
}

void calcPhasenraum(tPhasenraum* raum,int x1,int y1,int x2,int y2,int steps)
{
	int w = x2-x1;
	int h = y2-y1;
	float ONE_STEP = spFixedToFloat(SP_ONE>>one_step);
	if (!pause)
		s += (float)steps*ONE_STEP;
	//Drift:
	multiplyMatrixPhasenraum(raum,1.0f,s,0.0f,1.0f);
	//update the marching points
	int x,y;
	float square_size;
	if (w < h)
	{
		square_size = (1.0f / (float)RASTER_X * zoom * 2.0f) * (1.0f / (float)RASTER_Y * zoom * 2.0f * (float)(h)/(float)(w));
		for (x = 0; x <= RASTER_X; x++)
		{
			float f_x = ((float)x / (float)RASTER_X - 0.5f) * zoom * 2.0f;
			for (y = 0; y <= RASTER_Y; y++)
			{
				float f_y = ((float)y / (float)RASTER_Y - 0.5f) * zoom * 2.0f * (float)(h)/(float)(w);
				raum->marching_points[x][y] = distribution(raum,f_x,f_y);
			}
		}
	}
	else
	{
		square_size = (1.0f / (float)RASTER_Y * zoom * 2.0f) * (1.0f / (float)RASTER_X * zoom * 2.0f * (float)(w)/(float)(h));
		for (x = 0; x <= RASTER_X; x++)
		{
			float f_x = ((float)x / (float)RASTER_X - 0.5f) * zoom * 2.0f * (float)(w)/(float)(h);
			for (y = 0; y <= RASTER_Y; y++)
			{
				float f_y = ((float)y / (float)RASTER_Y - 0.5f) * zoom * 2.0f;
				raum->marching_points[x][y] = distribution(raum,f_x,f_y);
			}
		}
	}
}

void drawPhasenraumAll(tPhasenraum* raum,int x1,int y1,int x2,int y2)
{
	if (!draw_field)
		spRectangle(x1+x2>>1,y1+y2>>1,0,x2-x1,y2-y1,raum->color & 0b0011000110000110);
	drawPhasenraumEllipse    (raum,x1,y1,x2,y2);
	drawPhasenraumParticles  (raum,x1,y1,x2,y2);
	drawPhasenraumInformation(raum,x1,y1,x2,y2);
	drawPhasenraumIntegral   (raum,x1,y1,x2,y2);
}

void resetPhasenraumDrift(tPhasenraum* raum)
{
	raum->start_alpha = raum->alpha;
	raum->start_beta = raum->beta;
	raum->start_gamma = raum->gamma;
	memcpy(raum->start_particle,raum->particle,sizeof(float)*2*PARTICLE_COUNT);	
}
