// TestDyn.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>

typedef __declspec(dllimport) std::pair<int *, int *>(__cdecl * KeysFunc)(int, int);
typedef __declspec(dllimport) int * (__cdecl * Encode)(int *, int, int *);
typedef __declspec(dllimport) int * (__cdecl * Decode)(int *, int, int *);
typedef __declspec(dllimport) int(__cdecl * BinPow)(int, int, int);

void print(int * arr, int size) {
	std::cout << "[";
	for (int i = 0; i < size; ++i) {
		if (i == size - 1) std::cout << arr[i];
		else std::cout << arr[i] << " ";
	}
	std::cout << "]" << std::endl;
}

int main()
{
	HINSTANCE getDLL = LoadLibrary(TEXT("C:\\Users\\Alexzander\\Documents\\Visual Studio 2015\\Projects\\Lab2TestLab\\Release\\lab2TestLab.dll"));
	if (!getDLL) {
		printf("can`t locate .dll\n");
		return -1;
	}
	KeysFunc gk = (KeysFunc)GetProcAddress(getDLL, "getKeys");
	Decode dk = (Decode)GetProcAddress(getDLL, "decode");
	Encode ek = (Encode)GetProcAddress(getDLL, "encode");
	if (!gk || !dk || !ek) {
		printf("could not locate the function");
		return -1;
	}
	std::pair<int *, int *> param = gk(3, 11);
	std::cout << "Keys: opened {" << param.first[0] << ", " << param.first[1] << "}\nclosed {" << param.second[0] << ", " << param.second[1] << "}" << std::endl;
	int a[10] = { 2, 1, 11, 1, 6, 30, 13, 2, 3, 7 };
	std::cout << "Message to code: " << std::endl;
	print(a, 10);
	int * answer = ek(a, 10, param.first);
	std::cout << "Coded message: " << std::endl;
	print(answer, 10);
	int * decoded = dk(answer, 10, param.second);
	FreeLibrary(getDLL);
	std::cout << "Decoded message: " << std::endl;
	print(decoded, 10);
	system("pause");
    return 0;
}

