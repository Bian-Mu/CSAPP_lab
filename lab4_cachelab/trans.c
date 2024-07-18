/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void solution_of_32(int M, int *A, int *B);
void transpose_each_32(int M, int *A, int *B);

void solution_of_64(int M, int *A, int *B);
void transpose_each_64(int M, int *A, int *B);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M % 64 == 0)
    { // 1700
        solution_of_64(M,&A[0][0], &B[0][0]);
    }
    else if (M % 32 == 0)
    { // 288
        solution_of_32(M, &A[0][0], &B[0][0]);
    }
    else
    {   //1951
        for (int i = 0; i < N; i += 17)
        {
            for (int j = 0; j < M; j += 17)
            {
                for (int k = i; k < i + 17 && k < N; ++k)
                {
                    for (int s = j; s < j + 17 && s < M; ++s)
                    {
                        B[s][k] = A[k][s];
                    }
                }
            }
        }
    }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */
void solution_of_32(int M, int *A, int *B)
{
    int i, j;
    for (i = 0; i < M; i += 8)
    {
        for (j = 0; j < M; j += 8)
        {
            transpose_each_32(M, A + i * M + j, B + j * M + i);
        }
    }
}
void transpose_each_32(int M, int *A, int *B)
{
    int i, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    for (i = 0; i < 8; i++)
    {
        tmp1 = *(A + i * M);
        tmp2 = *(A + i * M + 1);
        tmp3 = *(A + i * M + 2);
        tmp4 = *(A + i * M + 3);
        tmp5 = *(A + i * M + 4);
        tmp6 = *(A + i * M + 5);
        tmp7 = *(A + i * M + 6);
        tmp8 = *(A + i * M + 7);
        *(B + i) = tmp1;
        *(B + M + i) = tmp2;
        *(B + 2 * M + i) = tmp3;
        *(B + 3 * M + i) = tmp4;
        *(B + 4 * M + i) = tmp5;
        *(B + 5 * M + i) = tmp6;
        *(B + 6 * M + i) = tmp7;
        *(B + 7 * M + i) = tmp8;
    }
}

//满分解法是，先转置左上到左上，右上到右上；复制右上，转置左下到右上，把复制的右上传送到左下；转置右下到右下
void solution_of_64(int M, int *A, int *B)
{
    int i, j;
    for (i = 0; i < M; i += 4)
    {
        for (j = 0; j < M; j += 4)
        {
            transpose_each_64(M, A + i * M + j, B + j * M + i);
        }
    }
}
void transpose_each_64(int M, int *A, int *B)
{
    int i, tmp1, tmp2;
    int tmp3, tmp4;
    for (i = 0; i < 4; i++)
    {
        tmp1 = *(A + i * M);
        tmp2 = *(A + i * M + 1);
        tmp3 = *(A + i * M + 2);
        tmp4 = *(A + i * M + 3);

        *(B + i) = tmp1;
        *(B + (1) * M + i) = tmp2;
        *(B + (2) * M + i) = tmp3;
        *(B + (3) * M + i) = tmp4;
    }
}
/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
