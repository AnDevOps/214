#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// checks if the current character is a alphabetical character
bool isLetter(char ch) {
    // uses the ASCII codes to compare and actually check if its a letter
    if( !( ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) ) )   return false;
    return true;
}

// checks if the current character is a numeric character
bool isNumber(char ch) {
    // uses the ASCII codes to compare and actually check if its a number
    if( !( (ch >= '0') && (ch <= '9') )  )  return false;
        return true;
}
// checks if the current character is a operational character
bool isOperator(char ch) {
    // if its not a number of a letter, then it must be a operational character
    if( (isNumber(ch)) || (isLetter(ch)) ) return false;
        return true;
}
// delim check
bool isDelim(char ch) {
    // different white space coniditons
    if(ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\0') return true;
    return false;
}

// Each operator will be assigned a specific flag in which this method will return to identify the current character.
// returns -1 if none of the operators match | this shouldnt ever hit.
int whichOperator(char *str){
    if( strcmp(str,"(") == 0){
        return 1;
    }else if ( strcmp(str,")") == 0){ 
        return 2;
    }else if ( strcmp(str,"[") == 0){ 
        return 3;
    }else if ( strcmp(str,"]") == 0){ 
        return 4;
    }else if ( strcmp(str,".") == 0){ 
        return 5;
    }else if ( strcmp(str,"->") == 0){ 
        return 6;
    }else if ( strcmp(str,"sizeof") == 0){ 
        return 7;
    }else if ( strcmp(str,",") == 0){ 
        return 8;
    }else if ( strcmp(str,"!") == 0){ 
        return 9;
    }else if ( strcmp(str,"~") == 0){ 
        return 10;
    }else if ( strcmp(str,">>") == 0){ 
        return 11;
    }else if ( strcmp(str,"<<") == 0){ 
        return 12;
    }else if ( strcmp(str,"^") == 0){ 
        return 13;
    }else if ( strcmp(str,"|") == 0){ 
        return 14;
    }else if ( strcmp(str,"++") == 0){ 
        return 15;
    }else if ( strcmp(str,"--") == 0){ 
        return 16;
    }else if ( strcmp(str,"+") == 0){ 
        return 17;
    }else if ( strcmp(str,"/") == 0){ 
        return 18;
    }else if ( strcmp(str,"||") == 0){ 
        return 19;
    }else if ( strcmp(str,"&&") == 0){ 
        return 20;
    }else if ( strcmp(str,"?") == 0){ 
        return 21;
    }else if ( strcmp(str,":") == 0){ 
        return 22;
    }else if ( strcmp(str,"==") == 0){ 
        return 23;
    }else if ( strcmp(str,"!=") == 0){ 
        return 24;
    }else if ( strcmp(str,"<") == 0){ 
        return 25;
    }else if ( strcmp(str,">") == 0){ 
        return 26;
    }else if ( strcmp(str,"<=") == 0){ 
        return 27;
    }else if ( strcmp(str,">=") == 0){ 
        return 28;
    }else if ( strcmp(str,"=") == 0){ 
        return 29;
    }else if ( strcmp(str,"+=") == 0){ 
        return 30;
    }else if ( strcmp(str,"-=") == 0){ 
        return 31;
    }else if ( strcmp(str,"*=") == 0){ 
        return 32;
    }else if ( strcmp(str,"/=") == 0){ 
        return 33;
    }else if ( strcmp(str,"%=") == 0){ 
        return 34;
    }else if ( strcmp(str,">>=") == 0){ 
        return 35;
    }else if ( strcmp(str,"<<=") == 0){ 
        return 36;
    }else if ( strcmp(str,"&=") == 0){ 
        return 37;
    }else if ( strcmp(str,"^=") == 0){ 
        return 38;
    }else if ( strcmp(str,"|=") == 0){ 
        return 39;
    }else if ( strcmp(str,"&") == 0){ 
        return 40;
    }else if ( strcmp(str,"-") == 0){ 
        return 41;
    }else if ( strcmp(str,"*") == 0){ 
        return 42;
    }else{
        return -1;
    }
}

// prints the current token based on the flag that we assigned it. | 0: words | 1-42: operators | 43-47: integers
void print(char *hold, int flag) {
    switch(flag) {
        // case for words (i.e abc e743we)
        case 0: 
            printf("word: \"%s\"\n", hold);
            break;

        // 1 - 42 operators 
        case 1:
            printf("left parenthesis: \"%s\"\n", hold);
            break;
        case 2:
            printf("right parenthesis: \"%s\"\n", hold);
            break;
        case 3:
            printf("left bracket: \"%s\"\n", hold);
            break;
        case 4:
            printf("right bracket: \"%s\"\n", hold);
            break;
        case 5:
            printf("structure member: \"%s\"\n", hold);
            break;
        case 6:
            printf("structure pointer: \"%s\"\n", hold);
            break;
        case 7:
            printf("sizeof: \"%s\"\n", hold);
            break;
         case 8:
            printf("comma: \"%s\"\n", hold);
            break;
        case 9:
            printf("negate: \"%s\"\n", hold);
            break;
        case 10:
            printf("1s complement: \"%s\"\n", hold);
            break;
        case 11:
            printf("shift right: \"%s\"\n", hold);
            break;
        case 12:
            printf("shift left: \"%s\"\n", hold);
            break;
        case 13:
            printf("bitwise XOR: \"%s\"\n", hold);
            break;
        case 14:
            printf("bitwise OR: \"%s\"\n", hold);
            break;
        case 15:
            printf("increment: \"%s\"\n", hold);
            break;
        case 16:
            printf("decrement: \"%s\"\n", hold);
            break;
        case 17:
            printf("addition: \"%s\"\n", hold);
            break;
        case 18:
            printf("division: \"%s\"\n", hold);
            break;
        case 19:
            printf("logical OR: \"%s\"\n", hold);
            break;
        case 20:
            printf("logical AND: \"%s\"\n", hold);
            break;
        case 21:
            printf("conditional true: \"%s\"\n", hold);
            break;
        case 22:
            printf("conditional false: \"%s\"\n", hold);
            break;
        case 23:
            printf("equality test: \"%s\"\n", hold);
            break;
        case 24:
            printf("inequality test: \"%s\"\n", hold);
            break;
        case 25:
            printf("less than test: \"%s\"\n", hold);
            break;
        case 26:
            printf("greater than test: \"%s\"\n", hold);
            break;
        case 27:
            printf("less than or equal test: \"%s\"\n", hold);
            break;
        case 28:
            printf("greater than or equal test: \"%s\"\n", hold);
            break;
        case 29:
            printf("assignment: \"%s\"\n", hold);
            break;
        case 30:
            printf("plus equals: \"%s\"\n", hold);
            break;
        case 31:
            printf("minus equals: \"%s\"\n", hold);
            break;
        case 32:
            printf("times equals: \"%s\"\n", hold);
            break;
        case 33:
            printf("divide equals: \"%s\"\n", hold);
            break;
        case 34:
            printf("mod equals \"%s\"\n", hold);
            break;
        case 35:
            printf("shift right equals: \"%s\"\n", hold);
            break;
        case 36:
            printf("shift left equals: \"%s\"\n", hold);
            break;
        case 37:
            printf("bitwise AND equals: \"%s\"\n", hold);
            break;
        case 38:
            printf("bitwise XOR equals: \"%s\"\n", hold);
            break;
        case 39:
            printf("bitwise OR equals: \"%s\"\n", hold);
            break;
        case 40:
            printf("AND/address operator: \"%s\"\n", hold);
            break;
        case 41:
            printf("minus/subtract operator: \"%s\"\n", hold);
            break;
        case 42:
            printf("multiply/dereference operator: \"%s\"\n", hold);
            break;
    
        // cases for numeric values
        case 43: 
            printf("decimal integer: \"%s\"\n", hold);
            break;
        case 44: 
            printf("octal integer: \"%s\"\n", hold);
            break;
        case 45: 
            printf("hexadecimal integer: \"%s\"\n", hold);
            break;
        case 46: 
            printf("floating point: \"%s\"\n", hold);
            break;
        // special case for floating values where it contains an e (i.e 3232.32e-10)
        case 47: 
            printf("floating point:"" \"%s\"\n", hold);
        default: 
            break;
    }
}

int main (int argc, char **argv) { 
    
    // hold is a string used to hold the current token that is to be printed
    char *hold = malloc(sizeof(char) * strlen(argv[1]));

    // last_held is a string that holds the previous string value of hold. This is used for cases such as "+++" where it would be a increment and a addition operator.
    char *last_held = malloc(sizeof(char) * strlen(argv[1]));

    // temp string necessary for string manipulation in certain cases
    char *temp = malloc(sizeof(char) * strlen(argv[1]));

    // the token can be of 4 types: 0 - words, 1 - nums, 2 - ops, -1 - delims
    int hold_type;

    // the current character has 4 types: 0 - words, 1 - nums, 2 - ops, -1 - delims
    int char_type;

    // used to which token we're going to be printing (i.e right bracket... integer... floating value)
    int flag;

    // variables are used for the build sequence of floating points since they contain special exceptions ("." "e" "e-")
    bool reset = false;
    int toBuildExp = 0;
    bool built = false;

    // assume hex if 0 is input so flag tells if actually hex
    bool hasX = false;

    // variable is used to keep track of where the character will be assigned in the token
    int count = 0;
    
    // to loop through every basic character and build the token
    int i;
    for(i = 0; argv[1][i] != '\0'; i++) {
        
        // every character will recieve a type. (letter = 0 | number = 1 | operator = 2)
        if (isLetter(argv[1][i]) ){
                char_type = 0;
        }else if ( isNumber(argv[1][i]) ){
                char_type = 1;
        }else if (isDelim(argv[1][i]) ){
                char_type = -1;
        }else{ //operator
                char_type = 2;
        }    
    


        // General Case 1
        // print case found so print and reset hold

        if( ((whichOperator(last_held) != -1) && (whichOperator(hold) == -1)) && !( whichOperator(last_held) == 7  && ( isLetter(argv[1][i]) || isNumber(argv[1][i]) )  ) ){ 
                // ** case for "+++" needed for operators which are more than size 1
                // ignore sizeof if current char is letter or number
                // we need to determine which operator the token before the current character is added is.
                flag = whichOperator(last_held);

                //print as normal
                print(last_held, flag);

                //removes last_held string from hold
                memmove(temp, hold + strlen(last_held), strlen(hold) - strlen(last_held));
                
                // we want to reset the values of the token  
                free(hold);
                hold = malloc(sizeof(char) * strlen(argv[1]));
                
                // we copy the current contents of temporary into hold (current token)
                strcpy(hold,temp);
                
                // we want to reset temp since it will be used for a different token
                free(temp);
                temp = malloc(sizeof(char) * strlen(argv[1]));
         
               
                //reset the values of last_held but keep hold and update values
                free(last_held);
                last_held = malloc(sizeof(char) * strlen(argv[1]));
                flag = whichOperator(hold);
                count = 1;
        }else if(strlen(hold) > 0){
           
           // this condition handles exceptions to the token which are not allowed. (i.e letters after a number in a numeric-designated token)
           // designated patterns can be found in the ASST0.pdf
           if
            // delimiter case
            (  (char_type == -1) 
            // operational characters after a token type 0 (word)
            || ((hold_type == 0) && (flag == 0) && (isOperator(argv[1][i])))
            // non-numeric cases after an octal integer 
            || (flag == 44) && !(isNumber(argv[1][i])) && (argv[1][i] != '.')
            // operator cases after hex
            || ((flag == 45) && (isOperator(argv[1][i])) && (hasX == true)) || ((flag == 45) && (argv[1][i] == '.') && (hasX == false) && (strcmp(hold, "0") != 0) )
            ) { 

            
                // if the current token is a operator, we need to determine which operator that is so we properly print
                if (hold_type == 2){
                    flag = whichOperator(hold);
                //single 0 is octal
                } else if (hold_type == 1 && strcmp(hold, "0") == 0) { // since we assumed 0 would cause a hexadecimal, we need to beware of this case.
                    flag = 44;
                //sizeof would be recognized as string first
                } else if (strcmp(hold, "sizeof") == 0){
                    flag = 7;
                }

                // corner case when it comes to incomplete hexadecimal integers otherwise we print the token as normal because we encountered a pattern error
                if (strcmp(hold,"0X") == 0){
                    print("0",44);
                    print("X",0);
                } else if (strcmp(hold,"0x") == 0){
                    print("0",44);
                    print("x",0);
                }else{
                    print(hold, flag);
                }

                // since we've printed token, we need to reset the values of token and the before-appended token for the next token that appears should the string not have ended.  
                free(hold);
                hold = malloc(sizeof(char) * strlen(argv[1]));
                free(last_held);
                last_held = malloc(sizeof(char) * strlen(argv[1]));    
                flag = -1;
                count = 0;
                hasX = false;

                }
            }

            
       // Condition 2: This condition is for the assembly of tokens. If the necessary conditions suffice, the character will be appended to the token in which we continue to the next iteration to determine if the next character is appended or not.
        // essentially fill while hold not empty case
        if(strlen(hold) != 0 && char_type != -1) {
            
            // Case 1: the current character type is the same as the token type. We will append the character to the token depending on the value of count (the next empty location in the token).
            if(((hold_type == 0) && ((char_type == 0) || (char_type == 1))) || (hold_type == 1) && (flag == 43) && (char_type == 1) || (hold_type == 2) && (char_type == 2)   ) {
                // we want to save the current token before we append the new character in the event that we need to do modifications on the previous token.
                last_held= strcpy(last_held,hold);

                // the character is appended to the token
                hold[count] = argv[1][i];
                count++;

                //**if input ends without delim
                // Corner Case: In this case, the argument ends without encountering a deliminator or encountering a pattern mismatch in the token.
                // mini case 1 - final checks of hold strings to see what to print out
                if(argv[1][i+1] == '\0') {
                   
                       if( ((whichOperator(last_held) != -1) && (whichOperator(hold) == -1)) && !( whichOperator(last_held) == 7  && ( isLetter(argv[1][i]) || isNumber(argv[1][i]) )  ) ){ 
                        //get flag operator
                        flag = whichOperator(last_held);
                        //print as normal
                        print(last_held, flag);
                        //removes last_held string from hold
                        memmove(temp, hold + strlen(last_held), strlen(hold) - strlen(last_held));
                        strcpy(hold,temp);
                    }
                        
                    // if the current token is a operator, we need to determine which operator that is so we properly print
                    if (hold_type == 2){
                        flag = whichOperator(hold);
                    //single 0 is octal
                    } else if (hold_type == 1 && strcmp(hold, "0") == 0) { // since we assumed 0 would cause a hexadecimal, we need to beware of this case.
                        flag = 44;
                    //sizeof would be recognized as string first
                    } else if (strcmp(hold, "sizeof") == 0){
                        flag = 7;
                    }

                    if (strcmp(hold,"0X") == 0){
                        print("0",44);
                        print("X",0);
                    } else if (strcmp(hold,"0x") == 0){
                        print("0",44);
                        print("x",0);
                    }else{
                        print(hold, flag);
                    }
                    

                    // since we've printed token, we need to reset the values of token and the before-appended token for the next token that appears should the string not have ended.  
                    free(hold);
                    hold = malloc(sizeof(char) * strlen(argv[1]));
                    free(last_held);
                    last_held = malloc(sizeof(char) * strlen(argv[1])); 
                    hasX = false; 
                    }
                continue;
            } 
            
            // In the following conditions, it focuses on creating the build sequence for floating values because they require a special build pattern + unique conditions that need to be fulfilled.

            // (Floating Point) Case 1: A "." is encountered while the current token type are numeric decimal integers. 
            // The decimal integer token will be converted to type floating point and the necessary steps to build the token without interference from encountering the wrong pattern are set up.
            if(hold_type == 1 && argv[1][i] == '.' && ((flag == 43 || flag == 44) || ((flag == 45) && (strcmp(hold, "0") == 0)))) {
               
                // In this case, we want to check if there's actually a character in the next iteration or not.
                // If there is no character, then it is not a floating point. If there is a character, we want to check if its numeric.
                if(isNumber(argv[1][i+1])  && (argv[1][i+1] != '\0')) {
                    // flag for floating point
                    flag = 46;
                    // unique build sequence for floating points which bypasses the exceptions due to the various corner cases
                    toBuildExp = 2; 
                // This condition establishes that the next character was either not a number or it was the end of the argument.
                // Thus we print the current token and we call the reset which resets all the token setups.
                } else if(!(isNumber(argv[1][i+1])) || (argv[1][i] == '\0')){
                    print(hold, flag);
                    reset = true;
                }
            
            // (Floating Point) Case 2: A "e" is encountered while the current token type is of type floating point.
            } else if((hold_type == 1) && (argv[1][i] == 'e') && (flag == 46)) {
                
                // In this list of conditions, we want to determine if the proper conditions for an exponent floating point to occur (i.e 2.0e10, 2.0e-10)
                if((argv[1][i+1] == '\0')
                || (isLetter(argv[1][i+1])) 
                || ((isOperator(argv[1][i+1]) && (argv[1][i+1] != '-')))  
                || ((argv[1][i+1] == '-') && !(isNumber(argv[1][i+2])))
                || ((argv[1][i+1] == '-') && (argv[1][i+2] == '\0'))) {
                    print(hold, flag);
                    reset = true;
                // unique build sequence where you check if the form (#.#e#-#) is available. (i.e 2.0e-10)
                } else if( (argv[1][i+1] == '-') && (isNumber(argv[1][i+2])) ) {
                    // flag 47 is the flag for floating point with exponents
                    flag = 47;

                    // tells the buildCondition that you can append 3 times without checking the pattern or delims. 3 is (print "e", "-", and the numeric value after "-")
                    toBuildExp= 3;

                    // this flag tells us when the floating points are set up or not.
                    built = false;
                // This is the normal build Condition for floating points with exponents.
                } else if(isNumber(argv[1][i+1])) {
                    flag = 47;
                    toBuildExp = 2;
                    built = false;
                }
            }
            
            // Special Build Sequence for Floating Points
            // Since floating points have unique conditions, we need to have a seperate build condition required for it.
            if(toBuildExp > 0 || ((flag == 46 || flag == 47) && (isNumber(argv[1][i])))) {
                // We append in the character like normal.
                hold[count] = argv[1][i];
                count++;

                // We keep track of how many times we need to go through with the build condition before we need to start checking the pattern / delims again.
                toBuildExp --;
                if(toBuildExp == 0) built = true;
    
                if(argv[1][i+1] == '\0') {}
                continue;
            } 

            // resets the values and accounts for edge cases
            if(reset) {
                free(hold);
                hold = malloc(sizeof(char) * strlen(argv[1]));
                count = 0;
                hasX = false;
                hold[count] = argv[1][i];
                hold_type = char_type;
                if(isLetter(argv[1][i])) flag = 0;
                if(char_type == 2) flag = whichOperator(hold);
                count++;
                reset = false;

                if(argv[1][i+1] == '\0') {
                    if(char_type == 2) flag = whichOperator(hold);
                    print(hold, flag);
                    free(hold);
                    hold = malloc(sizeof(char) * strlen(argv[1]));
                    
                    continue;
                }

                continue;

            }
        
            // if 0 was passed in as first char of a token- check next char for possible: octal(44), hex(45), or dec(43)
            if (flag == 45 && count == 1){
                if ( (argv[1][i] >= '0') && (argv[1][i] <= '7') ){
                    //octal accepts any numbers after inital 0: 0-1
                    flag = 44; 
                }else if ( (argv[1][i] == 'x') ||  (argv[1][i] == 'X') ){
                    //hex next char has to be X or x
                    hasX = true;
                    flag = 45;
                }else if ( (argv[1][i] == '8') ||  (argv[1][i] == '9') ){
                    //decimal if following char is 8 or 9 because that breaks the pattern for octal
                    flag = 43;              
                }
                hold[count] = argv[1][i];
                count++;
                continue;
            }

            //special cases for octal/hex after the first nonzero
            if (flag == 44 && count > 1){
                //if octal can only recieve numbers <= 7 to continue octal pattern/otherwise it becomes decimal num
                if ( (argv[1][i] == '8') ||  (argv[1][i] == '9') ){
                    flag = 43;
                }
                hold[count] = argv[1][i];
                count++;
                continue;
            }else if (flag == 45 && count > 1){
                //if hex pattern has been established (0x or 0X), the next chars can only be 0-9 or letters A-F
                //if its just 0x or 0X print as two separate tokens
                

                if( ( ((argv[1][i] >= 'g') && (argv[1][i] <= 'z')) || ((argv[1][i] >= 'G') && (argv[1][i] <= 'Z')) ) ){
                 
                    //(**if g-z encountered): print octal and reset hold to single letter (g-z)
                    print(hold,45);
                    free(hold);
                    hold = malloc(sizeof(char) * strlen(argv[1]));
                    flag = 0;
                    hold_type = 0;
                    count = 0;

                }
                hold[count] = argv[1][i];
                count++;
                continue;
            }
            
            
        }

        // condition 1a:
        /*
        This is condition will mostly be used to print the token when it either counters a delimiter or the types of the token and the current character mismatch.
        The current token (hold) will be printed to which then the values of the current token (hold) will be reset.
        It only does something if the current token is not empty.
        */
       
       //general case 1
       // ** print cases for after float built
       //OTHER PRINT CASES *need to be done after hold modification
       
       
         if(strlen(hold) > 0){

            // non-operational characters after a token type 2 (operators)
           if( ((hold_type == 2) && !(isOperator(argv[1][i])))
            // non-numeric cases after a decimal integer 
            || ((hold_type == 1) && !(isNumber(argv[1][i])) && (flag == 43))
            // floating cases:
            || ((flag == 46 || flag == 47) && (built == true) && !(isNumber(argv[1][i])))) { 
                if (hold_type == 2) flag = whichOperator(hold);
                print(hold, flag);
                free(hold);
                hold = malloc(sizeof(char) * strlen(argv[1]));
                free(last_held);
                last_held = malloc(sizeof(char) * strlen(argv[1]));
                flag = -1;
                count = 0;
            }

            
        }

        // Condition 3:
        /*
        This condition will be the first condition should the token be empty. It will append in the current character at the first slot and then determine the possible flag values of the token.
        */
        // FILL HOLD (EMPTY)

        if(strlen(hold) == 0 && char_type != -1) {
            
            if (isLetter(argv[1][i]) ){
                flag = 0;
                hold_type = 0;
            }else if ( isNumber(argv[1][i]) ){
                //check if 0 for octal/hex case
                if ((argv[1][i]) == '0'){
                    hold_type = 1;
                    flag = 45; //assume hex
                    
                }else{
                    hold_type = 1;
                    flag = 43; //otherwise set to dec
                }
            }else if (isOperator(argv[1][i]) ){
                //operator
                flag = 1;
                hold_type = 2;
            }
    
            //asigns the first value and moves on (count starts at 0 and after this is 1)
            hold[0] = argv[1][i];
            count++;

        }  
    }

    //if input ends without delim and hold still has char
         if (strlen(hold) > 0) {
            
            if (hold_type == 2){
                flag = whichOperator(hold);
            } else if(hold_type == 1 && strcmp(hold, "0") == 0) { // since we assumed 0 would cause a hexadecimal, we need to beware of this case.
                flag = 44;
            }
            
             if (strcmp(hold,"0X") == 0){
                        print("0",44);
                        print("X",0);
                    } else if (strcmp(hold,"0x") == 0){
                        print("0",44);
                        print("x",0);
                    }else{
                        print(hold, flag);
                    }
            
            
        }
}
