#pragma once
#include <iostream>

_declspec(dllexport)
std::pair<int *, int*> getKeys(int, int);
_declspec(dllexport)
int * encode(int *, int, int *);
_declspec(dllexport)
int * decode(int *, int, int *);
_declspec(dllexport)
bool isSimple(int);
_declspec(dllexport)
int bin_pow(int, int, int);
_declspec(dllexport)
char getChar();
_declspec(dllexport)
void printChar(char);
_declspec(dllexport)
void setConsoleCoding(const char *);
_declspec(dllexport)
void printString(char *, int);
_declspec(dllexport)
char * getString(int);
