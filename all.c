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
	#ifdef TRANSPARENCY
	spScaleDownSmooth(threeD_render,threeD_draw);
	#endif
	spBlitSurface(x1+x2>>1,y1+y2>>1,0,threeD_draw);
}

void fill_matrix_quadrupole(pMatrix matrix,float I)
{
	memset((*matrix),0,sizeof(float)*36);

	float energy = 250000.0f*0.000001602176565f; //Count * Q
	float k = -153.8f*I/(energy*0.6242f-0.511f)/1000000.0f; //1/mm²
	float L = 100.0f; //mm
	(*matrix)[0][0] = 1.0f;
	(*matrix)[1][1] = 1.0f;
	(*matrix)[2][2] = 1.0f;
	(*matrix)[3][3] = 1.0f;
	(*matrix)[4][4] = 1.0f;
	(*matrix)[5][5] = 1.0f;
	(*matrix)[1][0] = -k*L;
	(*matrix)[3][2] = k*L;
	(*matrix)[4][5] = 1.0;
}

void fill_matrix_buncher(pMatrix matrix,float a, float U, float phi, float f)
{
	memset((*matrix),0,sizeof(float)*36);

	phi = M_PI * phi / 180.0f;

	float ekin = 250000.0f*0.000001602176565f; //Count * Q
	float gamma = ekin/0.818710438f+1.0f;
	float beta = sqrt(1.0f-1.0f/(gamma*gamma));
	
	float w1 = ekin;
	float w0 = 0.000001602176565f * U * cos(phi); // Q*O*cos(phi)
	float lambda = 299792458000.0f / f; // c/f
	float r65 = sin(phi) * (2 * M_PI * 0.000001602176565f * U)/((w1 + w0) * beta * lambda);

	(*matrix)[0][0] = 1.0f;
	(*matrix)[0][1] = a;
	(*matrix)[1][1] = 1.0f;
	(*matrix)[2][2] = 1.0f;
	(*matrix)[2][3] = a;
	(*matrix)[3][3] = 1.0f;
	(*matrix)[4][4] = 1.0f;
	(*matrix)[4][5] = a / (gamma * gamma);
	(*matrix)[5][4] = r65;
	(*matrix)[5][5] = w1 / (w1 + w0);
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
	if (threeD_draw)
		spDeleteSurface(threeD_draw);
	#ifdef TRANSPARENCY
	if (threeD_render)
		spDeleteSurface(threeD_render);
	#endif
	threeD_draw = spCreateSurface(w,h);
	#ifdef TRANSPARENCY
	threeD_render = spCreateSurface(w*2,h*2);
	#else
	threeD_render = threeD_draw;
	#endif
	spSetPerspective( 45.0, ( float )w / ( float )h, 1.0f, 10000.0f );
}

void calcAll(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2,int steps)
{
	spSelectRenderTarget(threeD_render);
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
	spTranslate(0,0,spFloatToFixed(-1.0f)+spFloatToFixed(-4.0f*allZoom));
	spMulMatrix(rotation);
	int i;
	for (i = 0; i < PARTICLE_COUNT; i+=PARTICLE_NEXT_DRAW)
	{
		#ifdef TRANSPARENCY
		spEllipse3D(spFloatToFixed(X->particle[0][i]),spFloatToFixed(Y->particle[0][i]),spFloatToFixed(Z->particle[0][i]),SP_ONE/16,SP_ONE/16, get_colour(X->particle[1][i],Y->particle[1][i],Z->particle[1][i],i));
		#else
		spEllipse3D(spFloatToFixed(X->particle[0][i]),spFloatToFixed(Y->particle[0][i]),spFloatToFixed(Z->particle[0][i]),SP_ONE/32,SP_ONE/32, get_colour(X->particle[1][i],Y->particle[1][i],Z->particle[1][i]));
		#endif
	}
	#ifdef TRANSPARENCY
	spDeactivatePattern();
	#endif
	spSetBlending(SP_ONE/2);
	spSetLineWidth(4);
	Sint32 lineSize = spMul(SP_ONE/3,spFloatToFixed(-1.0f)+spFloatToFixed(-4.0f*allZoom));
	spLine3D(-lineSize,0,0,lineSize,0,0,X->color);
	spLine3D(0,-lineSize,0,0,lineSize,0,Y->color);
	spLine3D(0,0,-lineSize,0,0,lineSize,Z->color);
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
	/*printf("Change:\n");
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
	printf("\n");*/
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
	switch (element)
	{
		case 0: fill_matrix_solenoid(&change_matrix,1.0f); break;
		case 1: fill_matrix_buncher(&change_matrix,40.0f,19000.0f,-90.0f,260000000.0f); break;
		case 2: fill_matrix_buncher(&change_matrix,40.0f,12000.0f,+90.0f,1300000000.0f); break;
		case 3: fill_matrix_quadrupole(&change_matrix,+0.2f); break;
		case 4: fill_matrix_quadrupole(&change_matrix,-0.2f); break;
	}
	else
	switch (element)
	{
		case 0: fill_matrix_solenoid(&change_matrix,10.0f); break;
		case 1: fill_matrix_buncher(&change_matrix,0.01f,19000.0f,-90.0f,260000000.0f); break;
		case 2: fill_matrix_buncher(&change_matrix,0.01f,12000.0f,+90.0f,1300000000.0f); break;
		case 3: fill_matrix_quadrupole(&change_matrix,+0.2f); break;
		case 4: fill_matrix_quadrupole(&change_matrix,-0.2f); break;
	}
	resetPhasenraumDrift(X);
	resetPhasenraumDrift(Y);
	resetPhasenraumDrift(Z);
	
	multiplyMatrixPhasenraum(X,change_matrix[0][0],change_matrix[0][1],change_matrix[1][0],change_matrix[1][1]);
	multiplyMatrixPhasenraum(Y,change_matrix[2][2],change_matrix[2][3],change_matrix[3][2],change_matrix[3][3]);
	multiplyMatrixPhasenraum(Z,change_matrix[4][4],change_matrix[4][5],change_matrix[5][4],change_matrix[5][5]);
        	
	resetPhasenraumDrift(X);
	resetPhasenraumDrift(Y);
	resetPhasenraumDrift(Z);

	s = 0.0f;
}
