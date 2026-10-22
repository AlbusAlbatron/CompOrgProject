//header file shares the prototype for every function needed between shared files

int create_list(char *devices[], int temp_max);
int sort_list(char *in_list[], int in_list_len);

char* get_bus(char bus[], char *in_list[], int index);
char* get_device(char device[], char *in_list[], int index);
char* get_function(char function[], char *in_list[], int index);

char* get_vendor_id(char vendor_id[], char*in_list[], int index);
char* get_device_id(char device_id[], char*in_list[], int index);
char* get_class_code(char class_code[], char*in_list[], int index);
