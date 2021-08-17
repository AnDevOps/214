CS214 - Assignment 1

Project Description: 
In this project, the goal was to create our own implementation of malloc and free called mymalloc() and myfree() and we used a char array of size 4096 bytes to emulate main memory.
This was approached through a modular perspective we seperarated the states of how the pointer to the main memory emulator will operate. In addition to this, to emulate the different memory locations that can exist within the myblock array,
we created metadata of type short. 

In the case of mymalloc(), there were 2 different main cases: case that checks if it is the first time malloc has been carried (i.e myblock[] is empty) and case that inserts based on the fact that main memory is not empty.

Implementation
In order to emulate the metadata which is responsible for the corresponding memory allocation, we implemented metadata as a short. The length of a short type is 2 bytes and since it is a signed one, it represents the range of values from -32768 to 32767.
This is important because not only does the length of the emulated memory fit inside that range but we can use the positive and negative of the value to determine if it is being used or not.
We also decided that implementing the meta data in this way would be the most space efficient way of doing so. With only two bytes per metadata block, we can use more of the memory for actual mallocated data,
rather than using it all on the metadata to keep track of the mallocated data.
- Positive Values meant that the metadata and its correspondent memory was current in used.
- Negative Values meant that the metadata and its correspondent memory was not in use.
- the maximum size of a single malloc with our code is 4093 bytes

Generally mymalloc works by first determining if the memory array has been altered(if malloc has already been called), if it has, it will insert a meta data that allocated enough memory for the user's size.
From here it will insert its first meta data if it needs to. From then on, when malloc is called, it will traverse through the meta datas and insert metadatas and create new metadatas where fitting.

Generally myfree works by first checking the input pointer and determining if it is suitable and if there is even any malloc'd data to free. From here if the pointer is suitable for compares, myfree
will traverse through the metadata in memory and locate the pointer in the memory array. If found, myfree will do necessary freeing and necessary merging of free blocks. If not found, myfree will return an error

*Merging free datacan happen in one of 3 ways:
 (1) merging currently freed data with free block before it
 (2) merging currently freed data with free block after it
 (3) merging currently freed data with free blocks before and after it
-- we see that at in any case we need to merge at maximum three blocks together(in case 3), this is two merges
--in free, we coded the single merge to work for any two adjacent freeblocks, and simply ran it to scan the memory array twice.


In the case of malloc(): When the pointer argument is passed through mymalloc(), the code works by first determining if myblock[] is empty or not. This corresponds with whether mymalloc has ever been called in the life span of the program yet. Since global variables
were a restriction, we implemented firstMalloc() and we reserved the first byte of the program as a flag that determined if the mymalloc has ever been called on the emulated memory.

There were a series of subconditions (i.e possible edge cases / error conditions) that we had to account for such as 
(1) when allocating space for a requested pointer, if there were 1-2 bytes left over in the free space, it would just get appended because the leftover space is either (A) not large enough for a metadata (B) large enough for metadata but has no space for data afterwards
(2) when searching for the appropriate metadata, if the pointer goes beyond the scope of myblock[] (i.e an unused metadata that can house the requested allocation is not found), it would return an error.(saturated data error)
(3) when establishing a metadata for a new section of available memory locations, we had to account for the fact that 2 bytes were being used for the metadata.

The code had to also account for such errors:
A: Free()ing addresses that are not pointers:
B: Free()ing pointers that were not allocated by malloc():
C: Redundant free()ing of the same pointer:
D: Saturation of dynamic memory:




The process of what a mymalloc() function call would look like is:
mymalloc(x):
-> determines if mymalloc() has ever been called by checking the first byte
--> if it hasn't been called and the size of the malloc request is valid, insert in the positive metadata value, move the pointer past the metadata and free space, then insert a new negative metadata whose value is equal to -(4096 - x)

-> if mymalloc() has been called before, we begin to search the myblock[] for the valid metadata that can house the requested allocation
..


The process of what a myfree() function call would look like is:
myfree(*x):
-> determines if mymalloc() has ever been called by checking the first byte
-> determines if pointer is not null(accessible)
--> if either of these fail then myfree results in an error

-> if mymalloc() has been called before and we have allocated data, we traverse myblock[] for the valid metadata that needs to be freed
--> if found, do necessary freeing and merging
--> if not found return an error
..
