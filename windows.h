#pragma once
#include <sys/ptrace.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

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
