// LabNo1.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include <locale.h>
#include <Windows.h>

int comp(const void * var1, const void * var2)
{
	const wchar_t * e1 = *(const wchar_t **)var1;
	const wchar_t * e2 = *(const wchar_t **)var2;
	return wcscmp(e1, e2);
}

void defaultCoding()
{
	switch (sizeof(TCHAR))
	{
		case 1: 
			printf("System's symbol coding method is MBCS\n"); 
			break;
		case 2: 
			printf("System's symbol coding method is UNICODE\n"); 
			break;
		default:
			printf("System's symbol coding size is more than 2 bytes. This is very interesting fact!\n");
			break;
	}
};

wchar_t ** convertToUnicode(char ** familyNames, int array_size, int inner_char_size)
{
	wchar_t ** unicodeFamilyNames = new wchar_t *[array_size];
	for (int i = 0; i < array_size; ++i)
	{
		unicodeFamilyNames[i] = new wchar_t[inner_char_size];
		MultiByteToWideChar(CP_ACP, 0, familyNames[i], inner_char_size, unicodeFamilyNames[i], inner_char_size);
	}
	return unicodeFamilyNames;
};

void printWithT(wchar_t ** names_array, int array_length)
{
	for (int i = 0; i < array_length; ++i)
		_tprintf("%s\n", names_array[i]);
};

void printWithMB(wchar_t ** names_array, int array_length)
{
	for (int i = 0; i < array_length; ++i)
		MessageBoxW(0, names_array[i], names_array[i], MB_OK);
};

char ** convertToASCII(wchar_t ** names_array, int array_size, int inner_char_size)
{
	char ** ASCIIFamilyNames = new char *[array_size];
	for (int i = 0; i < array_size; ++i)
	{
		ASCIIFamilyNames[i] = new char[inner_char_size];
		WideCharToMultiByte(CP_ACP, 0, names_array[i], inner_char_size, ASCIIFamilyNames[i], inner_char_size, NULL, NULL);
		printf("%s\n", ASCIIFamilyNames[i]);
	}
	return ASCIIFamilyNames;
};




int main()
{
	_tsetlocale(LC_ALL, _T("Russian"));

	defaultCoding();

	char ** familyNames = new char *[3];
	for (int i = 0; i < 3; ++i)
		familyNames[i] = new char[40];
	familyNames[0] = "Жарков Александр Геннадьевич";
	familyNames[1] = "Жаркова Галина Юрьевна";
	familyNames[2] = "Жарков Геннадий Владимирович";

	char ** ptrToNames = familyNames;
	wchar_t ** unicodeNames =  convertToUnicode(familyNames, 3, 40);

	printWithT(unicodeNames, 3);
	printWithMB(unicodeNames, 3);

	qsort(unicodeNames, 3, sizeof(wchar_t *), comp);

	char ** ACSIInames = convertToASCII(unicodeNames, 3, 40);

	system("pause");
    return 0;
};

