// Lab2TestLab.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>

bool isSimple(int n) {
	if (n <= 1) {
		return false;
	}
	for (int i = 2; i <= n / 2; ++i) {
		if ((n % i) == 0) {
			return false;
		}
	}
	return true;
};

int bin_pow(int base, int p, int K) {
	if (p == 1) {
		return base;
	}

	if (p % 2 == 0) {
		int t = bin_pow(base, p / 2, K);
		return t * t % K;
	}
	else {
		return bin_pow(base, p - 1, K) * base % K;
	}
}

std::pair<int *, int*> getKeys(int param1, int param2)
{
	std::pair<int *, int*> openClose;
	int module = param1 * param2;
	int EulerFunc = (param1 - 1) * (param2 - 1);
	std::vector<int> simples;
	for (int i = 2; i <= EulerFunc; ++i) {
		if (!(EulerFunc % i) == 0 && isSimple(i) && i < EulerFunc) {
			simples.push_back(i);
		}
	}
	srand(time(NULL));
	int e = simples[std::rand() % (simples.size())];
	int tp = 2;
	while ((((e * tp) % EulerFunc) != 1) || (tp == e)) {
		++tp;
	}
	int * a1 = new int[2];
	a1[0] = e;
	a1[1] = module;
	int * a2 = new int[2];
	a2[0] = tp;
	a2[1] = module;
	openClose = std::make_pair(a1, a2);
	return openClose;

};

int * encode(int * input, int size, int * key)
{
	int * result = new int[size];
	for (int i = 0; i < size; ++i) {
		if(input[i] > key[1]) throw std::invalid_argument("value is greater than key " + key[1]);
		result[i] = bin_pow(input[i], key[0], key[1]);
	}
	return result;
};

int * decode(int * input, int size, int * key)
{
	int * result = new int[size];
	for (int i = 0; i < size; ++i) {
		result[i] = bin_pow(input[i], key[0], key[1]);
	}
	return result;
};

char getChar() {
	char tmp;
	std::cin >> std::hex >> tmp;
	return tmp;
};

char * getString(int length) {
	char * chr = new char[length];
	char b;
	for (int i = 0; i < length; ++i) {
		std::cin >> std::hex >> b;
		chr[i] = b;
	}
	return chr;
};

void printChar(char ch) {
	std::cout << ch << std::endl;
};

void printString(char * stri, int size) {
	for (int i = 0; i < size; ++i)
		std::cout << stri[i];
	std::cout << "\n";
};

void setConsoleCoding(const char * language) {
	char * langs[26] = { "Polish", "Czech", "Slovak", "Hungarian", "Slovene", "Bosnian", "Croatian", "Serbian", "Romanian", "Albanian", "Serbian C", "Bulgarian", "Macedonian", "Russian", "Ukranian", "French", "Spanish", "Deutch", "English", "Scandinavian", "Greek", "Turkish", "Hebrew", "Arabic", "Baltic", "Vietnamese"  };
	int codes[26] = { 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1251, 1251, 1251, 1251, 1251, 1252, 1252, 1252, 1252, 1252, 1253, 1254, 1255, 1256, 1257, 1258 };
	int pos = -1;
	for (int i = 0; i < 26; ++i) {
		if (strcmp(langs[i], language) == 0) {
			pos = i;
			std::cout << "Language is set to " << langs[pos] << std::endl;
			break;
		}
	}

	if(pos < 0) throw std::invalid_argument("such language is abcent");
	
	SetConsoleCP(codes[pos]);
	SetConsoleOutputCP(codes[pos]);
};

