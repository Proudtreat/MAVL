/*
*   This program is a personal tokenizing engine for the MAVL language it is a simple engine for specific needs.
*/

//The needed libraries
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

//Converts from a string/char to it's equivalent integer value
int stoi(int a,const char s){

    //multiplies the number by 10
    a*=10;

    a+= s-'0';

    return a;

}

int get_register(char** s){

    //The register number
    int number = 0;
    int count = 1;
    size_t len = sizeof(s);


    /*
    * Loop that iterates each char and checks if it's a digit 
    * which then calls the function stoi (check stoi documentation)
    * resulting on fetching the exact register number and putting it
    * in the variable 'number'.
    */
    for(size_t i = 0; i < len; i++){

        //Checks if char of str at index i is a digit
        if(isdigit((*s)[i])){

            //We override the number value to the new result returned from 
            //stoi function
            number = stoi(number,(*s)[i]);

        }
        count++;
        //Stop when u see that reach the ',' meaning you got the register number
        if((*s)[i] == ','){

            break;

        }
    }
    
    //Now we skip the chars that were used by the function so we
    //Shorten the string and by this faster operations.
    *s+=count;

    return number;
}

//Extracts the instruction string and returns it based on the instruction line given.
char* get_instrcution(char** s){
    
    //A count that'll be incremented and used in 2 places
    //1-Using it as an end value for the loop.
    //2-Using it as a skip value for the pointer pointer s.
    int count = 1;

    //Checks if the first char is an empty value or a space.
    if((*s)[0] == ' '){

        //skips it
        *s++;

    }

    //In this loop the count is to be incremented for the total value of the chars of the
    //instruction we have.
    for(size_t i = 0; i < sizeof(s);i++){

        //Checks if the value of the current char s is referencing is a space whatever the kind
        if((*s)[i] == ' ' || (*s)[i] == '\t' || (*s)[i] == '\0'){

            //it exits the loop when a space is found it exits the loop
            break;

        }else{//else it increases the count

        count++;
        }

    }

    //printf("%d",count);
    char command[count];

    //In this loop chars that tell the location of the instruction
    //are called and the values are copied to a seperate array.
    for(size_t i = 0; i < count; i++){

        command[i] = (*s)[i];
        

    }

    //Now we skip the chars that were used by the function so we
    //Shorten the string and by this faster operations.
    *s+=count;

    //now since we can't return an array in c we have to make it a char*
    char* ptr = command;

    return ptr;

}

//String to opcpde it takes the instruction string and extracts the 
//opcode from it based on a predefined instruction -> opcode table.
int stoo(char* s){

    

}

//main func
int main(void){

    /*A mere test*/
    char* s = "ADD R7,R12";

    printf("Instruction:%s\n", get_instrcution(&s));
    printf("Register 1:%d\n",get_register(&s));
    printf("Register 2:%d\n",get_register(&s));
    
    return 0;
}
