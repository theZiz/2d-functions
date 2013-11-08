float trace[TRACK_COUNT][TRACK_SIZE][3];
Uint16 trace_colour[TRACK_COUNT];
int trace_point[TRACK_COUNT];
int trace_start;
float global_z;

void initTrace(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z)
{
	int i,j;
	memset(trace,0,TRACK_COUNT*TRACK_SIZE*3*sizeof(float));
	for (i = 0; i < TRACK_COUNT; i++)
		trace_colour[i] = rand()&65535;
	trace_start = 0;
	char used[PARTICLE_COUNT];
	memset(used,0,PARTICLE_COUNT*sizeof(char));
	for (i = 0; i < TRACK_COUNT; i++)
		while (1)
		{
			trace_point[i] = rand()%PARTICLE_COUNT;
			if (used[trace_point[i]] == 0)
			{
				used[trace_point[i]] = 1;
				break;
			}
		}
	for (i = 0; i < TRACK_SIZE; i++)
		for (j = 0; j < TRACK_COUNT; j++)
		{
			trace[j][i][0] = X->particle[0][trace_point[j]];
			trace[j][i][1] = Y->particle[0][trace_point[j]];
			trace[j][i][2] = Z->particle[0][trace_point[j]];
		}
	global_z = 0.0f;
}

void drawTrace(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z)
{
	int i,j;
	for (j = 0; j < TRACK_COUNT; j++)
	{
		#ifdef TRANSPARENCY
		spEllipse3D(spFloatToFixed(trace[j][trace_start][0]),spFloatToFixed(trace[j][trace_start][1]),spFloatToFixed(trace[j][trace_start][2]+global_z),SP_ONE/4,SP_ONE/4, trace_colour[j]);
		#else
		spEllipse3D(spFloatToFixed(trace[j][trace_start][0]),spFloatToFixed(trace[j][trace_start][1]),spFloatToFixed(trace[j][trace_start][2]+global_z),SP_ONE/8,SP_ONE/8, trace_colour[j]);
		#endif
		for (i = (trace_start+1)%TRACK_SIZE; i != trace_start; i=(i+1)%TRACK_SIZE)
		{
			#ifdef TRANSPARENCY
			spEllipse3D(spFloatToFixed(trace[j][i][0]),spFloatToFixed(trace[j][i][1]),spFloatToFixed(trace[j][i][2]+global_z),SP_ONE/8,SP_ONE/8, trace_colour[j]);
			#else
			spEllipse3D(spFloatToFixed(trace[j][i][0]),spFloatToFixed(trace[j][i][1]),spFloatToFixed(trace[j][i][2]+global_z),SP_ONE/16,SP_ONE/16, trace_colour[j]);
			#endif
			/*
			int k = i+1;
			if (k >= TRACK_SIZE)
				k-=TRACK_SIZE;
			spLine3D(spFloatToFixed(trace[j][i][0]),spFloatToFixed(trace[j][i][1]),spFloatToFixed(trace[j][i][2]+global_z),
			         spFloatToFixed(trace[j][k][0]),spFloatToFixed(trace[j][k][1]),spFloatToFixed(trace[j][k][2]+global_z), trace_colour[j]);
			*/
		}
	}
}

float traceStepRest = 0;

void updateTrace(tPhasenraum* X,tPhasenraum* Y,tPhasenraum* Z,int steps)
{
	float ONE_STEP = spFixedToFloat(SP_ONE>>one_step);
	if (pause)
		return;
		
	global_z += (float)steps*ONE_STEP;
	
	if (test_values)
		ONE_STEP*=256.0f;
	int i,j;
	float parts = ((float)steps+traceStepRest)/TRACK_STEP*ONE_STEP;
	traceStepRest = (float)steps+traceStepRest-parts*TRACK_STEP/ONE_STEP;
	
	int i_parts = (int)parts;
	traceStepRest += parts-(float)i_parts;
	
	int old_start = trace_start;
	trace_start = (trace_start+TRACK_SIZE-i_parts) % TRACK_SIZE;
	
	for (j = 0; j < i_parts; j++)
		for (i = 0; i < TRACK_COUNT; i++)
		{
			trace[i][(trace_start+j)% TRACK_SIZE][0] = ((float)(i_parts-j)* X->particle[0][trace_point[i]]          +(float)(j)*trace[i][old_start][0])/(float)i_parts;
			trace[i][(trace_start+j)% TRACK_SIZE][1] = ((float)(i_parts-j)* Y->particle[0][trace_point[i]]          +(float)(j)*trace[i][old_start][1])/(float)i_parts;
			trace[i][(trace_start+j)% TRACK_SIZE][2] = ((float)(i_parts-j)*(Z->particle[0][trace_point[i]]-global_z)+(float)(j)*trace[i][old_start][2])/(float)i_parts;
		}
}
