// Philosophers.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#define PHILOSOPHERS 5
#define LEFT_FORK ((fork + PHILOSOPHERS - 1) % PHILOSOPHERS)
#define RIGHT_FORK ((fork + 1) % PHILOSOPHERS)
#define STATE_THINKING 0x0A
#define STATE_EATING 0x0B
#define STATE_WTE 0x0C

using namespace std;

class Semaphore {
public:
    Semaphore() {
        sem_c = 0;
    };

    void Init(int count) {
        sem_c = count;
    };

    void Enter() {
        while (sem_c == 0);
        --sem_c;
    };

    void Leave() {
        ++sem_c;
    };

private:
    int sem_c;
};

string * phils = new string[5]{ "Homer", "Socrates", "Aristotle", "Pythagoras", "Anaxagoras" };
TCHAR * threadNumber[] = { _T("0"), _T("1"), _T("2"), _T("3"), _T("4") };
int states[PHILOSOPHERS];
//Semaphore * mutexes, common_sem;
HANDLE * mutexes, common_sem;

void prepare() {
    /*mutexes = new Semaphore[PHILOSOPHERS];
    for (int i = 0; i < PHILOSOPHERS; ++i) {
        mutexes[i] = Semaphore();
    }
    common_sem.Init(1);*/
    mutexes = new HANDLE[PHILOSOPHERS];
    for (int i = 0; i < PHILOSOPHERS; ++i) {
        mutexes[i] = CreateSemaphore(0, 0, 1, 0);
    }
    common_sem = CreateSemaphore(0, 1, 1, 0);
}

void tryToTakeFork(int fork) {
    if (states[fork] == STATE_WTE && states[LEFT_FORK] != STATE_EATING && states[RIGHT_FORK] != STATE_EATING) {
        states[fork] = STATE_EATING;
        ReleaseSemaphore(mutexes[fork], 1, NULL);
        //mutexes[fork].Leave();
    }
}

void take_forks(int fork) {
    //common_sem.Enter();
    WaitForSingleObject(common_sem, INFINITE);
    states[fork] = STATE_WTE;
    tryToTakeFork(fork);
    ReleaseSemaphore(common_sem, 1, NULL);
    WaitForSingleObject(mutexes[fork], INFINITE);
    //common_sem.Leave();
    //mutexes[fork].Enter();
}

void put_forks(int fork) {
    //common_sem.Enter();
    WaitForSingleObject(common_sem, INFINITE);
    states[fork] = STATE_THINKING;
    tryToTakeFork(LEFT_FORK);
    tryToTakeFork(RIGHT_FORK);
    ReleaseSemaphore(common_sem, 1, NULL);
    //common_sem.Leave();
}

void think(string name) {
    printf("%s thinks\r\n", name.c_str());
}

void eat(string name) {
    printf("%s eats\r\n", name.c_str());
}

DWORD WINAPI philosopher(PVOID ptr) {
    int p = _tstoi((TCHAR *)ptr);
    while (true) {
        think(phils[p]);
        take_forks(p);
        eat(phils[p]);
        put_forks(p);
    }
}

int main()
{
    prepare();
    HANDLE * ths = new HANDLE[5];
    for (int i = 0; i < 5; ++i) {
        ths[i] = CreateThread(0, 0, philosopher, threadNumber[i], 0, 0);
    }

    WaitForMultipleObjects(5, ths, true, INFINITE);

    for (int i = 0; i < 5; ++i) {
        CloseHandle(ths[i]);
    }
    system("pause");
    return 0;
}

