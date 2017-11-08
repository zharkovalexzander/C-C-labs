// Lab6OS.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <Windows.h>

#define ThreadCount volatile DWORD
#define MAXTHREADS 10

#ifdef NDEBUG
    #define IS_DEBUG -1
#else
    #define IS_DEBUG 1
#endif

ThreadCount threadsСount = 0;
TCHAR * threadNumber[] = { _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"), _T("0") };
HANDLE threads[MAXTHREADS];
std::ofstream fout;


int waitForThread(char * ptChar) {
  /*  for (int i = 0; i < 1000000; ++i) {
        //wait for thread
    }*/
    if (IS_DEBUG == 1) {
        fout << ptChar << " thread was ended\r\n";
    }
    printf("%s thread was ended\r\n", ptChar);
    return 0;
}

DWORD WINAPI TestThread(PVOID ptr) {
    InterlockedIncrement(&threadsСount);
    char * ptChar = (char *)ptr;
    printf("%s thread was started\r\n", ptChar);
    if (IS_DEBUG == 1) {
        fout << ptChar << " thread was started\r\n";
    }
    return waitForThread(ptChar);
}

void process() {
    for (int i = 0; i < MAXTHREADS; ++i) {
        threads[i] = CreateThread(0, 0, TestThread, threadNumber[i], 0, 0);
        if (!threads[i]) {
            if (IS_DEBUG == 1) {
                fout << "Unable to create " << threadNumber[i] << "  thread" << std::endl;
            }
            printf("Unable to create %s thread\r\n", threadNumber[i]);
        }
    }
    WaitForMultipleObjects(MAXTHREADS, threads, false, INFINITE);
    if (IS_DEBUG == 1) {
        fout << "Thread count is " << threadsСount << std::endl;
    }
    printf("Thread count is %d\r\n", threadsСount);
    for (int i = 0; i < MAXTHREADS; ++i) {
        CloseHandle(threads[i]);
    }
}

int main()
{
    if (IS_DEBUG == 1) {
        fout = std::ofstream("output.txt");
    }
    process();
    system("pause");
    return 0;
}

