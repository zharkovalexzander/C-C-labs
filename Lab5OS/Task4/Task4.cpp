#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

HANDLE desc;
PROCESSENTRY32 proc_info;
MODULEENTRY32 module;

std::vector<TCHAR *> getModules(DWORD id);
bool contains(TCHAR * strin);

int main()
{
    desc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    proc_info.dwSize = sizeof(proc_info);
    if (desc != INVALID_HANDLE_VALUE) {
        BOOL snap = Process32First(desc, &proc_info);
        do {
            _tprintf(_T("Process name: %s | Threads: %d\nDLLs used: \n"), proc_info.szExeFile, proc_info.cntThreads);
            getModules(proc_info.th32ProcessID);
            _tprintf(_T("\n===============================\n"));
        } while (Process32Next(desc, &proc_info));
    }
    CloseHandle(desc);
    getchar();
    return 0;
}

std::vector<TCHAR *> getModules(DWORD id) {
    std::vector<TCHAR *> chars;
    HANDLE descr = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, id);
    module.dwSize = sizeof(module);
    if (descr != INVALID_HANDLE_VALUE) {
        BOOL snap = Module32First(descr, &module);
        do {
            if (contains(module.szModule)) {
                chars.push_back(module.szModule);
            }
        } while (Module32Next(descr, &module));
    }
    CloseHandle(descr);
    for (int i = 0; i < static_cast<int>(chars.size()); ++i) {
        _tprintf(_T("%s\n"), chars[i]);
    }
    return chars;
}

bool contains(TCHAR * strin) {
    TCHAR * result = _tcsstr(strin, _T("dll"));
    int position = result - strin;
    int substringLength = _tcslen(strin) - position;
    if (substringLength < _tcslen(strin)) return true;
    TCHAR * result1 = _tcsstr(strin, _T("DLL"));
    int position1 = result1 - strin;
    int substringLength1 = _tcslen(strin) - position1;
    if (substringLength1 < _tcslen(strin)) return true;
    return false;
}



