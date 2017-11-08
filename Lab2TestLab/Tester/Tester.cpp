// Tester.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include "../Lab2TestLab/RSA.h"

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
	std::pair<int *, int *> pa = getKeys(5, 7);
	std::cout << "Keys: opened {" << pa.first[0] << ", " << pa.first[1] << "}\nclosed {" << pa.second[0] << ", " << pa.second[1] << "}" << std::endl;
	int a[10] = { 2, 1, 11, 1, 5, 17, 20, 17, 13, 7 };
	std::cout << "Message to code: " << std::endl;
	print(a, 10);
	int * answer = encode(a, 10, pa.first);
	std::cout << "Coded message: " << std::endl;
	print(answer, 10);
	int * decoded = decode(answer, 10, pa.second);
	std::cout << "Decoded message: " << std::endl;
	print(decoded, 10);
	/*char tester;
	setConsoleCoding("French");
	tester = getChar();
	printChar(tester);
	char * prince = getString(5); 
	printString(prince, 5);
	setConsoleCoding("Ukranian");
	tester = getChar();
	printChar(tester);
	setConsoleCoding("Arabic");
	tester = getChar();
	printChar(tester);
	setConsoleCoding("Hebrew");
	tester = getChar();
	printChar(tester);*/
	system("pause");
    return 0;
}

