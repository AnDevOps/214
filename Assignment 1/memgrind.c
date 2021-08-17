#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "mymalloc.h"

#define malloc( x ) mymalloc( x , __FILE__ , __LINE__ )
#define free( x ) myfree( x , __FILE__ , __LINE__ )

int main (int argc, char **argv) {

//general variables
int i;
int j;
struct timeval start;
struct timeval end;

int workLoadMeanTimeA= 0;
//A: malloc() 1 byte and immediately free it - do this 120 times
char *testpointer;
  
    for(j = 0; j < 50; j ++){
        gettimeofday(&start, 0);
        for (i = 0; i < 120 ; i ++){
            testpointer = malloc(1);
            free(testpointer);
        }
        gettimeofday(&end, 0);
        workLoadMeanTimeA += ( (end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec) );
}
    
int workLoadMeanTimeB= 0;
//B: malloc() 1 byte, store the pointer in an array - do this 120 times.
//Once you've malloc()ed 120 byte chunks, then free() the 120 1 byte pointers one by one.
char *pointers[500];

for(j = 0; j < 50; j ++){
        gettimeofday(&start, 0);
        for (i = 0; i < 120 ; i ++){
            testpointer = malloc(1);
            pointers[i] = testpointer;
        }
        for (i = 0; i < 120 ; i ++){
            free(pointers[i]);
        }
         gettimeofday(&end, 0);
        workLoadMeanTimeB += ( (end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec) );

}
  


int workLoadMeanTimeC= 0;
//C: 240 times, randomly choose between a 1 byte malloc() or free()ing one of the malloc()ed pointers
//- Keep track of each operation so that you eventually malloc() 120 bytes, in total
//- Keep track of each operation so that you eventually free() all pointers
//(don't allow a free() if you have no pointers to free())
int zeroOrOne = rand() % 2;
int mallocCount = 0, currentlyMallocd = 0;

 for(j = 0; j < 50; j ++){
    zeroOrOne = rand() % 2;
    mallocCount = 0; 
    currentlyMallocd = 0;
        
        gettimeofday(&start, 0);
        for (i = 0; i < 240 ; i ++){                            //240 times (120 frees,120 mallocs)
            zeroOrOne = rand() % 2;                             //decides which to do(randomly)
            
          if (zeroOrOne == 1){                                //if == 1, -->malloc but if there has already been 120 mallocs, -->free
                if (mallocCount == 120){  
                    free(pointers[currentlyMallocd]);
                    currentlyMallocd--;

                }else{
                    testpointer = malloc(1);
                    currentlyMallocd++;
                    mallocCount++;
                    pointers[currentlyMallocd] = testpointer;
                }
                

            }else if (zeroOrOne == 0) {                          //if == 0, -->free but if there is nothing to free, -->malloc
                if (currentlyMallocd == 0){
                    testpointer = malloc(1);
                    currentlyMallocd++;
                    mallocCount++;
                    pointers[currentlyMallocd] = testpointer;
                }else{
                    free(pointers[currentlyMallocd]);
                    currentlyMallocd--;
                }
            }
        }
        
        gettimeofday(&end, 0);
        workLoadMeanTimeC += ( (end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec) );
    }
  

int workLoadMeanTimeD= 0;
//D: Start at size 1 and keep iterating until the maximum allowed size for malloc
//-for each iteration, malloc the current size and immediately free it 
//-since our malloc allows for a maximum mallocation of 4093, malloc then immediately free every number in the range 1 to 4093

 for(j = 0; j < 50; j ++){
          
        gettimeofday(&start, 0);
        for (i = 1; i < 4093 ; i ++){
            testpointer = malloc(i);
            free(testpointer);
        }
        gettimeofday(&end, 0);
        workLoadMeanTimeD += ( (end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec) );
 }

        
int workLoadMeanTimeE= 0;
//E: Malloc 254 bytes and save the pointers in an array 15 times and then malloc 253 bytes and save the pointer once more
//--memory is now at MAXIMUM allocation
//--then 15 times
//      --free a pointer from the initial array
//      --malloc 127 bytes(half of 254) and save pointer in a second array
//      --malloc 125 bytes(half of 254 -2 bytes for metadata) in a third array
//      --* once more do the last 3 items (126 and 124 bytes)
//      --free every pointer in second and third pointer arrays

    char *pointers1[50];
    char *pointers2[50];

    for(j = 0; j < 50; j ++){
          
     gettimeofday(&start, 0);
     for (i = 0; i < 15 ; i ++){
            testpointer = malloc(254);
            pointers[i] = testpointer;
        }
      pointers[16] = malloc(253);
        //maximum allocation with no free space left;

    for (i = 0; i < 15 ; i ++){
            
            free(pointers[i]);
            testpointer = malloc(127);
            pointers1[i] = testpointer;
            testpointer = malloc(125);
            pointers2[i] = testpointer;
            
    }   
            free(pointers[16]);
            testpointer = malloc(126);
            pointers1[16] = testpointer;
            testpointer = malloc(124);
            pointers2[16] = testpointer;

    for (i = 0; i < 15 ; i ++){

            free(pointers1[i]);
          
    }   
            free(pointers1[16]);

    for (i = 0; i < 15 ; i ++){

            free(pointers2[i]);
          
    }   
            free(pointers2[16]);

        gettimeofday(&end, 0);
        workLoadMeanTimeE += ( (end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec-start.tv_usec) );
    }


    //prints for average times
    printf("\n Average time for workload A is %d milliseconds\n", workLoadMeanTimeA/50); 
    printf("\n Average time for workload B is %d milliseconds\n", workLoadMeanTimeB/50); 
    printf("\n Average time for workload C is %d milliseconds\n", workLoadMeanTimeC/50); 
    printf("\n Average time for workload D is %d milliseconds\n", workLoadMeanTimeD/50); 
    printf("\n Average time for workload E is %d milliseconds\n", workLoadMeanTimeE/50); 
    

}
