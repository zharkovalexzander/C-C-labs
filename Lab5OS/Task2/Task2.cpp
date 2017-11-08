#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include <vector>

#define ANSI 0x0A
#define UNICODE_LITTLE_ENDIAN 0x0B
#define UNICODE_BIG_ENDIAN 0x0C
#define UTF_8 0x0D

DWORD returnCode = 1;
FILETIME fileTime;
WIN32_FIND_DATA fileData;
TCHAR toFind[] = _T("*.txt");
LPDWORD readBytes;

struct FileReader {
public:
    FileReader(TCHAR * name) {
        nmae = name;
    };

    DWORD defineCoding() {
        FILE * handle = _tfopen(nmae, _T("rb+"));
        char tempElem;
        fread(&tempElem, sizeof(char), 1, handle);
        if (tempElem == 0xFFFFFFEF)
        {
            fread(&tempElem, sizeof(char), 1, handle);
            if (tempElem == 0xFFFFFFBB)
            {
                fread(&tempElem, sizeof(char), 1, handle);
                if (tempElem == 0xFFFFFFBF)
                {
                    fclose(handle);
                    return UTF_8;
                }
            }
        }
        else
        {
            if (tempElem == 0xFFFFFFFE)
            {
                fread(&tempElem, sizeof(char), 1, handle);
                if (tempElem == 0xFFFFFFFF)
                {
                    fclose(handle);
                    return UNICODE_BIG_ENDIAN;
                }
            }
            else
            {
                if (tempElem == 0xFFFFFFFF)
                {
                    fread(&tempElem, sizeof(char), 1, handle);
                    if (tempElem == 0xFFFFFFFE)
                    {
                        fclose(handle);
                        return UNICODE_LITTLE_ENDIAN;
                    }
                }
            }
        }
        fclose(handle);
        return ANSI;
    };

    int fileSize() {
        FILE * tmp = _tfopen(nmae, _T("rb+"));
        long int save_pos, size_of_file;
        save_pos = ftell(tmp);
        fseek(tmp, 0L, SEEK_END);
        size_of_file = ftell(tmp);
        fseek(tmp, save_pos, SEEK_SET);
        fclose(tmp);
        return(size_of_file);
    }

    void readLinesAndSymbols(DWORD fileSize) {
        FILE * handle = _tfopen(nmae, _T("rb+"));
        char tempElem;
        DWORD code = defineCoding();
        int i = 0;
        switch (code)
        {
        case UTF_8: i = 3; break;
        case UNICODE_BIG_ENDIAN: 
            case UNICODE_LITTLE_ENDIAN: i = 2; break;
            default: i = 0; break;
        }
        
        for (int j = 0; j < i; ++j) {
            fread(&tempElem, sizeof(char), 1, handle);
        }

        std::vector<int> length;
        char * _text = new char[this->fileSize()];
        int top = 0;
        while (!feof(handle))
        {
            fread(&tempElem, sizeof(char), 1, handle);
            if (code == UNICODE_LITTLE_ENDIAN && tempElem == 0x0D) {
                length.push_back((top + 1) / 2);
                _text = new char[this->fileSize()];
                top = 0;
                for (int j = 0; j < 3; ++j) {
                    fread(&tempElem, sizeof(char), 1, handle);
                }
                continue;
            }
            else {
                if (code == UNICODE_BIG_ENDIAN && tempElem == 0x0D) {
                    length.push_back((top) / 2);
                    _text = new char[this->fileSize()];
                    top = 0;
                    for (int j = 0; j < 2; ++j) {
                        fread(&tempElem, sizeof(char), 1, handle);
                    }
                    continue;
                }
                else {
                    if (tempElem == 0x0D) {
                        length.push_back(top);
                        _text = new char[this->fileSize()];
                        top = 0;
                        fread(&tempElem, sizeof(char), 1, handle);
                        continue;
                    }
                }
            }
            _text[top++] = tempElem;        
        }


        if (code == UNICODE_LITTLE_ENDIAN || code == UNICODE_BIG_ENDIAN) {
            length.push_back(((top) / 2));
        }
        else {
            length.push_back(top - 1);
        }


        _tprintf(_T("For file '%s' of size '%d' was found '%d' lines \n\n"), nmae, fileSize, length.size());

        for (int k = 0; k < static_cast<int>(length.size()); ++k) {
                _tprintf(_T("%d-st line length : %d\n"), (k + 1), length[k]);
        }

        _tprintf(_T("\n\n"));
    }

private:
    TCHAR * nmae;
};

int main()
{
    HANDLE handle = FindFirstFile(toFind, &fileData);
    TCHAR * lowTime = _tgetenv(_T("LOWTIME"));
    TCHAR * hiTime = _tgetenv(_T("HIGHTIME"));
    if (!lowTime == NULL && !hiTime == NULL) {
        _stscanf(lowTime, _T("%d"), &fileTime.dwLowDateTime);
        _stscanf(hiTime, _T("%d"), &fileTime.dwHighDateTime);
        if (handle != INVALID_HANDLE_VALUE) {
            returnCode = 0;
            do {
                if (CompareFileTime(&fileTime, &fileData.ftLastWriteTime) < 0)
                {
                    FileReader * reader = new FileReader(fileData.cFileName);
                    reader->readLinesAndSymbols(fileData.nFileSizeLow);            
                }
            } while (FindNextFile(handle, &fileData));
        }
    }
}

