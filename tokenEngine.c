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

//main func
int main(){

    //The instrcution code example
    char *str = "LI R31,0";

    //The register number
    int number = 0;

    size_t len = sizeof(str);

    /*
    * Loop that iterates each char and checks if it's a digit 
    * which then calls the function stoi (check stoi documentation)
    * resulting on fetching the exact register number and putting it
    * in the variable 'number'.
    */
    for(size_t i = 0; i < len; i++){

        //Checks if char of str at index i is a digit
        if(isdigit(str[i])){

            //We override the number value to the new result returned from 
            //stoi function
            number = stoi(number,str[i]);

        }

        //Stop when u see that reach the ',' meaning you got the register number
        if(str[i] == ','){

            break;

        }
    }
    
    printf("%d\n",number);
    
    return 0;
}