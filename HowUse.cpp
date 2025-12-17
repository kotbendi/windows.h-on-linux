#include "windows.h"
#include <stdio.h>

int main(){
    BYTE Color = 0xff; // Create a BYTE variable
    HANDLE hprocess; //Create a process handle variable

    MessageBox("test", "Hello, World!"); // display a message box
    
    hprocess = OpenProcess(1234); // open a process with pid 1234
    
    int val = ReadProcessMemory(hprocess, (void*)0x7ffdf000 , &val, sizeof(val)); // read memory from the process

    WriteProcessMemory(hprocess, (void*)0x7ffdf000 , &val, sizeof(val)); // write memory to the process
    
    CloseHandle(hprocess); // close the process handle

    //Watch all functions in AllFunctions.txt
    return 0;
}
