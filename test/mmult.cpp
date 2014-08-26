#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <ctime>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "gtest/gtest.h"

#include "libgallocy.h"


#define MAX_THREAD 4


int NDIM = -1;


double** a = NULL;
double** b = NULL;
double** c = NULL;


typedef struct {
  int id;
  int noproc;
  int dim;
  double (**a), (**b), (**c);
} parm;


void init_matrix(double*** m) {
   int i;
   *m = (double**) custom_malloc(sizeof(double*) * NDIM);
   for (i = 0; i < NDIM; i++) {
      (*m)[i] = (double*) custom_malloc(sizeof(double) * NDIM);
   }
}


void mm(int me_no, int noproc, int n, double** a, double** b, double** c) {
   int i,j,k;
   double sum;
   i = me_no;
   while (i<n) {
      for (j = 0; j < n; j++) {
         sum = 0.0;
         for (k = 0; k < n; k++)
            sum = sum + a[i][k] * b[k][j];
         c[i][j] = sum;
      }
      i += noproc;
   }
}


void* worker(void *arg) {
   parm* p = (parm*) arg;
   mm(p->id, p->noproc, p->dim, (p->a), (p->b), (p->c));
   return NULL;
}


#if 0
void print_matrix(int dim) {
   int i,j;
   printf("The %d * %d matrix is\n", dim,dim);
   for(i=0;i<dim;i++){
      for(j=0;j<dim;j++)
         printf("%lf ",  c[i][j]);
      printf("\n");
   }
}
#endif


int check_matrix(int dim) {
  int i,j,k;
  int error=0;
  for(i=0;i<dim;i++) {
    for(j=0;j<dim;j++) {
      double e=0.0;
      for (k=0;k<dim;k++)
        e += a[i][k] * b[k][j];
      if (e!=c[i][j])
        error++;
    }
  }
  return error;
}


TEST(GallocyTest, MatrixMultiplication) {

   int j;
   pthread_t* threads;
   pthread_attr_t pthread_custom_attr;
   parm* arg;
   int n, i;

#if 0
   if (argc != 3) {
      printf("Usage: %s n dim\n  where n is no. of thread and dim is the size of matrix\n", argv[0]);
      exit(1);
   }

   n = atoi(argv[1]);

   if ((n < 1) || (n > MAX_THREAD)) {
      printf("The no of thread should between 1 and %d.\n", MAX_THREAD);
      exit(1);
   }

   NDIM = atoi(argv[2]);
#endif

   n = 4;
   NDIM = 128;

   ASSERT_EQ(a, (void *) NULL);
   ASSERT_EQ(b, (void *) NULL);
   ASSERT_EQ(c, (void *) NULL);

   init_matrix(&a);
   init_matrix(&b);
   init_matrix(&c);

   ASSERT_NE(a, (void *) NULL);
   ASSERT_NE(b, (void *) NULL);
   ASSERT_NE(c, (void *) NULL);

   for (i = 0; i < NDIM; i++) {
      for (j = 0; j < NDIM; j++) {
         a[i][j] = i + j;
         b[i][j] = i + j;
      }
   }

   threads = (pthread_t*) custom_malloc(n * sizeof(pthread_t));
   pthread_attr_init(&pthread_custom_attr);
   arg = (parm*) custom_malloc(sizeof(parm) * n);
   for (i = 0; i < n; i++) {
      arg[i].id = i;
      arg[i].noproc = n;
      arg[i].dim = NDIM;
      arg[i].a = a;
      arg[i].b = b;
      arg[i].c = c;
      pthread_create(&threads[i], &pthread_custom_attr, worker, (void*) (arg+i));
   }

   for (i = 0; i < n; i++) {
      pthread_join(threads[i], NULL);
   }

   // For small arrays, print them to the screen
   //if (NDIM <= 16)
   //  print_matrix(NDIM);

   ASSERT_EQ(check_matrix(NDIM), 0);

   custom_free(arg);
}
