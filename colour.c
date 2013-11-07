static float shrink(float value)
{
	if (test_values == 0)
		value*=256.0f;
	if (value < -1.0f)
		return 0.0f;
	if (value > 1.0f)
		return 1.0f;
	return value/2.0f+0.5f;
}

#ifdef TRANSPARENCY
static Uint16 get_colour(float x,float y,float z,int nr)
#else
static Uint16 get_colour(float x,float y,float z)
#endif
{
	float length = sqrt(x*x+y*y+z*z);
	if (test_values == 0)
		length*=256.0f;
	float value = 1.0f / ( 1.0f + length );
	#ifdef TRANSPARENCY
	//alpha is decreasing with distance
	spSetAlphaPattern4x4((int)(value*255.0f),allTransparency[nr]);
	//alpha is constant
	//spSetAlphaPattern4x4(80,allTransparency[nr]);
	#endif
	if (colormode == 0)
		//from green over yellow to red:
		return spGetHSV((int)((float)SP_PI*(value*2.0f/3.0f)),255,255);	
		
	float r,g,b,v;
	if (colormode == 1)
	{
		//from black over blue to white
		if (value > 0.5f)
		{
			v = (1.0f-value)*2.0f;
			r = v/4.0f;
			g = v/4.0f;
			b = v;
		}
		else
		{
			v = (0.5f-value)*2.0f;
			r = 0.25f + 0.75f*v;
			g = 0.25f + 0.75f*v;
			b = 1.0f;
		}
	}
	else
	{
		//length makes brighter, x,y & z make color	
		//X
		v = shrink(x);
		r += v;
		g += (1.0f-v);
		b += (1.0f-v);
		//Y
		v = shrink(y);
		r += (1.0f-v);
		g += v;
		b += (1.0f-v);
		//Z
		v = shrink(z);
		r += (1.0f-v);
		g += (1.0f-v);
		b += v;

		r*=(1.0f-value)/3.0f;
		g*=(1.0f-value)/3.0f;
		b*=(1.0f-value)/3.0f;
	}
	return spGetRGB((int)(r*255.0f),(int)(g*255.0f),(int)(b*255.0f));
}
