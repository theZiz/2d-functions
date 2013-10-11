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
} tPhasenraum;

void dicePhasenraumParticles(tPhasenraum* raum)
{
	//dicing the particles positions:
	float ATAN = 2.0f*raum->alpha/(raum->gamma-raum->beta);
	float phi = atan(ATAN)/2.0f;
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

void initPhasenraum(tPhasenraum* raum,float start_alpha,float start_beta,float epsilon)
{
	raum->epsilon = epsilon;
	raum->start_alpha = start_alpha;
	raum->start_beta = start_beta;
	raum->start_gamma = calc_gamma(raum->start_alpha,raum->start_beta);
	raum->alpha = raum->start_alpha;
	raum->beta = raum->start_beta;
	raum->gamma = raum->start_gamma;
	dicePhasenraumParticles(raum);
}

void loadPhasenraum(tPhasenraum* raum)
{
	//ToDo: Load from file!
	float A = -0.0175f;
	float B = 4.0f;
	float C = 0.0001;
	float epsilon = sqrt(C*B-A*A);
	initPhasenraum(raum,A/(-epsilon),B/epsilon,epsilon);
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
	line(x1,my,0,x2,my,0);
	line(mx,y1,0,mx,y2,0);
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
							 X_1,Y_2,raum->marching_points[x  ][y+1],65535);
		}
	}
}

void drawPhasenraumParticles(tPhasenraum* raum,int x1,int y1,int x2,int y2)
{
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
			spEllipse(x,y,0,2,2,spGetRGB(0,255,127));
	}
}

void drawPhasenraumInformation(tPhasenraum* raum,int x1,int y1,int x2,int y2)
{
	int mx = x1+x2 >> 1;
	int my = y1+y2 >> 1;
	int w = x2-x1;
	int h = y2-y1;
	float ATAN = 2.0f*raum->alpha/(raum->gamma-raum->beta);
	float phi = atan(ATAN)/2.0f;
	float size_factor = spFixedToFloat(spGetSizeFactor());
	spSetAlphaTest(1);
	char buffer[256];
	
	line(mx-(int)(cos(phi)*size_factor*30.0f),my-(int)(sin(phi)*size_factor*30.0f),0,
	     mx+(int)(cos(phi)*size_factor*30.0f),my+(int)(sin(phi)*size_factor*30.0f),0);
	line(mx-(int)(cos(phi+M_PI/2.0f)*size_factor*30.0f),my-(int)(sin(phi+M_PI/2.0f)*size_factor*30.0f),0,
	     mx+(int)(cos(phi+M_PI/2.0f)*size_factor*30.0f),my+(int)(sin(phi+M_PI/2.0f)*size_factor*30.0f),0);

	int one;
	if (x2-x1 < y2-y1)
		one = (float)(x2-x1)/zoom;
	else
		one = (float)(y2-y1)/zoom;

	int x = (int)(-raum->alpha*sqrt(raum->epsilon/raum->gamma)*(float)one)/2;
	int y = (int)(sqrt(raum->epsilon*raum->gamma)*(float)one)/2;
	line( x1+w*19/40+x, y1+h/2+y, 0,
	      x1+w*21/40+x, y1+h/2+y, 0);
	line( x1+w*19/40-x, y1+h/2-y, 0,
	      x1+w*21/40-x, y1+h/2-y, 0);

	x = (int)(sqrt(raum->epsilon*raum->beta)*(float)one)/2;
	y = (int)(-raum->alpha*sqrt(raum->epsilon/raum->beta)*(float)one)/2;
	line( x1+w/2+x, y1+h*19/40+y, 0,
	      x1+w/2+x, y1+h*21/40+y, 0);
	line( x1+w/2-x, y1+h*19/40-y, 0,
	      x1+w/2-x, y1+h*21/40-y, 0);

	y = (int)(sqrt(raum->epsilon/raum->beta)*(float)one)/2;
	line(x1+w*19/40,my-y,0,
	     x1+w*21/40,my-y,0); 
	sprintf(buffer,"√(ε/β)=%.3f",sqrt(raum->epsilon/raum->beta));
	spFontDrawMiddle(mx,my-y-font->maxheight,0,buffer,font);

	x = (int)(sqrt(raum->epsilon/raum->gamma)*(float)one)/2;
	line(mx+x,y1+h*19/40,0,
	     mx+x,y1+h*21/40,0);
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

void calcPhasenraum(tPhasenraum* raum,int x1,int y1,int x2,int y2,int steps)
{
	int w = x2-x1;
	int h = y2-y1;
	float ONE_STEP = 1.0f/(float)one_step;
	if (!pause)
		s += (float)steps*ONE_STEP;
	raum->beta = Cf(s)*Cf(s)*raum->start_beta
		       + -2.0f*Sf(s)*Cf(s)*raum->start_alpha
		       + Sf(s)*Sf(s)*raum->start_gamma;
	raum->alpha = -Cf(s)*Cd(s)*raum->start_beta
		        + (Sd(s)*Cf(s)+Sf(s)*Cd(s))*raum->start_alpha
		        + -Sf(s)*Sd(s)*raum->start_gamma;
	raum->gamma = Cd(s)*Cd(s)*raum->start_beta
		        + -2.0f*Sd(s)*Cd(s)*raum->start_alpha
		        + Sd(s)*Sd(s)*raum->start_gamma;
	#ifdef NO_BLAS
		int i;
		for (i = 0; i < PARTICLE_COUNT; i++)
		{
			raum->particle[0][i] = Cf(s)*raum->start_particle[0][i]
						         + Sf(s)*raum->start_particle[1][i];
			raum->particle[1][i] = Cd(s)*raum->start_particle[0][i]
						         + Sd(s)*raum->start_particle[1][i];
		}
	#else
		float matrix[2][2] = {{Cf(s),Sf(s)},{Cd(s),Sd(s)}};
		cblas_sgemm( CblasRowMajor, CblasNoTrans, CblasNoTrans, 2, PARTICLE_COUNT, 2,
					 1.0f, (float*)matrix, 2, (float*)raum->start_particle, PARTICLE_COUNT, 0.0f, (float*)raum->particle, PARTICLE_COUNT);
	#endif
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
				raum->marching_points[x][y] = function(f_x,f_y);
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
				raum->marching_points[x][y] = function(f_x,f_y);
			}
		}
	}
}

void drawPhasenraumAll(tPhasenraum* raum,int x1,int y1,int x2,int y2)
{
	drawPhasenraumEllipse    (raum,x1,y1,x2,y2);
	drawPhasenraumParticles  (raum,x1,y1,x2,y2);
	drawPhasenraumInformation(raum,x1,y1,x2,y2);
}