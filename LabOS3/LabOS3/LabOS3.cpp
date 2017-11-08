#include "stdafx.h"
#include <Windows.h>
#include <stdexcept>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

#define READ 0
#define READ_AND_DELETE 1
#define DELETE_MESSAGE 2

HANDLE postFile;
DWORD bytes_written = 0;
int message_count = 0;
int messages_sizes = 0;
int max_messages = 0;

int calccrc();
int main();


void read(TCHAR * name) {
    
    TCHAR tmp[100] = _T("");
    if (sizeof(TCHAR) == 2) {

        _tcscat(tmp, _T("PostUnicode//"));
        printf("The program uses UNICODE encoding. If program can`t find your mailbox, switch encoding to MBCS(ASCII).\n");
        //TCHAR tmp[100] = _T("PostUnicode//");
    }
    else {
        _tcscat(tmp, _T("PostAscii//"));
        printf("The program uses MBCS(ASCII) encoding. If program can`t find your mailbox, switch encoding to UNICODE.\n");
        //TCHAR tmp[100] = _T("PostAscii//");
    }
    _tcscat(tmp, name);
    _tcscat(tmp, _T(".post"));
    postFile = CreateFile(tmp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0);
    if (postFile == INVALID_HANDLE_VALUE) {
        throw std::invalid_argument( "INVALID_HANDLE_VALUE" );
    }

    if (GetFileSize(postFile, NULL) == 0) {
        system("cls");
        printf("Such file doesnt exist. Creating new file...\nEnter max size of mail box is ");
        scanf("%d", &max_messages);
        int val = calccrc();
        WriteFile(postFile, &val, 4, &bytes_written, 0);
        WriteFile(postFile, &message_count, 4, &bytes_written, 0);
        WriteFile(postFile, &messages_sizes, 4, &bytes_written, 0);
        WriteFile(postFile, &max_messages, 4, &bytes_written, 0);
        printf("File was successfully created!");
        printf("\n");
        system("pause");
    } else {
        int crc;
        ReadFile(postFile, &crc, 4, &bytes_written, 0);
        ReadFile(postFile, &message_count, 4, &bytes_written, 0);
        ReadFile(postFile, &messages_sizes, 4, &bytes_written, 0);
        ReadFile(postFile, &max_messages, 4, &bytes_written, 0);
        if (crc != calccrc()) {
            printf("File was corrupted!\nDo you want to delete this file? (Y/N)\n");
            char p;
            scanf("%1s", &p);
            if (p == 'Y') {
                CloseHandle(postFile);
                if (DeleteFile(tmp)) {
                    printf("Success!\n");
                    system("pause");
                    system("cls");
                    main();
                }
            }
            system("pause");
            exit(EXIT_FAILURE);
        }
    }
    SetFilePointer(postFile, 0, NULL, FILE_BEGIN);
}

void addMessage() {
    system("cls");
    if (message_count == max_messages) {
        printf("Box is full\n");
        system("pause");
        return;
    }

    SetFilePointer(postFile, 0, NULL, FILE_END);

    TCHAR message[1000] = { 0 };
    fflush(stdin);
    printf("Enter your message: \n");
    getchar();
    _fgetts(message, 1000, stdin);
    size_t len = _tcslen(message);
    if (len > 0 && message[len - 1] == '\n') {
        message[--len] = '\0';
    }
    int size_in_bytes = _tcslen(message) * sizeof(TCHAR);
    WriteFile(postFile, &size_in_bytes, 4, &bytes_written, 0);
    WriteFile(postFile, &message, size_in_bytes, &bytes_written, 0);
    ++message_count;
    messages_sizes += size_in_bytes;

    SetFilePointer(postFile, 4, NULL, FILE_BEGIN);
    WriteFile(postFile, &message_count, 4, &bytes_written, 0);
    WriteFile(postFile, &messages_sizes, 4, &bytes_written, 0);
    SetFilePointer(postFile, 0, NULL, FILE_BEGIN);
    int crc = calccrc();
    SetFilePointer(postFile, 0, NULL, FILE_BEGIN);
    WriteFile(postFile, &crc, 4, &bytes_written, 0);
    printf("Successfully added!\n");
    system("pause");
    return;
}

int readMessage(DWORD RorD) {
    system("cls");
    SetFilePointer(postFile, 0, NULL, FILE_BEGIN);
    printf("You have %d messages! Which one you want to read?\n", message_count);
    int pos;
    scanf("%d", &pos);
    if (pos < 1 || pos > message_count) {
        printf("Wrong message position!\n");
        system("pause");
        return -1;
    }

    SetFilePointer(postFile, 16, NULL, FILE_BEGIN);

    int counter = 1;
    int move;
    while (counter < pos) {
        BOOL tm = ReadFile(postFile, &move, 4, &bytes_written, 0);
        if (!tm) {
            GetLastError();
            system("pause");
        }
        SetFilePointer(postFile, move, NULL, FILE_CURRENT);
        ++counter;
    }

    ReadFile(postFile, &move, 4, &bytes_written, 0);
    int size = move / sizeof(TCHAR);
    TCHAR * message = new TCHAR[size];
    BOOL me = ReadFile(postFile, message, move, &bytes_written, 0);
    if (me && bytes_written == 0) {
        printf("down");
        system("pause");
    }
    system("cls");
    _tprintf(_T("%.*s"), size, message);
    printf("\n");
    if (RorD == READ) {
        system("pause");
    }
    return pos;
}

void removeAll() {
    if (message_count == 0)
        return;

    SetFilePointer(postFile, 16, NULL, FILE_BEGIN);
    message_count = 0;
    messages_sizes = 0;
    SetEndOfFile(postFile);
    SetFilePointer(postFile, 4, NULL, FILE_BEGIN);
    WriteFile(postFile, &message_count, 4, &bytes_written, 0);
    WriteFile(postFile, &messages_sizes, 4, &bytes_written, 0);
    SetFilePointer(postFile, 0, NULL, FILE_BEGIN);
    int crc = calccrc();
    SetFilePointer(postFile, 0, NULL, FILE_BEGIN);
    WriteFile(postFile, &crc, 4, &bytes_written, 0);
    system("cls");
    printf("All messages were removed!\n");
    system("pause");
}

void lettersCount(TCHAR * name) {
    system("cls");
    _tprintf(_T("The user `%s` has %d letters:)\n"), name, message_count);
    system("pause");
}

void boxesCount() {
    system("cls");
    int count = 0;
    WIN32_FIND_DATA fd;
    HANDLE h;
    if (sizeof(TCHAR) == 2) {
        h = FindFirstFile(_T("PostUnicode//*.post"), &fd);
    }
    else {
        h = FindFirstFile(_T("PostAscii//*.post"), &fd);
    }
   // h = FindFirstFile(_T("Post//*.post"), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        ++count;
        while (FindNextFile(h, &fd)) {
            ++count;
        }
        FindClose(h);
    }
    _tprintf(_T("There are %d mail boxes XD\n"), count);
    system("pause");
}

void remove(DWORD action, int position = 0) {
    if (action != READ_AND_DELETE) {
        system("cls");
    }
    int pos;
    if ((position < 1 || position > message_count) && position != 0) {
        printf("Wrong message position!\n");
        system("pause");
        return;
    }
    else {
        if (position == 0) {
            printf("You have %d messages! Which one you want to delete?\n", message_count);
            scanf("%d", &pos);
            if (pos < 1 || pos > message_count) {
                printf("Wrong message position!\n");
                system("pause");
                return;
            }
        }
        else {
            pos = position;
        }
    }

    SetFilePointer(postFile, 16, NULL, FILE_BEGIN);

    int counter = 1;
    int sdvig = 0;
    int move;
    vector<TCHAR *> strings;
    vector<int> bytes;
    while (counter < pos) {
        ReadFile(postFile, &move, 4, &bytes_written, 0);
        sdvig += 4;
        sdvig += move;
        SetFilePointer(postFile, move, NULL, FILE_CURRENT);
        ++counter;
    }
    BOOL tm = ReadFile(postFile, &move, 4, &bytes_written, 0);
    int deleted = move;
    SetFilePointer(postFile, move, NULL, FILE_CURRENT);
    
    sdvig += 16;

    while (!(tm && bytes_written == 0)) {
        ReadFile(postFile, &move, 4, &bytes_written, 0);
        int size = move / sizeof(TCHAR);
        TCHAR * message = new TCHAR[size];
        tm = ReadFile(postFile, message, move, &bytes_written, 0);
        strings.push_back(message);
        bytes.push_back(move);
    }
    
    SetFilePointer(postFile, sdvig, NULL, FILE_BEGIN);
    SetEndOfFile(postFile);

    SetFilePointer(postFile, 0, NULL, FILE_END);

    for (int i = 0; i < strings.size() - 1; ++i) {
        WriteFile(postFile, &bytes[i], 4, &bytes_written, 0);
        WriteFile(postFile, strings[i], bytes[i], &bytes_written, 0);
    }

    --message_count;
    messages_sizes -= deleted;

    SetFilePointer(postFile, 4, NULL, FILE_BEGIN);
    WriteFile(postFile, &message_count, 4, &bytes_written, 0);
    WriteFile(postFile, &messages_sizes, 4, &bytes_written, 0);
    SetFilePointer(postFile, 0, NULL, FILE_BEGIN);
    int crc = calccrc();
    SetFilePointer(postFile, 0, NULL, FILE_BEGIN);
    WriteFile(postFile, &crc, 4, &bytes_written, 0);

    printf("Removed!\n");
    system("pause");
}

int calccrc() {
    int crc = 0xFFFF;
    if (message_count != 0) {
        int move;
        SetFilePointer(postFile, 16, NULL, FILE_BEGIN);
        BOOL tm = ReadFile(postFile, &move, 4, &bytes_written, NULL);
        while (!(tm && bytes_written == 0)) {
            crc ^= move << 8;
            for (int j = 0; j < 8; j++) {
                crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
            }
            int size = move / sizeof(TCHAR);
            TCHAR * message = new TCHAR[size];
            ReadFile(postFile, message, move, &bytes_written, 0);
            for (int i = 0; i < size; ++i) {
                crc ^= message[i] << 8;
                for (int j = 0; j < 8; j++) {
                    crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
                }
            }  
            tm = ReadFile(postFile, &move, 4, &bytes_written, NULL);
        }
    }

    int arr_message[3] = { message_count, messages_sizes, max_messages };
    for (int i = 0; i < 3; ++i) {
        crc ^= arr_message[i] << 8;
        for (int j = 0; j < 8; j++) {
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
        }
    }
    return crc;
}


int main()
{
    TCHAR namer[20];
    if (sizeof(TCHAR) == 2) {
        printf("The program uses UNICODE encoding. If program can`t find your mailbox, switch encoding to MBCS(ASCII).\n");
    }
    else {
        printf("The program uses MBCS(ASCII) encoding. If program can`t find your mailbox, switch encoding to UNICODE.\n");
    }
    printf("Hello, user! Enter yours mail box name:\n");
    _tscanf(_T("%s"), &namer);
    read(namer);
    int value = -1;
    system("cls");
    while(value != 0) {
        printf("Exit : 0\nAdd message : 1\nRead message : 2\nRead message with removing : 3\nRemove : 4\nRemove all : 5\nLetters count : 6\nCount mail boxes : 7\n");
        scanf("%d", &value);
        switch (value) {
            case 0: return 0;
            case 1: addMessage(); break;
            case 2: readMessage(READ); break;
            case 3: remove(READ_AND_DELETE, readMessage(READ_AND_DELETE)); break;
            case 4: remove(DELETE_MESSAGE); break;
            case 5: removeAll(); break;
            case 6: lettersCount(namer); break;
            case 7: boxesCount(); break;
        }
        value = -1;
        system("cls");
    }
    CloseHandle(postFile);
    return 0;
}

