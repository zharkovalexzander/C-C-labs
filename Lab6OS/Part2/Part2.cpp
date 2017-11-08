// Part2.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#define ThreadCount volatile DWORD

ThreadCount threads�ount;

DWORD WINAPI TestThread(PVOID ptr) {
    InterlockedIncrement(&threads�ount);
    return 0;
}

void process() {
    int answer = 0;
    int handler = 2;
    while (true) {
        HANDLE * threads = new HANDLE[handler];
        threads�ount = 0;
        for (int h = 0; h < handler; ++h) {
            threads[h] = CreateThread(0, 0, TestThread, 0, 0, 0);
        }
        WaitForMultipleObjects(handler, threads, true, INFINITE);
        if (threads�ount != handler) {
            printf("It is possible to create %d threads maximum\r\n", answer);
            break;
        }
        answer = handler;
        for (int h = 0; h < handler; ++h) {
            CloseHandle(threads[h]);
        }
        handler *= 2;
        delete[] threads;
    }
}

int main()
{
    process();
    system("pause");
    return 0;
}

