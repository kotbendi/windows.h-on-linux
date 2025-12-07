#include "windows.h"
#include <stdio.h>

int main(){
    MessageBoxA("test", "Hello, World!"); // display a message box
    
    HANDLE hprocess = OpenProcess(1234); // open a process with pid 1234

    int val = ReadProcessMemory(hprocess, (void*)0x7ffdf000 , &val, sizeof(val)); // read memory from the process

    WriteProcessMemory(hprocess, (void*)0x7ffdf000 , &val, sizeof(val)); // write memory to the process

    
    return 0;
}
