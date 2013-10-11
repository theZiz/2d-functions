void drawAllEllipse(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int x1,int y1,int x2,int y2)
{
	
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
