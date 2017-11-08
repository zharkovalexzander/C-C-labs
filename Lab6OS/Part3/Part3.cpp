// Part3.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

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

class Data {
public:
    Data() {
        book = "Men";
        word_c = 0;
        parts = new string[35]{ "have", "no", "more", "time", "to", "understand", "anything.", "They", "buy", "things", "all", "ready", "made", "at", "the", "shops.", "But", "there", "is", "no", "shop", "anywhere", "where", "one", "can", "buy", "friendship,", "and", "so", "men", "have", "no", "friends", "any", "more." };
    };

    void write() {
        if (word_c < 34) {
            book.append(" ");
        }
        book.append(parts[word_c++]);
    };

    bool isFull() {
        return word_c == 35;
    }

    string read() {
        return book;
    };

private:
    string book;
    int word_c;
    string * parts;
};

Data d;
Semaphore read_prevent, write_prevent, write_protection, read_protection, common_sem, print_queue;
int read_counter = 0, write_counter = 0;
TCHAR * readersNames[] = { _T("Mike"), _T("Josh"), _T("Nick") };
TCHAR * writersNames[] = { _T("Elly"), _T("Megan") };
HANDLE threadsR[3];
HANDLE threadsW[2];

void sleep() {
    for(unsigned long a = 0; a < 10000000; ++a);
}

DWORD WINAPI writer(PVOID ptr) {
    while (!d.isFull()) {
        write_protection.Enter();
        if (++write_counter == 1) {
            read_prevent.Enter();
        }
        write_protection.Leave();
        write_prevent.Enter();
        d.write();
        write_prevent.Leave();
        print_queue.Enter();
        //cout << (char *)ptr << " writes." << endl;
        printf("%s writes.\r\n", (char *)ptr);
        print_queue.Leave();
        write_protection.Enter();
        if (--write_counter == 0) {
            read_prevent.Leave();
        }
        write_protection.Leave();
    }
    return 0;
};

DWORD WINAPI reader(PVOID ptr) {
    while (true) {
        common_sem.Enter();
        read_prevent.Enter();
        read_protection.Enter();
        if (++read_counter == 1) {
            write_prevent.Enter();
        }
        read_protection.Leave();
        common_sem.Leave();
        string r = d.read();
        read_protection.Enter();
        read_prevent.Leave();
        if (--read_counter == 0) {
            write_prevent.Leave();
        }
        read_protection.Leave();
        print_queue.Enter();
        printf("%s reads: '%s'\r\n", (char *)ptr, r.c_str());
        //cout << (char *)ptr << " reads: '" << r << "'\n";
        print_queue.Leave();
    }
}

void prepare() {
    read_prevent = Semaphore();
    read_prevent.Init(1);
    write_prevent = Semaphore();
    write_prevent.Init(1);
    write_protection = Semaphore();
    write_protection.Init(1);
    read_protection = Semaphore();
    read_protection.Init(1);
    common_sem = Semaphore();
    common_sem.Init(1);
    print_queue = Semaphore();
    print_queue.Init(1);
    d = Data();
}

int main()
{
    prepare();
    HANDLE * ths = new HANDLE[5];
    int Rpos = 0;
    int Wpos = 0;
    for (int i = 0; i < 5; ++i) {
        if (i % 2 != 0) {
            ths[i] = CreateThread(0, 0, writer, writersNames[Wpos++], 0, 0);
        }
        else {
            ths[i] = CreateThread(0, 0, reader, readersNames[Rpos++], 0, 0);
        }
    }

    WaitForMultipleObjects(5, ths, true, INFINITE);

    for (int i = 0; i < 5; ++i) {
        CloseHandle(ths[i]);
    }
    system("pause");
    return 0;
}

