// TESTIO.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "../Lab2TestLab/RSA.h"
#include <iostream>
#include <Windows.h>
#include <string>

int main()
{
	SetConsoleCP(1252);
	SetConsoleOutputCP(1252);
	/*const char * text = "Je demande pardon aux enfants d'avoir dôdié ce livre";*/
	int m = abcde();
	std::cout << m;
	system("pause");
    return 0;
}

