//header file shares the prototype for every function needed between shared files

int createList(char *devices[], int tempMax);
int sortList(char *inList[], int inListLen);

char* getBus(char bus[], char *inList[], int index);
char* getDevice(char device[], char *inList[], int index);
char* getFunction(char function[], char *inList[], int index);

char* getVendorID(char vendorID[], char*inList[], int index);
char* getDeviceID(char deviceID[], char*inList[], int index);
char* getClassCode(char classCode[], char*inList[], int index);