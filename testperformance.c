#include <stdlib.h>
#include <stdio.h>
#include "malloc.h"
#include "tst.h"

#define SIZE 10000
#define TIMES 15000

int main(int argc, char *argv[]){

  int i;
  char* array[TIMES];
  for(i=0; i<TIMES; i++)
  {
	int size = rand()%SIZE;
	array[i] = malloc(size);
  }
  for(i=0; i<TIMES;i++)
  {
	int size = rand()%SIZE;
	array[i] = realloc(array[i], size);
  }
  for(i=0; i<TIMES; i++)
  {
	free(array[i]);
  }
 

  /*
  for( i=0; i<TIMES; i++)
  {
	int size = rand()%SIZE;
	p = malloc(size);
	size = rand()%SIZE;
	p = realloc(p, size);
	free(p);
  }*/

  return 0;
}



