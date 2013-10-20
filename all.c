#include "matrix.c"

void fill_all_from_matrix(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,float* matrix,Uint16 color1,Uint16 color2,Uint16 color3,int dice)
{
	//matrix[x*6+y]
	float A = matrix[0*6+1];
	float B = matrix[0*6+0];
	float C = matrix[1*6+1];
	float epsilon = sqrt(C*B-A*A);
	initPhasenraum(X,A/(-epsilon),B/epsilon,epsilon,color1,dice);
	A = matrix[2*6+3];
	B = matrix[2*6+2];
	C = matrix[3*6+3];
	epsilon = sqrt(C*B-A*A);
	initPhasenraum(Y,A/(-epsilon),B/epsilon,epsilon,color2,dice);
	A = matrix[4*6+5];
	B = matrix[4*6+4];
	C = matrix[5*6+5];
	epsilon = sqrt(C*B-A*A);
	initPhasenraum(Z,A/(-epsilon),B/epsilon,epsilon,color3,dice);
}

void loadAll(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,Uint16 color1,Uint16 color2,Uint16 color3)
{
	//ToDo: Load from file!
	float matrix[6][6];
	memset(matrix,0,sizeof(float)*36);
	matrix[0][0] = 4.0f;
	matrix[0][1] = matrix[1][0] = -0.0175f;
	matrix[1][1] = 0.0001f;
	matrix[2][2] = 4.0f;
	matrix[2][3] = matrix[3][2] = -0.0175f;
	matrix[3][3] = 0.0f;
	matrix[4][4] = 2500.0f;
	matrix[5][4] = matrix[5][4] = -0.0175f;
	matrix[5][5] = 1.6e-7;
	spFilePointer f =  SDL_RWFromFile("./Default.matrix","rb");
	if (f)
	{
		int i;
		for (i = 0; i < 6; i++)
		{
			char buffer[256];
			char* line = buffer;
			spReadOneLine(f,line,255);
			int j;
			for (j = 0; j < 6; j++)
			{
				matrix[j][i] = atof(line);
				char* oldline = line;
				line = strchr(oldline,' ');
				line = &(line[1]);
				if (line == NULL)
				{
					line = &(line[1]);
					line = strchr(oldline,'\n');
					if (line == NULL)
						break;
				}
			}
		}
		SDL_FreeRW(f);
	}
	fill_all_from_matrix(X,Y,Z,(float*)matrix,color1,color2,color3,1);
}

void drawAllEllipse(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2)
{
	spBlitSurface(x1+x2>>1,y1+y2>>1,0,threeD);
}

void fill_matrix_solenoid(float *matrix,float I)
{
	float f = 344.6f / (I * I);

	//matrix[y+x*6]
	memset(matrix,0,sizeof(float)*36);
	matrix[0] = 1.0f;
	matrix[7] = 1.0f;
	matrix[14] = 1.0f;
	matrix[21] = 1.0f;
	matrix[28] = 1.0f;
	matrix[35] = 1.0f;
	
	matrix[0+1*6] = matrix[2+3*6] = -1.0f / f;
}

void fill_matrix_current(float *matrix,tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z)
{
	//matrix[y+x*6]
	memset(matrix,0,sizeof(float)*36);
	matrix[0+1*6] = matrix[1+0*6] = -X->epsilon*X->alpha;
	matrix[0+0*6] = X->epsilon*X->beta;
	matrix[1+1*6] = X->epsilon*X->gamma;
	matrix[3+2*6] = matrix[2+3*6] = -Y->epsilon*Y->alpha;
	matrix[2+2*6] = Y->epsilon*Y->beta;
	matrix[3+3*6] = Y->epsilon*Y->gamma;
	matrix[4+5*6] = matrix[5+4*6] = -Z->epsilon*Z->alpha;
	matrix[4+4*6] = Z->epsilon*Z->beta;
	matrix[5+5*6] = Z->epsilon*Z->gamma;
}

void drawAllInformation(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2)
{
	int w = x2-x1;
	int h = y2-y1;
	char buffer[256];
	float matrix[6][6];
	fill_matrix_current((float*)matrix,X,Y,Z);

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

void mul_particles(tPhasenraum* raum,float mul)
{
	int i;
		for (i = 0; i < PARTICLE_COUNT; i++)
			raum->particle[1][i] *= mul;
}

void calculate_matrix(float* new_matrix,float* old_matrix,float* change_matrix)
{
	//temp[y+x*6]
	float temp[36];
	//temp = change*old
	mul_matrix(change_matrix,old_matrix,temp);	
	//new = temp*change.transpose;
	mul_matrix_trans(temp,change_matrix,new_matrix);	
	int x,y;
	printf("Change:\n");
	for (y = 0; y < 6; y++)
	{
		printf("|");
		for (x = 0; x < 6; x++)
			printf(" %06.3f |",change_matrix[x*6+y]);
		printf("\n");
	}
	printf("\n");
	printf("Old:\n");
	for (y = 0; y < 6; y++)
	{
		printf("|");
		for (x = 0; x < 6; x++)
			printf(" %06.3f |",old_matrix[x*6+y]);
		printf("\n");
	}
	printf("\n");
	printf("Temp:\n");
	for (y = 0; y < 6; y++)
	{
		printf("|");
		for (x = 0; x < 6; x++)
			printf(" %06.3f |",temp[x*6+y]);
		printf("\n");
	}
	printf("\n");
	printf("New:\n");
	for (y = 0; y < 6; y++)
	{
		printf("|");
		for (x = 0; x < 6; x++)
			printf(" %06.3f |",new_matrix[x*6+y]);
		printf("\n");
	}
	printf("\n");
}

void all_new_matrix(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z)
{
	//Getting the change matrix:
	float change_matrix[6][6];
	fill_matrix_solenoid((float*)change_matrix,1.05f);
	//Getting the old matrix
	float old_matrix[6][6];
	fill_matrix_current((float*)old_matrix,X,Y,Z);
	//Calculating the new matrix:
	float new_matrix[6][6];
	calculate_matrix((float*)new_matrix,(float*)old_matrix,(float*)change_matrix);
	float X_old_high = sqrt(X->epsilon/X->beta);
	float Y_old_high = sqrt(Y->epsilon/Y->beta);
	float Z_old_high = sqrt(Z->epsilon/Z->beta);
	fill_all_from_matrix(X,Y,Z,(float*)new_matrix,X->color,Y->color,Z->color,0);
	float X_new_high = sqrt(X->epsilon/X->beta);
	float Y_new_high = sqrt(Y->epsilon/Y->beta);
	float Z_new_high = sqrt(Z->epsilon/Z->beta);
	mul_particles(X,X_new_high/X_old_high);
	mul_particles(Y,Y_new_high/Y_old_high);
	mul_particles(Z,Z_new_high/Z_old_high);
	resetPhasenraum(X);
	resetPhasenraum(Y);
	resetPhasenraum(Z);
	s = 0.0f;
}
