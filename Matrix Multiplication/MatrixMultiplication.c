#include "pthread.h"
#include "stdio.h"
#include <stdlib.h>

#define M 3
#define K 2
#define N 3

int A[M][K]={{1,4},{2,5},{3,6}};
int B[K][N]={{8,7,6},{5,4,3}};
int C[M][N];

/* structure for passing data to threads */
struct v
{
   int i; /*row*/
   int j; /*column*/
};

void* thread_cal(void *arg)
{
    struct v *temp=(struct v*) arg;
    int row=temp->i;
    int col=temp->j;
    int t=0;

    C[row][col]=0;

    for(t=0;t<K;t++)
        C[row][col] += A[row][t]*B[t][col];

    pthread_exit((void *)C[row][col]);
}

int main()
{
    pthread_t matrix_multi[M][N];
    void* product[M][N];
    int i=0;
    int j=0;
    /*We have to create M * N worker threads*/
    for(i=0;i<M;i++)
        for(j=0;j<N;j++)
        {
            struct v *data=(struct v*)malloc(sizeof(struct v));
            data->i=i;
            data->j=j;
            /*Now create the thread passing it data as a parameter*/
            pthread_create(&matrix_multi[i][j],NULL,thread_cal,(void*)data);
        }

    for(i=0;i<M;i++)
    {
        for(j=0;j<N;j++)
        {
            pthread_join(matrix_multi[i][j],(void *) &product[i][j]);
            printf(" %d ", (int)product[i][j]);
        }
        printf("\n");
    }

    exit(0);
}
