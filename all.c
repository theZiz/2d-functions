void drawAllEllipse(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2)
{
	spBlitSurface(x1+x2>>1,y1+y2>>1,0,threeD);
}

void drawAllInformation(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2)
{
	int w = x2-x1;
	int h = y2-y1;
	char buffer[256];
	float matrix[6][6];
	memset(matrix,0,sizeof(float)*36);
	matrix[1][0] = matrix[0][1] = -X->epsilon*X->alpha;
	matrix[0][0] = X->epsilon*X->beta;
	matrix[1][1] = X->epsilon*X->gamma;
	matrix[3][2] = matrix[2][3] = -Y->epsilon*Y->alpha;
	matrix[2][2] = Y->epsilon*X->beta;
	matrix[3][3] = Y->epsilon*X->gamma;
	matrix[5][4] = matrix[4][5] = -Z->epsilon*Z->alpha;
	matrix[4][4] = Z->epsilon*Z->beta;
	matrix[5][5] = Z->epsilon*Z->gamma;

	int tx = x1;
	int ty = y1;
	int size_x = w/32;
	int size_y = h/18;
	if (x1 > 0)
		tx = x2-size_x*14;
	if (y1 > 0)
		ty = y2-size_y*8;
	int x,y;

	int i;
	for (i = 0; i < 3; i++)
	{
		int add = i*size_y*2;
		switch(i)
		{
			case 0:	spFontDrawRight(tx+6*size_x,add+ty+2*size_y+font->maxheight/2,0,"σ₁=",font); break;
			case 1:	spFontDrawRight(tx+6*size_x,add+ty+2*size_y+font->maxheight/2,0,"σ₂=",font); break;
			case 2:	spFontDrawRight(tx+6*size_x,add+ty+2*size_y+font->maxheight/2,0,"σ₃=",font); break;
		}
		spRotozoomSurface(tx+6*size_x,add+ty+3*size_y,0,spFontGetLetter(font,'(')->surface,spFloatToFixed(1.0f),spFloatToFixed(2.0f),0);
		for (x = 0; x < 2; x++)
			for (y = 0; y < 2; y++)
			{
				sprintf(buffer,"%.2f",matrix[x+i*2][y+i*2]);
				spFontDrawMiddle(tx+(x+2)*size_x*4,add+ty+(y+2)*size_y,0,buffer,font);
			}
		spRotozoomSurface(tx+14*size_x,add+ty+3*size_y,0,spFontGetLetter(font,')')->surface,spFloatToFixed(1.0f),spFloatToFixed(2.0f),0);
	}
}

void drawAllAll(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2)
{
	drawAllEllipse(X,Y,Z,x1,y1,x2,y2);
	drawAllInformation(X,Y,Z,x1,y1,x2,y2);
}

void updateThreeD(int w,int h)
{
	if (threeD)
		spDeleteSurface(threeD);
	threeD = spCreateSurface(w,h);
	spSetPerspective( 45.0, ( float )w / ( float )h, 1.0f, 10000.0f );
}

void calcAll(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2,int steps)
{
	spSelectRenderTarget(threeD);
	spSetZSet(1);
	spSetZTest(1);
	spResetZBuffer();
	spClearTarget( 0b0011000110000110 );
	spIdentity();
	float allZoom = zoom;
	if (test_values == 0)
	{
		allZoom*=256.0f;
	}
	spTranslate(0,0,spFloatToFixed(-4.0f)*allZoom);
	spMulMatrix(rotation);
	Sint32 size = spFloatToFixed(1.0f / ( 1.0f + allZoom ) +1.0f)/32;
	int i;
	for (i = 0; i < PARTICLE_COUNT; i+=PARTICLE_NEXT_DRAW)
	{
		float length = sqrt(X->particle[1][i]*X->particle[1][i]+
		                    Y->particle[1][i]*Y->particle[1][i]+
		                    Z->particle[1][i]*Z->particle[1][i]);
		if (test_values == 0)
			length*=256.0f;
		float value = 1.0f / ( 1.0f + length );
		Sint32 x = spFloatToFixed(X->particle[0][i]);
		Sint32 y = spFloatToFixed(Y->particle[0][i]);
		Sint32 z = spFloatToFixed(Z->particle[0][i]);
		Uint16 color = spGetHSV((int)((float)SP_PI*(value*2.0f/3.0f)),255,255);
		spEllipse3D(x,y,z,size,size, color);
	}
	spSetBlending(SP_ONE/2);
	spSetLineWidth(4);
	spLine3D(-SP_ONE*zoom,0,0,SP_ONE*allZoom,0,0,X->color);
	spLine3D(0,-SP_ONE*zoom,0,0,SP_ONE*allZoom,0,Y->color);
	spLine3D(0,0,-SP_ONE*zoom,0,0,SP_ONE*allZoom,Z->color);
	spSetBlending(SP_ONE);
	spSetLineWidth(1);
	spSetZSet(0);
	spSetZTest(0);
	spSelectRenderTarget(screen);
}
