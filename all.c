void fill_all_from_matrix(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,pMatrix matrix,Uint16 color1,Uint16 color2,Uint16 color3,int dice)
{
	float A = (*matrix)[0][1];
	float B = (*matrix)[0][0];
	float C = (*matrix)[1][1];
	float epsilon = sqrt(C*B-A*A);
	initPhasenraum(X,A/(-epsilon),B/epsilon,epsilon,color1,dice);
	A = (*matrix)[2][3];
	B = (*matrix)[2][2];
	C = (*matrix)[3][3];
	epsilon = sqrt(C*B-A*A);
	initPhasenraum(Y,A/(-epsilon),B/epsilon,epsilon,color2,dice);
	A = (*matrix)[4][5];
	B = (*matrix)[4][4];
	C = (*matrix)[5][5];
	epsilon = sqrt(C*B-A*A);
	initPhasenraum(Z,A/(-epsilon),B/epsilon,epsilon,color3,dice);
}

void loadAll(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,Uint16 color1,Uint16 color2,Uint16 color3)
{
	//ToDo: Load from file!
	tMatrix matrix;
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
	fill_all_from_matrix(X,Y,Z,&matrix,color1,color2,color3,1);
}

void drawAllEllipse(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2)
{
	spBlitSurface(x1+x2>>1,y1+y2>>1,0,threeD);
}

void fill_matrix_solenoid(pMatrix matrix,float I)
{
	float f = 344.6f / (I * I);

	//matrix[y+x*6]
	memset((*matrix),0,sizeof(float)*36);
	(*matrix)[0][0] = 1.0f;
	(*matrix)[1][1] = 1.0f;
	(*matrix)[2][2] = 1.0f;
	(*matrix)[3][3] = 1.0f;
	(*matrix)[4][4] = 1.0f;
	(*matrix)[5][5] = 1.0f;
	
	(*matrix)[1][0] = (*matrix)[3][2] = -1.0f / f;
}

void fill_matrix_current(pMatrix matrix,tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z)
{
	//matrix[y+x*6]
	memset((*matrix),0,sizeof(float)*36);
	(*matrix)[1][0] = (*matrix)[0][1] = -X->epsilon*X->alpha;
	(*matrix)[0][0] = X->epsilon*X->beta;
	(*matrix)[1][1] = X->epsilon*X->gamma;
	(*matrix)[2][3] = (*matrix)[3][2] = -Y->epsilon*Y->alpha;
	(*matrix)[2][2] = Y->epsilon*Y->beta;
	(*matrix)[3][3] = Y->epsilon*Y->gamma;
	(*matrix)[5][4] = (*matrix)[4][5] = -Z->epsilon*Z->alpha;
	(*matrix)[4][4] = Z->epsilon*Z->beta;
	(*matrix)[5][5] = Z->epsilon*Z->gamma;
}

void drawAllInformation(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2)
{
	int w = x2-x1;
	int h = y2-y1;
	char buffer[256];
	tMatrix matrix;
	fill_matrix_current(&matrix,X,Y,Z);

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

void calculate_matrix(pMatrix new_matrix,pMatrix old_matrix,pMatrix change_matrix)
{
	//temp[y+x*6]
	tMatrix temp;
	//temp = change*old
	mul_matrix(change_matrix,old_matrix,&temp);	
	//new = temp*change.transpose;
	mul_matrix_trans(&temp,change_matrix,new_matrix);	

	int x,y;
	printf("Change:\n");
	for (y = 0; y < 6; y++)
	{
			printf("|");
			for (x = 0; x < 6; x++)
					printf(" %06.3f |",(*change_matrix)[x][y]);
			printf("\n");
	}
	printf("\n");
	printf("Old:\n");
	for (y = 0; y < 6; y++)
	{
			printf("|");
			for (x = 0; x < 6; x++)
					printf(" %06.3f |",(*old_matrix)[x][y]);
			printf("\n");
	}
	printf("\n");
	printf("Temp:\n");
	for (y = 0; y < 6; y++)
	{
			printf("|");
			for (x = 0; x < 6; x++)
					printf(" %06.3f |",temp[x][y]);
			printf("\n");
	}
	printf("\n");
	printf("New:\n");
	for (y = 0; y < 6; y++)
	{
			printf("|");
			for (x = 0; x < 6; x++)
					printf(" %06.3f |",(*new_matrix)[x][y]);
			printf("\n");
	}
	printf("\n");
}

void rotate_points_to_zero(tPhasenraum* raum)
{
	float phi = calc_phi(raum->alpha,raum->beta,raum->gamma);
	float SIN = -sin(-phi);
	float COS = cos(-phi);
	int i;
	for (i = 0; i < PARTICLE_COUNT; i++)
	{
		float x = raum->particle[0][i]* COS+raum->particle[1][i]*SIN;
		float y = raum->particle[0][i]*-SIN+raum->particle[1][i]*COS;
		raum->particle[0][i] = x;
		raum->particle[1][i] = y;
	}		
}

void rotate_points_to_angle(tPhasenraum* raum)
{
	float phi = calc_phi(raum->alpha,raum->beta,raum->gamma);
	float SIN = -sin(phi);
	float COS = cos(phi);
	int i;
	for (i = 0; i < PARTICLE_COUNT; i++)
	{
		float x = raum->particle[0][i]* COS+raum->particle[1][i]*SIN;
		float y = raum->particle[0][i]*-SIN+raum->particle[1][i]*COS;
		raum->particle[0][i] = x;
		raum->particle[1][i] = y;
	}		
}

void resize_points(tPhasenraum* raum,float old_a,float old_b)
{
	float phi = calc_phi(raum->alpha,raum->beta,raum->gamma);
	float SIN = -sin(phi); float COS = cos(phi);
	float a=sqrt(raum->epsilon/(raum->gamma*COS*COS-2.0f*raum->alpha*COS*SIN+raum->beta*SIN*SIN));
	float b=sqrt(raum->epsilon/(raum->gamma*SIN*SIN+2.0f*raum->alpha*COS*SIN+raum->beta*COS*COS));
	int i;
	for (i = 0; i < PARTICLE_COUNT; i++)
	{
		raum->particle[0][i] *= a/old_a;
		raum->particle[1][i] *= b/old_b;
	}		
}

void all_new_matrix(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z)
{
	//Getting the change matrix:
	tMatrix change_matrix;
	if (test_values == 0)
		fill_matrix_solenoid(&change_matrix,1.05f);
	else
		fill_matrix_solenoid(&change_matrix,10.5f);
	//Getting the old matrix
	tMatrix old_matrix;
	fill_matrix_current(&old_matrix,X,Y,Z);
	//Calculating the new matrix:
	tMatrix new_matrix;
	calculate_matrix(&new_matrix,&old_matrix,&change_matrix);
	
	printf("%f --- %f\n",old_matrix[0][1],new_matrix[0][1]);
        	
	float phi = calc_phi(X->alpha,X->beta,X->gamma);
	float SIN = -sin(phi); float COS = cos(phi);
	float X_a=sqrt(X->epsilon/(X->gamma*COS*COS-2.0f*X->alpha*COS*SIN+X->beta*SIN*SIN));
	float X_b=sqrt(X->epsilon/(X->gamma*SIN*SIN+2.0f*X->alpha*COS*SIN+X->beta*COS*COS));
	phi = calc_phi(Y->alpha,Y->beta,Y->gamma);
	SIN = -sin(phi); COS = cos(phi);
	float Y_a=sqrt(Y->epsilon/(Y->gamma*COS*COS-2.0f*Y->alpha*COS*SIN+Y->beta*SIN*SIN));
	float Y_b=sqrt(Y->epsilon/(Y->gamma*SIN*SIN+2.0f*Y->alpha*COS*SIN+Y->beta*COS*COS));
	phi = calc_phi(Z->alpha,Z->beta,Z->gamma);
	SIN = -sin(phi); COS = cos(phi);
	float Z_a=sqrt(Z->epsilon/(Z->gamma*COS*COS-2.0f*Z->alpha*COS*SIN+Z->beta*SIN*SIN));
	float Z_b=sqrt(Z->epsilon/(Z->gamma*SIN*SIN+2.0f*Z->alpha*COS*SIN+Z->beta*COS*COS));

	rotate_points_to_zero(X);
	rotate_points_to_zero(Y);
	rotate_points_to_zero(Z);

	fill_all_from_matrix(X,Y,Z,&new_matrix,X->color,Y->color,Z->color,0);

	resize_points(X,X_a,X_b);
	resize_points(Y,Y_a,Y_b);
	resize_points(Z,Z_a,Z_b);
	rotate_points_to_angle(X);
	rotate_points_to_angle(Y);
	rotate_points_to_angle(Z);

	memcpy(X->start_particle,X->particle,sizeof(float)*2*PARTICLE_COUNT);
	memcpy(Y->start_particle,Y->particle,sizeof(float)*2*PARTICLE_COUNT);
	memcpy(Z->start_particle,Z->particle,sizeof(float)*2*PARTICLE_COUNT);
	s = 0.0f;
}
