#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>

#define CreateNotepadProc(procname, startupi, proc_info) (CreateProcess(0, procname, 0, 0, FALSE, 0, 0, 0, &startupi, &proc_info))
#define SuspendedCreateProc(procname, startupi, proc_info) CreateProcess (0, procname, 0, 0, FALSE, CREATE_SUSPENDED, 0, 0, &startupi, &proc_info);

DWORD returnCode = 0;
STARTUPINFO startupi = { sizeof(startupi) };
PROCESS_INFORMATION proc_info;
TCHAR procname[] = _T("notepad.exe");

int main()
{
    BOOL point = CreateNotepadProc(procname, startupi, proc_info);
    if (!point) {
        printf("Error with creation\n");
        returnCode = 1;
    }
    else {
        WaitForSingleObject(proc_info.hProcess, INFINITE);
        GetExitCodeProcess(proc_info.hProcess, &returnCode);
    }
    return returnCode;
}

