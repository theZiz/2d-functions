typedef float tMatrix[6][6];
typedef tMatrix* pMatrix;

void mul_matrix(pMatrix A,pMatrix B,pMatrix C)
{
	//matrix[y+x*6]
	int x,y;
	for (x = 0;x < 6; x++)
		for (y = 0; y < 6; y++)
			(*C)[y][x] = (*A)[y][0]*(*B)[0][x]
			           + (*A)[y][1]*(*B)[1][x]
			           + (*A)[y][2]*(*B)[2][x]
			           + (*A)[y][3]*(*B)[3][x]
			           + (*A)[y][4]*(*B)[4][x]
			           + (*A)[y][5]*(*B)[5][x];
}

void mul_matrix_trans(pMatrix A,pMatrix B,pMatrix C)
{
	//matrix[y+x*6]
	int x,y;
	for (x = 0;x < 6; x++)
		for (y = 0; y < 6; y++)
			(*C)[y][x] = (*A)[y][0]*(*B)[x][0]
			           + (*A)[y][1]*(*B)[x][1]
			           + (*A)[y][2]*(*B)[x][2]
			           + (*A)[y][3]*(*B)[x][3]
			           + (*A)[y][4]*(*B)[x][4]
			           + (*A)[y][5]*(*B)[x][5];
}
