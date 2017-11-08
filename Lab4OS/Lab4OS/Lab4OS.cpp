// Lab4OS.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define READ 0
#define READ_AND_DELETE 1
#define DELETE_MESSAGE 2

HANDLE postFile;
HANDLE memPost;
LPVOID dd;
DWORD bytes_written = 0;
int message_count = 0;
int messages_sizes = 0;
int max_messages = 0;
DWORD last_msg_address;

#define NEW_ADDRESS(address, size) ((PVOID)((DWORD)address + size))

SYSTEM_INFO virtualMem;
MEMORYSTATUSEX statusMem;
MEMORY_BASIC_INFORMATION mbi;

class MemoryUnit {
public:
    MemoryUnit(PVOID address = NULL, DWORD size = NULL, DWORD state = NULL) {
        this->address = address;
        this->size = size;
        this->state = state;
    };

    PVOID getAddress() {
        return address;
    };

    void setAddress(PVOID value) {
        address = value;
    };

    DWORD getSize() {
        return size;
    };

    void setSize(DWORD value) {
        size = value;
    };

    DWORD getState() {
        return state;
    };

    void setState(DWORD value) {
        state = value;
    };

protected:
    PVOID address;
    DWORD size;
    DWORD state;
};

class LRUUnit : public MemoryUnit {
public:
    LRUUnit(int priority, PVOID address = NULL, DWORD size = NULL, DWORD state = NULL)
        : MemoryUnit(address, size, state) {
        this->priority = priority;
    };

    void query(int new_priority) {
        priority = new_priority;
    };

    int livesFor(time_t t) {
        return priority;
    };

protected:
    int priority;
};

class VMemory {
public:
    VMemory(int size = 100) {
        vMem = std::vector<MemoryUnit *>();
        this->size = size;
    };

    MemoryUnit * reserve(int bytes) {
        MemoryUnit * unit = NULL;
        if (static_cast<int>(vMem.size()) < size || contains(bytes)) {
            int index = indexOf(bytes);
            if (index != -1) {
                vMem.erase(vMem.begin() + index);
            }
        } else {
            unit = last_used();
        }
        vMem.push_back(new MemoryUnit(NULL, bytes, NULL));   
        return unit;
    };

   int get(int index) {
        return vMem[index]->getSize();
    };

    int indexOf(int size) {
        for (int i = 0; i < static_cast<int>(vMem.size()); ++i) {
            if (vMem[i]->getSize() == size) {
                return i;
            }
        }
        return -1;
    };

    bool contains(int size) {
        return indexOf(size) != -1;
    };

    void remove(int index) {
        vMem.erase(vMem.begin() + index);
    };

    MemoryUnit * last_used() {
        MemoryUnit * front = vMem.front();
        vMem.erase(vMem.begin());
        return front;
    };

    char * toString() {
        char * buffer = new char[100]{ 0 };
        char * message = new char[10000]{ 0 };
        strcat(message, "\nVirtual Memory:\n");
        for (int i = 0; i < static_cast<int>(vMem.size()); ++i) {
            strcat(message, itoa(vMem[i]->getSize(), buffer, 10));
            strcat(message, "\n");
        }
        return message;
    };

private:
    std::vector<MemoryUnit *> vMem;
    int size;
};

class PageFile {
public:
    PageFile(int size = 1000) {
        pFile = std::vector<MemoryUnit *>();
        this->size = size;
    };

    bool write(MemoryUnit * unit) {
        if(unit == NULL) {
            return false;
        }
        if (static_cast<int>(pFile.size()) == size) {
            throw new std::exception("Out of memory");
        }
        pFile.push_back(unit);
        return true;
    };

    int indexOf(int size) {
        for (int i = 0; i < static_cast<int>(pFile.size()); ++i) {
            if (pFile[i]->getSize() == size) {
                return i;
            }
        }
        return -1;
    };

    bool contains(int size) {
        return indexOf(size) != -1;
    };

    DWORD get(int index) {
        return pFile[index]->getSize();
    };

    void remove(int index) {
        pFile.erase(pFile.begin() + index);
    };

    MemoryUnit * back() {
        return pFile.back();
    };

    bool empty() {
        return pFile.empty();
    };

    void removeLast() {
        pFile.pop_back();
    };

    char * toString() {
        char * buffer = new char[100]{ 0 };
        char * message = new char[10000]{ 0 };
        strcat(message, "\nPage file:\n");
        for (int i = 0; i < static_cast<int>(pFile.size()); ++i) {
            strcat(message, itoa(pFile[i]->getSize(), buffer, 10));
            strcat(message, "\n");
        }
        return message;
    };
    
private:
    std::vector<MemoryUnit *> pFile;
    int size;
};

class LRUMapTable {
public:
    LRUMapTable(int size = 4) {
        LRU = std::vector<LRUUnit *>();
        this->size = size;
        priority = 0;
    };

    void insert(int page) {
        if (contains(page)) {
            int index = indexOf(page);
            LRU[index]->query(priority++);
        } else {
            if (size == static_cast<int>(LRU.size())) {
                time_t cur_time = time(0);
                int diff = LRU[0]->livesFor(cur_time);
                int pos = 0;
                for (int i = 0; i < size; ++i) {
                    int seconds = LRU[i]->livesFor(cur_time);
                    if (seconds < diff) {
                        diff = seconds;
                        pos = i;
                    }
                }
                LRU[pos] = new LRUUnit(priority++, NULL, page, NULL);
            } else {
                LRU.push_back(new LRUUnit(priority++, NULL, page, NULL));
            }
         }
    };

    char * toString() {
        char * buffer = new char[100]{ 0 };
        char * message = new char[10000]{ 0 };
        strcat(message, "\nLRU Table:\n");
        for (int i = 0; i < static_cast<int>(LRU.size()); ++i) {
            strcat(message, itoa(LRU[i]->getSize(), buffer, 10));
            strcat(message, "\n");
        }
        return message;
    };

    void print() {
        printf("%s\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n", toString());
    };

    int indexOf(int size) {
        for (int i = 0; i < static_cast<int>(LRU.size()); ++i) {
            if (LRU[i]->getSize() == size) {
                return i;
            }
        }
        return -1;
    };

    bool contains(int size) {
        return indexOf(size) != -1;
    };

    DWORD get(int index) {
        return LRU[index]->getSize();
    };

private:
    std::vector<LRUUnit *> LRU;
    int size;
    int priority;
};

class Memory {
public:
    Memory() {
        virtual_v = new VMemory();
        page = new PageFile();
    };

    Memory(int virtual_size, int page_size) {
        virtual_v = new VMemory(virtual_size);
        page = new PageFile(page_size);
    };

    bool reserve(int bytes) {
        if (!tryFile(bytes)) {
            MemoryUnit * unit = virtual_v->reserve(bytes);
            return page->write(unit);
        }
        return false;
    };

    bool read(int bytes) {
        if (virtual_v->contains(bytes) || page->contains(bytes)) {
            return reserve(bytes);
        }
        return false;
    };

    int release(int bytes) {
        int returnable = NULL;
        if (virtual_v->contains(bytes) || page->contains(bytes)) {
            if (!virtual_v->contains(bytes) && page->contains(bytes)) {
                int index = page->indexOf(bytes);
                returnable = page->get(index);
                page->remove(index);
            } else {
                int index = virtual_v->indexOf(bytes);
                returnable = virtual_v->get(index);
                virtual_v->remove(index);
                if (!page->empty()) {
                    int insert = page->back()->getSize();
                    page->removeLast();
                    virtual_v->reserve(insert);
                }
            }
        }
        return returnable;
    }

    void print() {
        printf("%s%s\n~~~~~~~~~~~\n", virtual_v->toString(), page->toString());
    };

private:
    bool tryFile(int bytes) {
        if (!virtual_v->contains(bytes) && page->contains(bytes)) {
            int index = page->indexOf(bytes);
            page->write(virtual_v->last_used());
            virtual_v->reserve(page->get(index));
            page->remove(index);
        }
        return false;
    };

    VMemory * virtual_v;
    PageFile * page;
};

std::vector<MemoryUnit *> freeRegions;
std::vector<MemoryUnit *> reservedRegions;

PVOID reserveMin(int);
bool freeUnit(PVOID);
void getProcInfo();
void addMessage();
int readMessage(DWORD);
void lettersCount(TCHAR *);
void boxesCount();
void removeAll();
int calccrc();
void read(TCHAR *);


DWORD ALIGN(DWORD size, DWORD gr) {
    return (size + gr - 1) / gr * gr;
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

    DWORD start = (DWORD)dd + 16;

    int counter = 1;
    int move;
    vector<TCHAR *> strings;
    vector<int> bytes;
    vector<int> sizes;
    while (counter < pos) {
        move = *(int *)((LPVOID)start);
        start += 4;
        start += move;
        ++counter;
    }
    DWORD file_address = start;
    move = *(int *)((LPVOID)start);
    start += move;
    start += 4;
    int deleted = move;

    while (move != 0) {
        move = *(int *)((LPVOID)start);
        if (move == 0) break;
        start += 4;
        int size = move / sizeof(TCHAR);
        TCHAR * message = new TCHAR[size]{ 0 };
        for (int i = 0; i < size; ++i) {
            message[i] = *(TCHAR *)((LPVOID)start);
            start += sizeof(TCHAR);
        }
        strings.push_back(message);
        bytes.push_back(move);
        sizes.push_back(size);
    }

    DWORD clear_from = file_address;

    int difference = last_msg_address - file_address;
    for (int i = 0; i < difference; ++i) {
        *(char *)((LPVOID)clear_from) = 0;
        clear_from++;
    }

    last_msg_address = file_address;


    for (int i = 0; i < strings.size(); ++i) {
        *(int *)((LPVOID)last_msg_address) = bytes[i];
        last_msg_address += 4;
        for (int j = 0; j < sizes[i]; ++j) {
            *(TCHAR *)((LPVOID)last_msg_address) = strings[i][j];
            last_msg_address += sizeof(TCHAR);
        }
    }

    --message_count;
    messages_sizes -= deleted;

    *((int *)dd + 1) = message_count;
    *((int *)dd + 2) = messages_sizes;

    printf("Removed!\n");
    system("pause");
}

int main()
{
    printf("PART1 ===================================\n\n");
    statusMem.dwLength = sizeof(MEMORYSTATUSEX);
    GetSystemInfo(&virtualMem);
    printf("Info about computer memory:\n");
    printf("Memory page size: %d B = %d KB\n", virtualMem.dwPageSize, (int)virtualMem.dwPageSize / 1024);
    printf("The lowest memory address accessible to applications: %#x\n", virtualMem.lpMinimumApplicationAddress, (long)virtualMem.lpMinimumApplicationAddress / 1024);
    printf("The highest memory address accessible to applications: %#x\n", virtualMem.lpMaximumApplicationAddress, (long)virtualMem.lpMaximumApplicationAddress / 1024, (long)virtualMem.lpMaximumApplicationAddress / (1024*1024));

    GlobalMemoryStatusEx(&statusMem);
    printf("\nInfo about virtual memory:\n");

    printf("Available virtual memory: %I64d B = %I64d KB = %I64d MB\n", statusMem.ullAvailVirtual, statusMem.ullAvailVirtual / 1024, statusMem.ullAvailVirtual / (1024*1024));
    printf("Available page file memory: %I64d KB = %I64d MB\n", statusMem.ullAvailPageFile / 1024, statusMem.ullAvailPageFile / (1024*1024));
    printf("Total virtual memory: %I64d B = %I64d KB = %I64d MB\n", statusMem.ullTotalVirtual, statusMem.ullTotalVirtual / 1024, statusMem.ullTotalVirtual / (1024*1024));
    printf("Total page file memory: %I64d KB = %I64d MB\n", statusMem.ullTotalPageFile / 1024, statusMem.ullTotalPageFile / (1024*1024));
    printf("Phys memory: %I64d KB = %I64d MB\n", statusMem.ullTotalPhys / (1024 * 1024));
    printf("\nThere is %I64d B = %I64d KB = %I64d MB free of virtual memory\n", statusMem.ullAvailExtendedVirtual, statusMem.ullAvailExtendedVirtual / 1024, statusMem.ullAvailExtendedVirtual / (1024 * 1024));

    printf("\nPART2 ===================================\n");

    getProcInfo();

    std::vector<PVOID> ads;
    ads.push_back(reserveMin(255));
    ads.push_back(reserveMin(255));
    ads.push_back(reserveMin(4096));
    ads.push_back(reserveMin(653490));
    for ( ; static_cast<int>(ads.size()) > 0 ; ) {
        freeUnit(ads.back());
        ads.pop_back();
    }

    printf("\nPART3 ===================================\n");

    Memory * memExample = new Memory(4, 100);
    memExample->reserve(100);
    memExample->reserve(200);
    memExample->reserve(400);
    memExample->print();

    memExample->reserve(200);
    memExample->print();

    memExample->reserve(300);
    memExample->reserve(100);
    memExample->print();

    memExample->reserve(500);
    memExample->print();
    memExample->reserve(400);
    memExample->print();

    memExample->read(100);
    memExample->print();
    memExample->read(200);
    memExample->print();

    memExample->reserve(600);
    memExample->print();
    memExample->release(100);
    memExample->print();
    memExample->release(200);
    memExample->print();
    memExample->release(500);
    memExample->print();
    memExample->release(600);
    memExample->print();

    printf("\nPART3 ===================================\n");

    LRUMapTable * tableExample = new LRUMapTable();
    tableExample->insert(200);
    tableExample->print();
    tableExample->insert(500);
    tableExample->print();
    tableExample->insert(200);
    tableExample->print();
    tableExample->insert(600);
    tableExample->print();
    tableExample->insert(700);
    tableExample->print();
    tableExample->insert(50);
    tableExample->print();
    
    getchar();

    ////////

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
    while (value != 0) {
        printf("Exit : 0\nAdd message : 1\nRead message : 2\nRead message with removing : 3\nRemove : 4\nRemove all : 5\nLetters count : 6\nCount mail boxes : 7\n");
        scanf("%d", &value);
        switch (value) {
            case 1: addMessage(); break;
            case 2: readMessage(READ); break;
            case 3: remove(READ_AND_DELETE, readMessage(READ_AND_DELETE)); break;
            case 4: remove(DELETE_MESSAGE); break;
            case 5: removeAll(); break;
            case 6: lettersCount(namer); break;
            case 7: boxesCount(); break;
        }
        system("cls");
    }
    UnmapViewOfFile(dd);
    CloseHandle(memPost);
    CloseHandle(postFile);
    return 0;
}

void getProcInfo() {
    freeRegions = std::vector<MemoryUnit *>();
    reservedRegions = std::vector<MemoryUnit *>();
    HANDLE curProcess = GetCurrentProcess();
    DWORD address = (DWORD)virtualMem.lpMinimumApplicationAddress;
    DWORD granula = virtualMem.dwAllocationGranularity;
    int count = 1;
    while (address < (DWORD)virtualMem.lpMaximumApplicationAddress) {
        VirtualQueryEx(curProcess, (const void *)address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
        MemoryUnit * unit = new MemoryUnit(mbi.BaseAddress,
            ALIGN(mbi.RegionSize, granula), mbi.State);
        if (mbi.State != MEM_FREE) {
            reservedRegions.push_back(unit);
        } else {
            printf("\n%d-st free memory block\nRLength in bytes: %d B = %d KB = %d MB\n", count++, mbi.RegionSize, (int)mbi.RegionSize / 1024, (int)mbi.RegionSize / (1024 * 1024));
            printf("Memory address: %#x\n", mbi.BaseAddress);
            freeRegions.push_back(unit);
        }
        address += unit->getSize();
    }
}

PVOID reserveMin(int bytes) {
    DWORD min = 0;
    for (int i = 1; i < static_cast<int>(freeRegions.size()); ++i) {
        if (freeRegions[i]->getSize() < freeRegions[min]->getSize() && freeRegions[i]->getSize() >= bytes) {
            min = i;
        }
    }

    freeRegions[min]->setSize(freeRegions[min]->getSize() - bytes);
    //freeRegions[min]->setSize(freeRegions[min]->getSize() - bytes);
    PVOID nAddress = freeRegions[min]->getAddress();
    freeRegions[min]->setAddress(NEW_ADDRESS(nAddress, (DWORD)(bytes + 1)));

    if (freeRegions[min]->getSize() == 0) {
        freeRegions.erase(freeRegions.begin() + min);
    }
    reservedRegions.push_back(new MemoryUnit(nAddress,
        (DWORD)bytes, MEM_RESERVE));

    printf("\nUnit with size %d was allocated at %#x\n", bytes,
        reservedRegions.back()->getAddress(), freeRegions[min]->getAddress());
    return reservedRegions.back()->getAddress();
}

bool freeUnit(PVOID address) {
    for (int i = 0; i < static_cast<int>(reservedRegions.size()); ++i) {
        if (reservedRegions[i]->getAddress() == address) {
            reservedRegions[i]->setState(MEM_FREE);
            freeRegions.push_back(reservedRegions[i]);
            reservedRegions.erase(reservedRegions.begin() + i);
            printf("\nUnit with size %d was released at %#x\n", freeRegions.back()->getSize(),
                freeRegions.back()->getAddress());
            return true;
        }
    }
    return false;
}

////////////////////////////////////


void read(TCHAR * name) {

    TCHAR tmp[100] = _T("");
    if (sizeof(TCHAR) == 2) {

        _tcscat(tmp, _T("PostUnicode//"));
        printf("The program uses UNICODE encoding. If program can`t find your mailbox, switch encoding to MBCS(ASCII).\n");
    }
    else {
        _tcscat(tmp, _T("PostAscii//"));
        printf("The program uses MBCS(ASCII) encoding. If program can`t find your mailbox, switch encoding to UNICODE.\n");
    }
    _tcscat(tmp, name);
    _tcscat(tmp, _T(".post"));
    postFile = CreateFile(tmp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0);
    if (postFile == INVALID_HANDLE_VALUE) {
        throw std::invalid_argument("INVALID_HANDLE_VALUE");
    }

    if (GetFileSize(postFile, NULL) == 0) {
        system("cls");
        printf("Such file doesnt exist. Creating new file...\nEnter max size of mail box is ");
        scanf("%d", &max_messages);
        memPost = CreateFileMapping(postFile, 0, PAGE_READWRITE, 0, (16 + (max_messages * 4) + (max_messages * 1000 * sizeof(TCHAR))), NULL);
        dd = MapViewOfFile(memPost, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        int val = calccrc();
        last_msg_address = (DWORD)dd;
        *(int *)dd = val;
        last_msg_address += 4;
        *((int *)dd + 1) = message_count;
        last_msg_address += 4;
        *((int *)dd + 2) = messages_sizes;
        last_msg_address += 4;
        *((int *)dd + 3) = max_messages;
        last_msg_address += 4;
        printf("File was successfully created!");
        printf("\n");
        system("pause");
    }
    else {
        memPost = CreateFileMapping(postFile, 0, PAGE_READWRITE, 0, (16 + (100 * 4) + (100 * 1000 * sizeof(TCHAR))), NULL);
        dd = MapViewOfFile(memPost, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        message_count = *((int *)dd + 1);
        messages_sizes = *((int *)dd + 2);
        max_messages = *((int *)dd + 3);

        /*printf("%d:%d\n", *(int *)dd, calccrc());
        system("pause");*/

        last_msg_address = (DWORD)dd + 16;

        int move = *(int *)((LPVOID)last_msg_address);
        last_msg_address += 4;
        while (move != 0) {
            last_msg_address += move;
            move = * (int *)((LPVOID)last_msg_address);
            last_msg_address += 4;
        }
        last_msg_address -= 4;
    }
}

void addMessage() {
    system("cls");
    if (message_count == max_messages) {
        printf("Box is full\n");
        system("pause");
        return;
    }

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
    *(int *)((LPVOID)last_msg_address) = size_in_bytes;
    last_msg_address += 4;
    for (int i = 0; i < len; ++i) {
        *(TCHAR *)((LPVOID)last_msg_address) = message[i];
        last_msg_address += sizeof(TCHAR);
    }
    ++message_count;
    messages_sizes += size_in_bytes;

    *((int *)dd + 1) = message_count;
    *((int *)dd + 2) = messages_sizes;

    int crc = calccrc();
    *(int *)dd = crc;

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

    DWORD address_to_read = (DWORD)dd + 16;

    int counter = 1;
    int move;
    while (counter < pos) {
        move = *(int *)((LPVOID)address_to_read);
        address_to_read += 4;
        address_to_read += move;
        ++counter;
    }

    move = *(int *)((LPVOID)address_to_read);
    address_to_read += 4;
    int size = move / sizeof(TCHAR);
    TCHAR * message = new TCHAR[size]{ 0 };
    for (int i = 0; i < size; ++i) {
        message[i] = *(TCHAR *)((LPVOID)address_to_read);
        address_to_read += sizeof(TCHAR);
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

    message_count = 0;
    messages_sizes = 0;
    *((int *)dd + 1) = message_count;
    *((int *)dd + 2) = messages_sizes;
    int difference = last_msg_address - (DWORD)dd - 16;
    DWORD start = (DWORD)dd + 16;
    for (int i = 0; i < difference; ++i) {
        *(char *)((LPVOID)start) = 0;
        start++;
    }
    last_msg_address = (DWORD)dd + 16;
    int crc = calccrc();
    *(int *)dd = crc;
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

int calccrc() {
    int crc = 0xFFFF;
    if (message_count != 0) {
        DWORD point = (DWORD)dd + 16;
        int move;
        move = *(int *)((LPVOID)point);
        point += 4;
        while (move != 0) {
            crc ^= move << 8;
            for (int j = 0; j < 8; j++) {
                crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
            }
            int size = move / sizeof(TCHAR);
            TCHAR * message = new TCHAR[size]{ 0 };
            for (int i = 0; i < size; ++i) {
                message[i] = *(TCHAR *)((LPVOID)point);
                point += sizeof(TCHAR);
            }
            for (int i = 0; i < size; ++i) {
                crc ^= message[i] << 8;
                for (int j = 0; j < 8; j++) {
                    crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
                }
            }
            move = *(int *)((LPVOID)point);
            if (move == 0) break;
            point += 4;
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


