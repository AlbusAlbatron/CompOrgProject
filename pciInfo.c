#include <stdio.h>      //this is needed to read files
#include <dirent.h>     //this is needed to open and read directories
#include <string.h>     //this is needed for the substrings and for strcmp()
#include <stdlib.h>     //this is needed for quicksort, if we move that to another file, can remove

//This function creates the list, to refresh the list just call it again
//Parameters: A list to Overwrite and store values in, the size of that list
//Opens the directory that contains all the Pci devices
//Creates a struct that will hold all of the details of the contents of that directory
//while loop iterates through the opened directory until it runs out (!= NULL)
//Skips over the path to current directory, parent directory and hidden files by checking if the first character is '.'
//Then stores any real directory into our list
//For loop to count all elements of the array
//Close the directory ROOT
//return the amount of elements in the array
//When calling this function, you can assign it to a variable pciCount and need a big empty array to pass in as a parameter 
int createList(char *devices[], int tempMax) {
    DIR *ROOT = opendir("/sys/bus/pci/devices");
    struct dirent *file;

    int index = 0;
    while (( file = readdir(ROOT)) != NULL ) 
    {
        if (( file->d_name[0] == '.' )) continue;

        devices[index] = file->d_name;
        index++;
    }

    int pciCount = 0;
    for (int i = 0;(devices[i] != NULL) && (i < tempMax); i++) pciCount++;

    closedir(ROOT);
    return pciCount;
}


//This function is needed for sorting the list, I don't really understand it though
int compare_strings(const void *a, const void *b) {
    const char *str_a = *(const char **)a;
    const char *str_b = *(const char **)b;
    return strcmp(str_a, str_b);
}
//Sorts the list using qsort() from stdlib.h
int sortList(char *inList[], int inListLen) {
    qsort(inList, inListLen, sizeof(char *), compare_strings);
    return 0;
}


//These three functions are pretty much identical
//Takes in a string to overwrite, the device list, and an index from that list
//Overwrites the input string with the correct substring
//'\0' closes or like ends the string so theres not trailing characters and stuff
char* getBus(char bus[], char *inList[], int index) {
    strncpy(bus, inList[index] + 5, 2);
    bus[2] = '\0';
    return bus;
}
char* getDevice(char device[], char *inList[], int index) {
    strncpy(device, inList[index] + 8, 2);
    device[2] = '\0';
    return device;
}
char* getFunction(char function[], char *inList[], int index) {
    strncpy(function, inList[index] + 11, 1);
    function[1] = '\0';
    return function;
}


//These three functions have the same parameters as previous, but different content
//Stores the root path as a string, and the target file to get the ID from
//Strings are annoying in C, if theres a better way to concatonate than I did, idk
//Open the file from the created path
//Make a temp string to hold all content of the file
//Take the target substring, same way as above functions
//Close the file and return the string
char* getVendorID(char vendorID[], char*inList[], int index) {
    char rootPath[] = "/sys/bus/pci/devices";
    char targetFile[] = "vendor";

    char path[60] = {0};
    for (int i = 0; i < 60; i++) {
        if (i < 20) path[i] = rootPath[i];
        else if (i == 20) path[i] = '/';
        else if (i < 33) path[i] = inList[index][i - 21];
        else if (i == 33) path[i] = '/';
        else path[i] = targetFile[i - 34];
    }

    FILE *file;
    file = fopen(path, "r");
    char vendorCont[10];
    fgets(vendorCont, 10, file);
    fclose(file);
    strncpy(vendorID, vendorCont + 2, 4);
    vendorID[4] = '\0';
    return vendorID;
}
char* getDeviceID(char deviceID[], char*inList[], int index) {
    char rootPath[] = "/sys/bus/pci/devices";
    char targetFile[] = "device";
    
    char path[60] = {0};
    for (int i = 0; i < 60; i++) {
        if (i < 20) path[i] = rootPath[i];
        else if (i == 20) path[i] = '/';
        else if (i < 33) path[i] = inList[index][i - 21];
        else if (i == 33) path[i] = '/';
        else path[i] = targetFile[i - 34];
    }

    FILE *file;
    file = fopen(path, "r");
    char deviceCont[10];
    fgets(deviceCont, 10, file);
    fclose(file);
    strncpy(deviceID, deviceCont + 2, 4);
    deviceID[4] = '\0';
    return deviceID;
}
char* getClassCode(char classCode[], char*inList[], int index) {
    char rootPath[] = "/sys/bus/pci/devices";
    char targetFile[] = "class";
    
    char path[60] = {0};
    for (int i = 0; i < 60; i++) {
        if (i < 20) path[i] = rootPath[i];
        else if (i == 20) path[i] = '/';
        else if (i < 33) path[i] = inList[index][i - 21];
        else if (i == 33) path[i] = '/';
        else path[i] = targetFile[i - 34];
    }

    FILE *file;
    file = fopen(path, "r");
    char classCont[16];
    fgets(classCont, 16, file);
    fclose(file);
    strncpy(classCode, classCont + 2, 6);
    classCode[6] = '\0';
    return classCode;
}