void mul_matrix(float* A,float* B,float* C)
{
	//matrix[y+x*6]
	int x,y;
	for (x = 0;x < 6; x++)
		for (y = 0; y < 6; y++)
			C[y+x*6] = A[y+0*6]*B[0+x*6] + A[y+1*6]*B[1+x*6] + A[y+2*6]*B[2+x*6]+
								 A[y+3*6]*B[3+x*6] + A[y+4*6]*B[4+x*6] + A[y+5*6]*B[5+x*6];
}

void mul_matrix_trans(float* A,float* B,float* C)
{
	//matrix[y+x*6]
	int x,y;
	for (x = 0;x < 6; x++)
		for (y = 0; y < 6; y++)
			C[y+x*6] = A[y+0*6]*B[x+0*6] + A[y+1*6]*B[x+1*6] + A[y+2*6]*B[x+2*6]+
								 A[y+3*6]*B[x+3*6] + A[y+4*6]*B[x+4*6] + A[y+5*6]*B[x+5*6];
}
