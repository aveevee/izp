/*
   * IZP Project 1 - "Prace s textem"
   * Autor: Urmanbetov Aidan (xurman00)
   * Date: 30.10.2022
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


/*Function to control correct input to stdin, you can only enter numbers from 0 to 9.
 *The function returns true or false values. If you enter a number from 0 to 9,
 *the function returns true, otherwise false
 */  
bool correct_input(char argv[]){
    size_t i = 0;
    while(i < strlen(argv)){
        if (argv[i] < '0' || argv[i] > '9')
        {
            return true;
        }   
        i++;
    }
    return false;
}

//A function that converts letters into numbers, according to the T9 search 
char contacttonum(char name) {
    char con_to_num = ' ';
    if (name == ' '){   //condition for ignoring spaces when converting
        return con_to_num;
    }    
    /*an array numbers with the values of letters in the form of numbers, 
     *where then, using mathematical operations, all letters are converted to lowercase 
     *and the first value of the letter is subtracted 
     *(the letter "a" whose value according to the ASCII table is 97) 
     *and the index of the numbers array is returned
     */
    char numbers[26] = {'2', '2', '2', '3', '3', '3', '4', '4', '4', '5', '5', '5', '6', '6', '6', '7', '7', '7', '7', '8', '8', '8', '9', '9', '9', '9'};
    con_to_num = numbers[(tolower(name)) - 97]; //The tolower() function is defined in the ctype.h header file.
    return con_to_num;
}

int main(int argc, char *argv[]) {
    char name[102]; 
    char number[102];
    bool not_found = true;
    
    /*Condition for checking the number of arguments in the input and for checking the characters.
     * In case of more than 2, the program throws an error message.
     * If we have characters other than numbers 0-9, the program throws an error message.
     */
    if(argc > 2){
        fprintf(stderr, "Enter, please, only 1 argument!\n");
        return 1;
    } else if(argc > 1 && correct_input(argv[1])){
        fprintf(stderr, "Incorrect input! Enter numbers only!\n");
        return 1;
    }
    //a loop that writes the names of contacts to the "name" array
    while(fgets(name, 102, stdin) != NULL){
        //a loop that checks '\n' in a string and replaces it with '\0' (fgets loads line with \n)
        for(int i = 0; i < 102; i++){
            if(name[i] == '\n'){
                name[i] = '\0';
            }
        }
        //loading contact's number
        if(fgets(number, 102, stdin)==NULL){
            return 0;
        }
        //a loop that checks '\n' in a string and replaces it with '\0' (fgets loads line with \n)
        for(int i = 0; i < 102; i++){
            if(number[i] == '\n'){
                number[i] = '\0';
            }
        }
        //a condition that prints all contacts, in case if the program is running without arguments
        if(argc == 1){
            fprintf(stdout, "%s, %s\n", name, number);
            not_found = false;
        }
        //a condition in which contacts are searched by name or phone number 
        if (argc == 2){
            char name_num[strlen(name)]; //declaring an array to which contacts converted to numbers will be written
            char zero_is_plus[strlen(number)]; //declaring an array that will convert plus to zero
            strcpy(zero_is_plus, number);
            //a condition that converts plus to zero
            if(zero_is_plus[0] == '+'){
                zero_is_plus[0] = '0';
            }
            //a loop that converts contact names to numbers according to T9 search
            for (size_t i = 0; i < strlen(name); i++){
                name_num[i] = contacttonum(name[i]);
            }
            //a condition that searches for contacts by name
            if (strstr(name_num, argv[1]) != NULL){
                fprintf(stdout, "%s, %s\n", name, number);
                not_found = false;
            //a condition that searches for contacts by number    
            } else if(strstr(zero_is_plus, argv[1]) != NULL){
                fprintf(stdout, "%s, %s\n", name, number);
                not_found = false;
            } 
        }
    }    
    //a condition that compares a boolean value of "not found"
    if(not_found == true){
        fprintf(stdout, "Not found\n");
    }
    return 0;
}
