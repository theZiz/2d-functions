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

void line(int x1,int y1,int z1,int x2,int y2,int z2,Uint16 color)
{
	spSetLineWidth(2);
	spLine(x1,y1,z1,x2,y2,z2,spGetRGB(128,128,128));
	spSetLineWidth(1);
	if (x1 == x2)
	{
		if (y1 < y2)
			spLine(x1,y1+1,z1,x2,y2-1,z2,color);
		else
			spLine(x1,y1-1,z1,x2,y2+1,z2,color);
	}
	else
	{
		if (x1 < x2)
			spLine(x1+1,y1,z1,x2-1,y2,z2,color);
		else
			spLine(x1-1,y1,z1,x2+1,y2,z2,color);
	}
}
