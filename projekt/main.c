#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 1024
#define REGA "/proc/proj4wojfil/rejA"
#define REGS "/proc/proj4wojfil/rejS"
#define REGW "/proc/proj4wojfil/rejW"

unsigned int read_from_file(char *);
int write_to_file(char *, unsigned int);
int test_module();

int main(void){
    int test = test_module();
    if(test > 0){
        printf("TEST FAILED at %d values\n",test);
    }
    else{
        printf("TEST PASSED\n");
    }
    return 0;
}

unsigned int read_from_file(char *filePath) {
    char buffer[MAX_BUFFER + 1];
    int file = open(filePath, O_RDONLY);
    if (file == -1) {
        printf("Open %s - error number %d\n", filePath, errno);
        exit(5);
    }
    int n = read(file, buffer, MAX_BUFFER - 1);
    close(file);

    if (n == -1) {
        printf("Read %s - error number %d\n", filePath, errno);
        exit(5);
    }

    buffer[n] = '\0';

    return strtoul(buffer, NULL, 10);
}

int write_to_file(char *filePath, unsigned int input){
    char buffer[MAX_BUFFER];
    FILE *file=fopen(filePath, "w");
    if(file == NULL){
        printf("Open %s - error number %d\n", filePath, errno);
        exit(6);
    }
    snprintf(buffer, MAX_BUFFER, "%o",input);
    fwrite(buffer, strlen(buffer), 1, file);
    fclose(file);
    return 0;
}

unsigned int calculate(unsigned int test_value){
    write_to_file(REGA, test_value);
    unsigned int read;
    sleep(1);
    do{
        read = read_from_file(REGW);
    }
    while(read ==0 || read==1);
    read = read_from_file(REGW);
    printf("test_value=0%o, out=0%o(OCT) out=%u(dec) state=%d\n", test_value, read, read, read_from_file(REGS));
    return read;
}


int test_module() {
    unsigned int args[9] = {02, 03, 05, 07, 011, 012, 013, 014, 01750}; 
    unsigned int results[9] = {03, 05, 013, 021, 027, 035, 037, 045, 017357}; 

    for (int i = 0; i < 9; i++) {
        unsigned int result = calculate(args[i]);
        if (result != results[i]) {
            printf("Test failed for input 0%o: expected 0%o, got 0%o\n", args[i], results[i], result);
            return i + 1;
        }
    }
    return 0;
}