#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <libgen.h>
#include <math.h>

// struct (LL) for single data structure
typedef struct single_direct_structure {
    // variable that we used to match against ASCII text
    char ch[250];
    // frequency that it occurs
    double frequency;
    // pointer to the next struct
    struct single_direct_structure *next;
} single_token_node;

// struct (LL) for shared data structures
typedef struct shared_data_structure {
    // struct (LL) containing a directory and the tokens
    single_token_node *data;
    // pointer to the next struct
    struct shared_data_structure *next;
} shared_data;

// struct for arguments
typedef struct directory_arguments {
    // directory name
    char *path;
    // mutex - used for file-handling accessing the shared - data structure
    pthread_mutex_t * file_lock;
    // shared data-structure
    shared_data * shared_info;
} arguments;

int direct_open(char *path) {
    // directory
    DIR * path_dir = opendir(path);

    // determining is the path exists & can be opened
    if(path_dir == NULL) {
        printf("The requested directory is not accessible at this time. | %s\n", path);
        return 1;
    } else {
        closedir(path_dir);
        return 0;
    }

    return 1;
}
//tokenizer methods/node helper methods
//takes in a file and used fopen to read char by char and returns linked list data structure with corresponding tokens and their frequencies in alphabetical order
// *** still need to add in part where the headnode is the filename:total#ofTokens - rn it just returns the tokens but we need to decide how to format that

//node constructor
single_token_node* createNode(char* istring, double ifrequency, single_token_node* inext) 
{ 
  single_token_node* p = malloc(sizeof(single_token_node));
  strcpy(p->ch, istring);
  p->frequency = ifrequency;
  p->next = inext;
  return p;
}


//insert token node at end of info(shared_data) LL
shared_data * insertTokenNode(single_token_node * single_token, shared_data * shared_data_root){
    shared_data* new_shared_data = malloc(sizeof(shared_data));
    new_shared_data->data = single_token; 
    new_shared_data->next = NULL;

    if (shared_data_root == NULL) return new_shared_data;
    shared_data* ptr = shared_data_root;

    while (ptr->next != NULL){
        ptr = ptr->next;
    }
    ptr->next = new_shared_data;


    return shared_data_root;
}
//insert node at front of 
single_token_node * insertNode(single_token_node * single_token, single_token_node * root) {
    single_token_node * toReturn = (single_token_node *) malloc(sizeof(single_token_node));
    strcpy(toReturn->ch, single_token->ch);
    toReturn->frequency = single_token->frequency;
    toReturn->next = NULL;


    if(root == NULL) return toReturn;
    
    toReturn->next = root;
    return toReturn;
}

 //insert node alphabetically
single_token_node* alphaInsertNode(char* string, double frequency, single_token_node* front) 

{ 
   single_token_node* insertNode = createNode(string,frequency,NULL);
   single_token_node* prev = front;
   single_token_node* ptr = front;
   int inserted = 0;

    //check empty
    if(front == NULL){
        return insertNode;
    }

    //check first
    if (strcmp(string, ptr->ch) < 0){
        insertNode->next = front;
        return insertNode;
    }

    //else iterate thru
    for (ptr = front; ptr != NULL; ptr = ptr->next){
        if (strcmp(string, ptr->ch) < 0){
            insertNode->next = ptr;
            prev->next = insertNode;
            inserted = 1;
            break;
        }
        prev = ptr;
    }

    //if wasnt found insert at end
    if (inserted == 0){
        prev->next = insertNode;
    }

    return front;
}

// checks if the current character is a upper case - should use lowercase if it is
 bool isUpperCLetter(char ch) {
    // uses the ASCII codes to compare and actually check if its a letter
    if ( (ch >= 'A') && (ch <= 'Z') ){
         return true;
    }else{
         return false;
    }
   
}

// delim check (only whitespaces and newlines)
 bool isDelim(char ch) {
    // different white space coniditons
    if(ch == ' ' || ch == '\n') return true;
    return false;
}

//tokenizer method
void * tokenizeFile(void * thread_argument_struct) {

    int max_str_size = 250;
    int max_arr_size = 30000;
    int hold_index = 0;
    int match_found = 0;
    char current_char = ' ';
    double frequency[max_arr_size];
    int numTokens = 0;
    int totalnumTokens = 0;

    arguments * info = (arguments *) thread_argument_struct;
    char *fileName = info->path;

    FILE *file = fopen(fileName, "r");
    int c;
    
    if (file == NULL) return NULL; //could not open file
    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);


     //max size for each array = length of txtfile
    //this is also the max size of each token
    //char arr[NUMBER_OF_STRING][MAX_STRING_SIZE]
    //IF THESE GET TOO BIG ====> SEG FAULT
    
  

    char tokens[max_arr_size][max_str_size];
    for (int i = 0; i < max_arr_size; i++){
        strcpy(tokens[i],"");
    }

    // hold is a string used to hold the current token that is to be printed
    char *hold = malloc(sizeof(char) * max_arr_size);

    while ((c = fgetc(file)) != EOF) {
        //reset
        match_found = 0;
        //save current char
        current_char = c;

        //if delim is encountered and hold is holding chars
        // - if token array is empty then make it first token
        // - if token array has items check current string hold to all items in array
        // -- if match found iterate corresponding frequency count
        // -- if no match found create new token
        if (isDelim(current_char)){ 
    
            if ((numTokens == 0) && (strlen(hold) != 0)){
                strcpy(tokens[0], hold);
                frequency[0] = 1;
                numTokens ++;
            
            }else if ((numTokens != 0) && (strlen(hold) != 0)){
                for (int i = 0; i < numTokens; i++){
                    if (strcmp(tokens[i],hold) == 0){
                        frequency[i] ++;
                        match_found = 1;
                    }
                }
                if (match_found == 0){
                    strcpy(tokens[numTokens ],hold);
                    frequency[numTokens] = 1;
                    numTokens ++;
            
                }
            }
            // reset the values of the token  
                memset(hold, 0, sizeof(char) * max_arr_size);
                hold_index = 0;

        //else (if not delim) add char to hold
        }else{
            if (isUpperCLetter) current_char = tolower(current_char);
            hold[hold_index] = current_char;
            hold_index ++;
        }
    }

    //if hold isnt empty do last hold_string check
    if (strlen(hold) != 0){
           for (int i = 0; i < numTokens; i++){
                    if (strcmp(tokens[i],hold) == 0){
                        frequency[i] ++;
                        match_found = 1;
                    }
                }
                if (match_found == 0){
                    strcpy(tokens[numTokens ],hold);
                    frequency[numTokens] = 1;
                    numTokens ++;
            
                }
    }
    
    //get total number of tokens that occured in file
    for (int i = 0; i < numTokens; i++){
       
        totalnumTokens += frequency[i];
    }
    
    //create return node structure
    single_token_node* front = NULL;
    for (int i = 0; i < numTokens; i++){
        front = alphaInsertNode(tokens[i],frequency[i]/totalnumTokens,front);
    }

    //head structure with name of file and total # of tokens
    single_token_node* head = createNode(fileName, totalnumTokens,front);

    pthread_mutex_lock(info->file_lock);

    insertTokenNode(head,info->shared_info);
    pthread_mutex_unlock(info->file_lock);

    //UNLOCK MUTEX
}

///end of tokenizer////////////////////////////////////////////////////////////////////////////

//insert node at end of 
single_token_node * insertNodeEnd(single_token_node * single_token, single_token_node * root) {
    single_token_node * toReturn = createNode(single_token->ch, single_token->frequency, NULL);

    if(root == NULL) return toReturn;
    
    single_token_node* ptr = root;
    while (ptr->next!=NULL){
        ptr = ptr->next;
    }
    ptr->next = toReturn;
    return root;
}

int recurseSubDirectories(char *path) {
    // insert code

    return 2;
}

char * appendSubDirectories(char * path, char * old_path, char * subdirect) { 
    
    if(old_path[strlen(old_path) - 1] != '/') strcat(path, "/");
    strcat(path, old_path); strcat(path, subdirect);

    return path;
}

void file_handle() {
    printf("-\n");
}

void * handle_directory (void *thread_argument_struct) {
    pthread_t * thread_pointer = (pthread_t *) malloc(sizeof(pthread_t) * 10);
    int threadCount = 1;

    // used for finding the sub-directories
    char * new_path;
    arguments * subdirect;
    
    // cast the void * pointer as a struct pointer
    arguments * info = (arguments *) thread_argument_struct;

   //////////////// printf("Path: %s \n", info->path);///////////////////

    // precaution: determines if the directory can be open-able
    direct_open(info->path);

    // needed to open the current working directory
    DIR * open_directory = opendir (info->path);
    struct dirent * dp;

    while ((dp=readdir(open_directory)) != NULL) {
            // if the type is neither a directory or a regular file, we ignore.
            if(dp->d_type != DT_DIR && dp->d_type != DT_REG) continue;

            if(dp->d_type == DT_DIR && (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)) continue;

            if(info->path[strlen(info->path) - 1] != '/') {
                new_path = malloc((strlen(info->path) + strlen(dp->d_name) + strlen("/") + 1));
                strcpy(new_path, info->path);
                strcat(new_path, "/");
            } else {
                new_path = malloc((strlen(info->path) + strlen(dp->d_name) + 1));
                strcpy(new_path, info->path);
            }

            strcat(new_path, dp->d_name); 

            if(dp->d_type == DT_REG || dp->d_type == DT_DIR) {
                // create new argument struct that will contain the new path 

                subdirect = (arguments *) malloc(sizeof(arguments));
                subdirect->path = new_path;
                subdirect->file_lock = info->file_lock;
                subdirect->shared_info = info->shared_info;

                if(dp->d_type == DT_REG) pthread_create(&thread_pointer[threadCount], NULL, &tokenizeFile, (void *) subdirect);
                if(dp->d_type == DT_DIR) pthread_create(&thread_pointer[threadCount], NULL, &handle_directory, (void *) subdirect);
                
                threadCount++;
                int size = sizeof(thread_pointer) / sizeof(thread_pointer[0]);
                if(threadCount >= size) thread_pointer = realloc(thread_pointer, sizeof(pthread_t) * size + sizeof(pthread_t) * 10);
                continue;
            }
    }

    // joins the threads
    int i = 0;
    while (i < threadCount) {
        pthread_join(thread_pointer[i], NULL);
        i++;
    }

    closedir(open_directory);

    free(thread_pointer);


}

//helper method to determine amt of files that are in LL
int countfiles(shared_data* input)
{
    int count = 0;
    shared_data* pass_ptr_ptr = input;

    while (pass_ptr_ptr != NULL){
        count++;
        pass_ptr_ptr = pass_ptr_ptr->next;
    }
    return count;
}



//helper method returns mean construction
single_token_node* mean_construction(single_token_node* file1, single_token_node* file2 ){
    single_token_node* file1ptr = file1->next;
    single_token_node* file2ptr = file2->next;//ignore the start directory paths
    single_token_node* first = createNode(file1ptr->ch,file1ptr->frequency,NULL);

    int found = 0;
    
    for (file1ptr = file1->next->next; file1ptr != NULL; file1ptr = file1ptr->next){
        single_token_node* temp = createNode(file1ptr->ch,file1ptr->frequency,NULL);
        first = insertNodeEnd(temp,first);
    }

    single_token_node* ptr = first;

    for ( file2ptr = file2->next; file2ptr != NULL; file2ptr = file2ptr->next){
        found = 0;
        for (ptr = first; ptr != NULL; ptr = ptr->next){
            if (strcmp(ptr->ch,file2ptr->ch) == 0){
                ptr->frequency = (ptr->frequency + file2ptr->frequency)/2;
                found = 1;
                break;
            }
            
        }
        if (found == 0){
            single_token_node* temp = createNode(file2ptr->ch,(file2ptr->frequency/2.0),NULL);
            first = insertNodeEnd(temp,first);
        }
        
    }

    // for (ptr = first; ptr != NULL; ptr = ptr->next){
    //        printf("%s:%f\n", ptr->ch, ptr->frequency);
    // }
    
    return first;
}


void bubbleSort(single_token_node *token_array[], int numFiles) 
{ 
   int i, j; 
   int swapped = 0;
   for (i = 0; i < numFiles; i++) 
   { 
     swapped = 0; 
     for (j = 0; j < numFiles-i-1; j++) 
     { 
        if(token_array[j]->frequency < token_array[j+1]->frequency) 
        { 
           single_token_node * temp = token_array[j];
           token_array[j] = token_array[j+1];
           token_array[j+1] = temp;
            
            swapped = 1; 
        } 
     } 
  
     // IF no two elements were swapped by inner loop, then break 
     if (swapped == false) 
        break; 
   } 
} 
int numUniqueTokens (single_token_node* input){
    int returnval = 0;
    single_token_node* ptr = input;

    while (ptr != NULL){
        returnval ++;
        ptr = ptr->next;
    }
    return returnval;
}


double calculatejsd (single_token_node* mean_construct,single_token_node* file1, single_token_node* file2 ){
    double firstKLD = 0;
    double secondKLD = 0;
    double total = 0;
    double logBoi = 0;
    
    for (single_token_node* file1ptr = file1->next; file1ptr != NULL; file1ptr = file1ptr->next){
        firstKLD = file1ptr->frequency;
       for( single_token_node* ptr = mean_construct; ptr != NULL; ptr = ptr->next){
           if (strcmp(file1ptr->ch,ptr->ch) == 0){
               
               logBoi = (file1ptr->frequency/ptr->frequency);
               break;
           }
       }
       logBoi = log10(logBoi);
       total += firstKLD*logBoi;
    }
    

    for (single_token_node*  file2ptr = file2->next; file2ptr != NULL; file2ptr = file2ptr->next){
           secondKLD = file2ptr->frequency;
       for( single_token_node* ptr = mean_construct; ptr != NULL; ptr = ptr->next){
           if (strcmp(file2ptr->ch,ptr->ch) == 0){
               
               logBoi = (file2ptr->frequency/ptr->frequency);
               break;
           }
       }
       
       logBoi = log10(logBoi);
       total += secondKLD*logBoi;
    }

    return total/2;
}

char* getColor (double number){
    if (number < 0.1) {
        return "\033[0;31m";
    }else if (number < 0.15){ 
        return "\033[0;33m";
    }else if (number < 0.2){ 
        return "\033[0;32m";
    }else if (number < 0.25){ 
        return "\033[0;36m";
    }else if (number < 0.3){
        return "\033[0;34m";
    }
    else {
        return "\033[0m";
    }
}

int main(int argc, char **argv) {
	// determines if the argv[1] is a valid directory --> perhaps we have a method that does it's own file checks
    if(direct_open(argv[1]) == 1) return EXIT_SUCCESS;

   shared_data * shared_pointer = (shared_data *) malloc(sizeof(shared_data));
   shared_pointer->data = NULL;
   shared_pointer->next = NULL;

    // init & allocate a mutex for the file handling
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

    // init & allocate a struct for arguments that we can pass to the parameters
    // argv[1] - the initial directory that we start from (base)
    // mut - the mutex used to lock pthreads
    // shared - the shared datastructure 
    // current_thread - keeps track of the thread id
    // threadList_pointer - points to the thread array... used as threadList_pointer[current_thread]
    arguments * pass_pointer = (arguments *) malloc(sizeof(arguments));
    pass_pointer->path = argv[1];
    pass_pointer->file_lock = &mut;
    pass_pointer->shared_info = shared_pointer;

    // main() should call handle_directory(arguments)
    handle_directory((void *) pass_pointer);

    shared_data* pass_ptr_ptr = shared_pointer->next;
    //single_token_node* = pass_ptr_ptr->data;

    //check num of files
    int numFiles = countfiles(pass_ptr_ptr);

    //from this we need to compute how many comparisons we have to do/
    //this is just each one compares to the other ones after it int the array

    //create array of token pointers
    single_token_node *token_array[numFiles];

    //fill in pointers
    for (int i = 0; i<numFiles; i++){
        token_array[i] = pass_ptr_ptr->data;
        pass_ptr_ptr = pass_ptr_ptr->next;
    }

    //reorder in order
    bubbleSort(token_array,numFiles);

    //just for testing
    // for (int i = 0; i<numFiles; i++){
    //     for (single_token_node* ptr = token_array[i]; ptr != NULL; ptr = ptr->next){
    //          printf("%s:%f\n", ptr->ch, ptr->frequency);
    //     }
    // }

    ////////////////////////////////////////////////
   

    single_token_node* mean_construct = malloc(sizeof(single_token_node)*100);
    int file1tokens = 0;
    int file2tokens = 0;
    double answer = 0;
    
    for (int compares = 0; compares < numFiles; compares++){
       // printf("           %d             \n ", compares);
        for (int compareTo = compares + 1 ; compareTo < numFiles; compareTo++){
            file1tokens = numUniqueTokens(token_array[compares]);
            //printf("%d : ",file1tokens);
            file2tokens = numUniqueTokens(token_array[compareTo]);
            //printf("%d\n",file2tokens);
            if (file1tokens < file2tokens){
                mean_construct = mean_construction(token_array[compares],token_array[compareTo]);
                answer = calculatejsd(mean_construct,token_array[compares],token_array[compareTo]);
                printf("%s",getColor(answer));
                printf("%f ",answer);
                printf("\033[0m");
                printf("%s and %s\n",token_array[compares]->ch,token_array[compareTo]->ch);
                mean_construct = malloc(sizeof(single_token_node));
            }else{
                mean_construct = mean_construction(token_array[compareTo],token_array[compares]);
                calculatejsd(mean_construct,token_array[compareTo],token_array[compares]);
                answer = calculatejsd(mean_construct,token_array[compareTo],token_array[compares]);
                printf("%s",getColor(answer));
                printf("%f ",answer);
                printf("\033[0m");
                printf("%s and %s",token_array[compares]->ch,token_array[compareTo]->ch);
                mean_construct = malloc(sizeof(single_token_node));
            }
        }
    }
   

	return 0;
}
