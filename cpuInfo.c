#include <stdio.h> //needed for fopen and was used in debugging. No print statements in final version
#include <stdlib.h> //needed for converting strings to numbers
#include <string.h> //needed for strlen() and strcmp()

//this function counts the amount of lines in the file cpuinfo that start with the word 'processor'
//that is simplified to starting with 'p' and having an 'r' at index 8 of the line. this matches nothing else
//opens the file
//creates a big string to store the contents of one line
//iterates through every line of the file
//checks for a match, increments count if found
//returns count
int get_count() {
    int count = 0;

    FILE *file;
    file = fopen("/proc/cpuinfo", "r");
    char read_line[1000];
    while (fgets(read_line, sizeof read_line, file) != NULL){
        if ((read_line[0] == 'p') && (read_line[8] == 'r')) count++;
    };
    fclose(file);
    return count;
}

//this function takes in 2 arguments, the field name to target, and the string to write output to
//it searches the file for the first instance of the field, writes the field content and returns 1
//if it gets through the all file lines without a match, it returns 0
//opens the file
//first thing it does is strip all whitespace from the request input and adds a colon to the end
//makes a new string to hold the cleaned result, skips past every character of the input that is whitespace
//saves every letter of the request into the clean string, and a colon in the following index
//this colon is overwritten every time except the last, resulting in a single trailing colon
//it reads the file and iterates through every line
//it does similar cleaning to each line, removing whitespace. It looks for a colon to stop saving the string
//it compares the two cleaned strings
//if they match, it iterates through the whole line until hitting a colon
//when it hits the colon, it writes the rest of the line to the out_str, and returns 1
int parse_cpu_info(char request[], char* out_str) {
    FILE *file;
    file = fopen("/proc/cpuinfo", "r");

    //Format the input target field
    char stripped_request[100] = {' '};
    for (int i = 0, j = 0; i < strlen(request); i++) {
        if (request[i] == ' ') continue;
        if (request[i] == '\t') continue;
        stripped_request[j] = request[i];
        stripped_request[j+1] = ':';
        j++;
    }

    char read_line[1000];
    while (fgets(read_line, sizeof read_line, file) != NULL) {
        //Format the field of each line
        char stripped_field[100];
        for (int i = 0, j = 0; i < 100; i++) {
            if (read_line[i] == ' ') continue;
            if (read_line[i] == '\t') continue;
            stripped_field[j] = read_line[i];
            j++;
            if (read_line[i] == ':') {
                stripped_field[j] = '\0';
                break;
            }
        }
        if (!strcmp(stripped_field, stripped_request)) {
            int start_read_val = 0;
            for (int i = 0, j = 0; i < 100; i++) {
                if (start_read_val) {
                    out_str[j] = read_line[i];
                    j++;
                }
                else {
                    if (read_line[i] == ':') {
                        start_read_val = 1;
                        i++;
                    }
                }
            }
            out_str[strlen(out_str) - 1] = '\0';
            fclose(file);
            return 1;
        }
    }   

    fclose(file);
    return 0;
}

//this function calls the above function with the specific field name "model name"
//if the function returns 1, it means the file contains a "model name" field
//if it doesn't, we do not want an empty string for model name so we set it to "unknown"
//I cant actually test if this works because my cores have a model name. if someone wants to test this that would be great
int get_model_name(char* out_str) {
    if (!parse_cpu_info("model name", out_str)) out_str = "unknown";
    return 0;
}

//This function takes in 2 args, the core you want info for and the array to write output to
//It searches the file to find a line that starts with cpuN where N is the arg core
//if it finds a match, it writes all the following numbers to the array and returns 1
//if theres no match in the file, it returns 0
//opens the file
//makes a big string to store each line, and iterates through every line
//it checks to see if the line starts with "cp" which only matches the first line and any numbered cpus
//stores the 4th and 5th character from each cpu line, either number space or number number, and converts to an int
//if that int matches the arg core, we have our match and this line will be used to write to the array
//make a string to hold up the number we are reading
//iterates through the line, starting from index 5 for double digit cpus, this is the first character in the first number
//if this is a single digit cpu, this is a space, which we check and skip
//now we write characters to the string we made until we hit a space
//when we hit a space, we convert that string to a long, write the long to the output array, and clear the string
//repeat until the end of the line, then return 1
int parse_stat(int core, long out_arr[]) {
    FILE *file;
    file = fopen("/proc/stat", "r");

    char read_line[1000];
    while (fgets(read_line, sizeof read_line, file) != NULL) {
        if ((read_line[0] == 'c') && (read_line[1] == 'p')) {
            char line_num[10] = {' '};
            line_num[0] = read_line[3];
            line_num[1] = read_line[4];
            line_num[2] = '\0';
            int int_line_num = atoi(line_num);
            if (int_line_num == core) {

                char num_holder[10] = {' '};
                for (int i = 5, j = 0, k = 0; i < strlen(read_line); i++) {
                    if (i == 5) if (read_line[i] == ' ') continue;
                    if (read_line[i] == ' ') {
                        out_arr[j] = atol(num_holder);
                        for (int l = 0;  l < 10; l++) num_holder[l] = ' ';
                        j++;
                        k = 0;
                        continue;
                    }
                    else num_holder[k] = read_line[i];
                    k++;
                }

                fclose(file);
                return 1;
            } 
        }
    }

    fclose(file);
    return 0;
}

//These functions call the above function
//creates a long array, calls the function on that array, sums up the required elements of the array and returns the sum
//indices 0,1,2,5,6,7 for busy
//indices 3 and 4 for idle
long get_busy(int core) {
    long core_stats[12] = {0};
    parse_stat(core, core_stats);
    long sum_busy = core_stats[0] + core_stats[1] + core_stats[2] + core_stats[5] + core_stats[6] + core_stats[7];
    return sum_busy;
}
long get_idle(int core) {
    long core_stats[12] = {0};
    parse_stat(core, core_stats);
    long sum_idle = core_stats[3] + core_stats[4];
    return sum_idle;
}