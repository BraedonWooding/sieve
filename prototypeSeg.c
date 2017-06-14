#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <pthread.h>
#include <omp.h>

#define LIMIT 1000000000
//24*x < 2097152
#define CACHE_SIZE 1920000
//CACHE_SIZE / 8
#define LENGTH 240000

void primeFinder (unsigned char list[], unsigned long long start);
void assignPos (unsigned char list[], unsigned long long pos);
void preProcess (unsigned char list[], unsigned long long start);
unsigned char primePos (unsigned char list[], unsigned long long pos);
unsigned long long listPrinter (unsigned char list[]);
unsigned long long stLoc (unsigned long long st, unsigned long long mul);

int main(int argc, char * argv[]) {

   unsigned long long size = CACHE_SIZE;
   unsigned long long limit = LIMIT;

   if (argc <= 1 || sscanf(argv[1], "%llu", &limit) != 1){
      printf("No arguments given, default %llu used.\n", limit);
   }

   limit /= CACHE_SIZE;
   limit++;

   unsigned char *list = (unsigned char*)malloc((size/8) * sizeof(unsigned char));
   //checks if malloc worked
   if (list == NULL) {
       printf("malloc didn't work\n");
       exit(EXIT_FAILURE);
   }

   unsigned long long i = 0;
   unsigned long long total = 0;

   while (i<limit){
      preProcess(list, i);

      primeFinder(list, i);

      total += listPrinter(list);
      i++;
   }

   printf("%llu\n",total);

   free(list);

   return EXIT_SUCCESS;
}

void primeFinder (unsigned char list[], unsigned long long start) {

   unsigned long long startPos = (start*CACHE_SIZE);
   unsigned long long startValue = 5;
   unsigned long long i = stLoc(startPos, startValue) - 1;
   unsigned long long endPos = startPos+CACHE_SIZE;
   unsigned long long j = 0;

   double lenCalc = ceil(sqrt(endPos));

   //needed due to presieve (stLoc can return a value out of phase)
   i += (i&1)?startValue:0;

   while (startValue < lenCalc) {
      //plus 1 as arrays start at 0
      j = i;
        
      while (j < CACHE_SIZE) {
         assignPos(list, j);
         j += startValue*2;
      }

      //this skips the even numbers
      startValue++;
      startValue++;

      i = stLoc(startPos, startValue) - 1;
      i += (i&1)?startValue:0;

   }
}

unsigned long long stLoc (unsigned long long st, unsigned long long mul){

   //returns equivalent array position
   unsigned long long offset = st%mul;
   if (!st) return mul*mul;
   return ((offset)?(mul - offset):0);
}

void assignPos (unsigned char list[], unsigned long long pos){

   unsigned char power = 1;
   //I bitshift below to increase preformance
   int internalPos = (~pos)&7;
   //incase it wasn't noticed, /8 as each char holds 8 bits
   list[pos/8] = list[pos/8] | (power << internalPos);
}

unsigned long long listPrinter(unsigned char list[]) {

   unsigned long long count = 0;
   
   #pragma omp parallel num_threads(4) reduction(+:count)
   {
      unsigned long long i = omp_get_thread_num();

      while (i < LENGTH) {
         unsigned char n = ~list[i];
         while (n) {
            n &= (n-1);
            count++;
         }
         i+=4;
      }
   }
   return count;
}

unsigned char primePos (unsigned char list[], unsigned long long pos){
   unsigned char toFind = list[pos/8];
   int internalPos = (~pos)&7;
   return ((toFind >> internalPos) & 1);
}

void preProcess (unsigned char list[], unsigned long long start) {

   register unsigned long long i = 0;//
   register unsigned long long length = LENGTH;//
   
   unsigned char preVal[3];
   unsigned char j = 0;//

   if(start){

      preVal[0] = 117;
      preVal[1] = 215;
      preVal[2] = 93;

   } else {

      preVal[0] = 215;
      preVal[1] = 93;
      preVal[2] = 117;
      list[0] = 149;
      i++;//
   }

   while (i < length) {
      list[i] = preVal[j];
      i++;
      j++;
      if (i < length) {
         list[i] = preVal[j];
         i++;
         j++;
         if (i < length) {
            list[i] = preVal[j];
            i++;
            j=0;
         }
      }
   }
   /*
   #pragma omp parallel num_threads(3)
   {
      unsigned int j = omp_get_thread_num();
      unsigned long long i = j+(!start);
      while (i < LENGTH){
         list[i] = preVal[j];
         i+=3;
      }
   }*/
}
