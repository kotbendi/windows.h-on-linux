#pragma once
#include "lib.h"

typedef unsigned int        UINT32, *PUINT32;
typedef unsigned __int64    UINT64, *PUINT64;
typedef BOOL* PBOOL;
typedef void* LPVOID;
typedef uint32_t DWORD;
typedef unsigned char BYTE;
typedef uint16_t WORD;
typedef struct {
    pid_t pid;      
    int mem_fd;     
} HANDLE;
size_t ReadProcessMemory(HANDLE hProcess, void* addr, void* Buffer, size_t Size){
    if (hProcess.mem_fd == -1)
        return -1;
    ssize_t n = pread(hProcess.mem_fd, Buffer, Size, (off_t)addr);
    if (n == -1) {
        perror("pread");
    }
    return n;
}
HANDLE OpenProcess(pid_t pid) {
    HANDLE handle;
    handle.pid = pid;

    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("ptrace attach");
        handle.mem_fd = -1;
        return handle;
    }


    waitpid(pid, NULL, 0);


    char mem_path[64];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
    handle.mem_fd = open(mem_path, O_RDWR);
    if (handle.mem_fd == -1) {
        perror("open /proc/<pid>/mem");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
    }

    return handle;
}
int MessageBoxA(const char* title, const char* message) {
    int screen;
    Display* display = XOpenDisplay(NULL);
    
    Window window;
    if (display == NULL) {
        return 1;
    }
    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        10, 10,               
        200, 100,             
        1,                   
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XStoreName(display, window, title);
    XMapWindow(display, window);

    XEvent event;
    while (1) {
        XNextEvent(display, &event);

        if (event.type == Expose) {  

            XDrawString(display, window,
                        DefaultGC(display, screen),
                        10, 20,
                        message, 14);
        }
        
        if (event.type == KeyPress) {
            break;
        }
    }

    XCloseDisplay(display);
    return 0;
}
size_t WriteProcessMemory(HANDLE hProcess, void* addr, void* Buffer, size_t Size){
    if (hProcess.mem_fd == -1)
        return -1;
    ssize_t n = pwrite(hProcess.mem_fd, Buffer, Size, (off_t)addr);
    
    if (n == -1) {
        perror("pwrite");
    }
    return n;
}
int VirtualFree(void* mem, size_t size){
    return munmap(mem, size);
}
void *VirtualAlloc(size_t size){

    return mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
}
int CloseHandle(HANDLE hProcess){
    if (hProcess.mem_fd != -1) {
        close(hProcess.mem_fd);
    }
    if (ptrace(PTRACE_DETACH, hProcess.pid, NULL, NULL) == -1) {
        perror("ptrace detach");
        return -1;
    }
    return 0;
}
int CreateFile(const char* filename,const char* text){
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("fopen");
        return -1;
    }
    fwrite(text, 1, strlen(text), file);
    fclose(file);
    return 0;
}
char* ReadFile(const char* filename){ 
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("fopen");
        return nullptr;
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = new char[fileSize + 1];
    if (!buffer) {
        perror("new");
        fclose(file);
        return nullptr;
    }

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
        perror("fread");
        delete[] buffer;
        fclose(file);
        return nullptr;
    }
    buffer[fileSize] = '\0'; 

    fclose(file);
    return buffer;
}
