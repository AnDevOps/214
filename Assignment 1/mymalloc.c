#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mymalloc.h"

#define MEM_LENGTH 4096
#define MAX_LENGTH 4093

// simulates main memory 
static char myblock [MEM_LENGTH];

bool firstMalloc() {
    // temp pointer that points to the first byte
    char *temp = myblock;

    // we're gonna set the first byte to 1 (should be a type char)()
    if(*temp == 1) return false;

    // returns true if its everything but 1
    return true;
}

void * mymalloc(size_t size, const char *file, int line) {

    // Error Condition: The requested malloc size is 0.
    // mymalloc argument must be > 0
    if(size == 0) {
        printf("Error: Requested Malloc Size is 0.\n");
        return NULL;
    }

    // error case where we malloc > 4093 because we reserve one byte for flag, two bytes for metadata --> 4096 - 2 - 1 = 4093 available
    // memarray will never be able to hold anything over max_length (attempting to saturate memory)
    if(size > MAX_LENGTH) {
        printf("Error: Required Malloc Size is beyond the maximum length of the memory.\n");
        return NULL;
    }

    //each iteration of malloc starts at start of memarray
    char *arrp = myblock, *returnp;
    bool found = false;
    short meta, nextMeta;
    int pointerCount = 0;

   // CASE A: Malloc has never been called yet - myblock[4096] should be empty.
   if(firstMalloc()) {

       // special value we'll use to indicate whether malloc was ever been called before
       char flag = 1;

       // arrp should be pointing to the first malloc, so we copy the flag to the first byte
       memcpy(arrp, &flag, sizeof(char));

       // we want to move the pointer past the flag + the pointerCount too
       arrp ++;
       pointerCount ++;

       // we want to save the current size of the metadata
       meta = size;

        // Edge Case: This is where you malloc a large size but have 1-2 bytes left over which means that you either A) can't afford to fit a meta data B) can afford to fit a metadata but have no available space
       if(meta >= (MEM_LENGTH - sizeof(short) - sizeof(char) - 2)) {
           meta = MAX_LENGTH;
           memcpy(arrp, &meta, sizeof(short));
       } else {
           // we want to copy the metadata into the array and iterate past
           memcpy(arrp, &meta, sizeof(short));
       }

       // iterate past the meta data
       arrp += sizeof(short);
       pointerCount += 2;

       // we want to save the pointer to this free space
       returnp = arrp;

       // case where it allocates the max size, the maximum size that it can ever have is 4096 - 2 (size of metadata) - 1 (size of flag) thus it doesnt need the second empty metadata
        if(size >= (MEM_LENGTH - sizeof(short) - sizeof(char) - 2)) return returnp;

        // skip past the allocated data
        arrp += size;
        pointerCount += size;

        // create new metadata into array (MEM_LENGTH - METADATA (2 BYTES) - ALLOCATED SPACE (SIZE) - METADATA (2 BYTES) - FLAG (1 BYTE))
        meta = -(MEM_LENGTH - (size + 2*sizeof(short) + sizeof(char)) );
        
        // we copy the metadata into the array
        memcpy(arrp, &meta, sizeof(short));
   
   // case B: there is already a metadata in the memory, go to the next !
   } else {

       // we want to skip past the 'firstmalloc' flag | arrp should be pointing to the front
       arrp ++;
       pointerCount ++;

       // make a loop that finds the unused meta data and determines if it is the right size
       while(found == false) {
           // saves the current size of the metadata
            memcpy(&meta, arrp, sizeof(meta));

            // Case 1: The size of the current metadata is being used ---> We skip to the next iteration along with the increments of the pointers.
            // -- meta > 0 : if metadata > 0, it means its used

            // Case 2: The metadata is currently not used but the space that the metadata holds is not sufficent.
            // -- meta < 0: if metadata < 0, it means its not used. 
            // -- abs(meta) < size:  if the absolute value of the meta (it's negative to denote unused) is less than size, that is not sufficent.
            if((meta > 0) || ((meta < 0) && (abs(meta) < size))) {
                // Error Case : We reached the last possible metadata and it's already being used.
                if(pointerCount + 2 + abs(meta) >= MEM_LENGTH) {
                    printf("Error: There is no contiguous memory available for allocation.\n");
                    return NULL;
            }

                // skips past the metadata
                arrp += sizeof(short);
                pointerCount += 2;

                // skips past the allocated space
                arrp += abs(meta);
                pointerCount += abs(meta);

                // next iteration
                continue;
            }

            // Case 3: The metadata is currently not used AND the space that the metadata holds is sufficent : we can insert/replace 
            if((meta < 0) && (abs(meta) >= size)) {                
                // we can terminate the search
                found = true;

                // we save the current size of the free block of memory : This is used for when we calculate the NEXT metadablock because we need to subtract out all the stuff we inserted
                memcpy(&nextMeta, arrp, sizeof(nextMeta));

                // meta = size ... technically we can just use size
                meta = size;
                
                // Edge Case 1: This is where you malloc a large size but have 1-2 bytes left over which means that you either A) can't afford to fit a meta data B) can afford to fit a metadata but have no available space
                /* Edge Case 2: The second edge case is when we have a available space that is large enough for the requested space but the allocated space >= available space - 2. This means that we end up having to append
                   the remaining bytes because it is either A) not large enough for a metadata B) large enough for a metadata but has no space so it is redundant */
                if(pointerCount + size >= (MEM_LENGTH - sizeof(short) - sizeof(char) - 2) || size >= abs(nextMeta) - 2) meta = abs(nextMeta);

                memcpy(arrp, &meta, sizeof(short));

                // skip past the meta data 
                arrp += sizeof(short);

                // case where it allocates the max size, the maximum size that it can ever have is 4096 - 2 (size of metadata) - 1 (size of flag) thus it doesnt need the second empty metadata
                if(pointerCount + size >= (MEM_LENGTH - sizeof(short) - sizeof(char) - 2) || size >= abs(nextMeta) - 2) return arrp;

                // saves the pointer to return because the arrp should be pointing to the free space now
                returnp = arrp;

               // now we want to check if we actually insert the new NEXT metadata or not
               if(abs(nextMeta) > size) {

                   // skip past the allocated space
                   arrp += size;
                   pointerCount += size;

                   // space should be equal to old space - space of allocation - space of metadata
                   meta = -(abs(nextMeta) - (size + sizeof(short)));

                   // we want copy in the metadata
                   memcpy(arrp, &meta, sizeof(short));
               }
            
            }

       }
    }
    return returnp;
}

void myfree(void *ptrin, const char *file, int line) {

    // Error Condition A: This condition checks if we are freeing addresses that are not pointers.
    // In this case, if we attempt to free a non-pointer such as int x = 5, the un-initialized pointer that we pass into the argument will be NULL.
    if (ptrin == NULL) {
		printf("Error: You have attempted to free an address that is not a pointer in the memory array.\n");
		return;
	}

    // Error Condition 2: This condition accounts for when we attempt to free an empty myblock[4096]
    if (firstMalloc()){
        printf("Error: There is nothing allocated in the memory thus there is nothing to free.\n");
		return;
    }

    // We want to typecast the argument pointer as a char because chars are of size 1 byte and easy to manipulate. 
    char *ptr = (char*)ptrin;

    //check if ptr is in actual memory block and if it is, free it
    //do this by checking every block after already allocated space

    // Ptrchecker is the pointer that will iterate through myblock. It starts at myblock + 1 because we want to skip over the flag.
    char* ptrchecker = myblock + 1;

    // Bool flags that are used to search for the correct metadata that is connected to the argument pointer.
    bool run = true, found = false;

    // nextMeta: variable used to hold the current metadata
    // meta : variable used to hold the next/previous metadata 
    short nextMeta = 0, meta = 0, zero = 0;
    int count = 0, checkfreed = 0;
    int8_t zeroSizeOne = 0;

    while (found == false && run == true){
        
        //store current meta data size
        memcpy( &nextMeta, ptrchecker, sizeof(nextMeta));

        //cant run over limit
        if ((count + abs(nextMeta) + 2 ) == MEM_LENGTH){
            break;
        }
        //iterate into 'returnptr'
        ptrchecker += sizeof(short);
        count += sizeof(short);
        
        //check
        if ((char*)ptrchecker == ptr){
            found = true;
            //do freeing
        
            //step back and set to negative(if in use)
            if (nextMeta > 0){
                ptrchecker -= sizeof(short);
                meta = nextMeta * (-1);
                //copy negative into ptrchecker
                memcpy(ptrchecker, &meta, sizeof(short)); 

            //if already not in use ERROR --only works when freeing same ptr twice, first one did not result in any combining
            // Error Condition C: This error condition accounts for the case when we attempt to free a pointer that is already freed.
            } else {
                printf("\nError: The pointer that we are attempting to free is already freed.\n");
                return;
            }

            //now clear data
            ptrchecker += sizeof(short);
            int j;
            for (j = 0; j < abs(meta); j++){
                memcpy(ptrchecker, &zeroSizeOne, sizeof(zeroSizeOne)); 
                ptrchecker ++;
            }

            break;

        }else{
            //if in 'free data', check all for pointer, if found - error case this pointer has already been freed
            if (nextMeta < 0){
                checkfreed = abs(nextMeta);
                while (checkfreed > 1){
                    ptrchecker ++;

                    // Error Condition C: This error condition accounts for the case when we attempt to free a pointer that is already freed.
                    if ((char*)ptrchecker == ptr){
                        printf("\nError: The pointer that we are attempting to free is already freed.\n");
                        return;
                    }
                    checkfreed --;
                }
                ptrchecker++;

            }else{
                 //iterate to next meta data
                ptrchecker += abs(nextMeta);
                count += abs(nextMeta);
            }
            
        }
    }
    

    // Error Condition B: This error condition accounts for the case where the pointer argument that is passed does not exist within the memory array.
    if (found != true){
        printf("\nError: The pointer passed in the argument was not allocated with malloc.\n");
        return;
    }

    // now go back and combine any unused space 
    // note: after any free, the most unused space that can be combines would be 3 meta datas (freeing space between two freed spaces)
    // other cases (1)/(2) combine current free with freespace after/before
    // ie: the most u will need to combine any data is two times, so check twice
    int i;
    for(i = 0; i < 2 ; i++){

        ptrchecker = myblock + 1;
        char* ptrcheckernext = ptrchecker;
        bool run2 = true;
        count = 1;
        
        
        while (run2 == true && found == true){
            
            //store current meta data size
            memcpy( &nextMeta, ptrchecker, sizeof(nextMeta));
            
            //cant run over limit
            if ((count + abs(nextMeta) + 2 ) == MEM_LENGTH){
                break;
            }
            
            //check if current block free
            if (nextMeta < 0){
                
                //check if next block also free
                ptrcheckernext = ptrchecker;
                ptrcheckernext += abs(nextMeta);
                ptrcheckernext += sizeof(short);
                memcpy( &meta, ptrcheckernext, sizeof(short));
                
                if (meta < 0){
                    //if next metadata is a free block:
                    //copy new size to ptrchecker
                    nextMeta -= (sizeof(short) + abs(meta));
                    memcpy(ptrchecker, &nextMeta, sizeof(short)); 
                    //clear ptrcheckernext
                    memcpy(ptrcheckernext, &zero, sizeof(short)); 
                    //end loop
                    run2 = false;
                } else if (meta > 0){
                    //if next metadata is in use
                    //iterate to next meta data
                    ptrchecker += abs(nextMeta);
                    ptrchecker += sizeof(short);
                    count += abs(nextMeta);
                    count += sizeof(short);
                }

            }else{
                //iterate to next meta data
                ptrchecker += abs(nextMeta);
                ptrchecker += sizeof(short);
                count += abs(nextMeta);
                count += sizeof(short);
                
            }
        }
    }

    return;

}
