#include "stdafx.h"
#include <Windows.h>

#define CreateProc(procname, startupi, proc_info, priority, path) (CreateProcess(0, procname, 0, 0, FALSE, priority, 0, path, &startupi, &proc_info))
#define CreateNotepadProc(procname, startupi, proc_info) (CreateProcess(0, procname, 0, 0, FALSE, 0, 0, 0, &startupi, &proc_info))
#define SuspendedCreateProc(procname, startupi, proc_info) CreateProcess (0, procname, 0, 0, FALSE, CREATE_SUSPENDED, 0, 0, &startupi, &proc_info);


DWORD returnCode = 0;
STARTUPINFO startupi = { sizeof(startupi) };
PROCESS_INFORMATION proc_info;
SYSTEMTIME sys_time;
FILETIME file_time; 
TCHAR * task1 = _T("Task1.exe");
TCHAR * task2 = _T("Task2.exe");


int main()
{
    GetSystemTime(&sys_time);
    SystemTimeToFileTime(&sys_time, &file_time);
    TCHAR buf1[MAX_PATH];
    _stprintf(buf1, task1);
    BOOL proc = CreateProc(buf1, startupi, proc_info, BELOW_NORMAL_PRIORITY_CLASS ,_T("../Debug"));
    if (!proc) {
        printf("Task1.exe process creating error\n");
        returnCode = 1;
    } else {
        WaitForSingleObject(proc_info.hProcess, INFINITE);
        GetExitCodeProcess(proc_info.hProcess, &returnCode);
        CloseHandle(proc_info.hThread);
        CloseHandle(proc_info.hProcess);
    }

    if (returnCode == 0) {
        TCHAR buf_low[MAX_PATH];
        _stprintf(buf_low, _T("LOWTIME=%d"), file_time.dwLowDateTime);
        _tputenv(buf_low);
        TCHAR buf_hi[MAX_PATH];
        _stprintf(buf_hi, _T("HIGHTIME=%d"), file_time.dwHighDateTime);
        _tputenv(buf_hi);
        TCHAR buf2[MAX_PATH];
        _stprintf(buf2, task2);
        BOOL proc_task2 = CreateProc(buf2, startupi, proc_info, NORMAL_PRIORITY_CLASS, _T("../Debug"));
        if (!proc_task2) {
            printf("Task2.exe process creating error\n");
            returnCode = 2;
        }
        else {
            WaitForSingleObject(proc_info.hProcess, INFINITE);
            GetExitCodeProcess(proc_info.hProcess, &returnCode);
            CloseHandle(proc_info.hThread);
            CloseHandle(proc_info.hProcess);
        }
    }

    getchar();

    return returnCode;
}

