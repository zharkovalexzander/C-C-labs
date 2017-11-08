// CrossWord.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstdarg>
#include <windows.h>

//Заменяем тип vector<string> ключевым словом "words" для удобства и так с остальными тоже
#define words vector<string>
#define section pair<int, words>
#define dictionary vector<section>

using namespace std;

//класс, который хранит и обрабатывает слова
class Dictionary {
public:
    //конструктор: открывает файл с указанным именем и читает каждое слово
    //для каждого слова, он вызывает фукнцию arrange(line)
    Dictionary(string dictionaryName) {
        last_length = 0;
        queryWas = false;
        string line;
        ifstream file(dictionaryName);
        if(file.is_open())
        {
            while(getline(file, line))
            {
                arrange(line);
            }
            file.close();
        }
    };
    //класс который хранит инофрмацию о запросах (пример: найти букву "а" на второй позиции)
    class Query {
    public:
        Query(int pos, char let) {
            position = pos;
            letter = toupper(let);
        };
        //функция, которая определяет, подходит ли входная строка, такому запросу (проверят есть ли символ на позиции)
        bool fits(string str) {
            if (position >= str.length()) {
                throw exception("Not valid character position");
            }
            return (str.at(position) == letter);
        };

    private:
        int position;
        char letter;
    };
    //фунция которая выполняет поиск по всем словам
    void execute_query(int length) {
        //проверяет есть ли массив со словами длинны "length"
        if (contains_length(length)) {
            last_length = length;
            words * temp;
            //если да, то находим массив с нужными словами
            for (int i = 0; i < wordcontainer.size(); ++i) {
                if (wordcontainer[i].first == length) {
                    temp = &wordcontainer[i].second;
                    //после того как нашли, проверяем каждое слово, подходит ли оно
                    for (int j = 0; j < temp->size(); ++j) {
                        string word = temp->at(j);
                        try {
                            //если подходит (фунцкия check_for_queries(word, qurs), то добавляем слово в список найденных)
                            if (check_for_queries(word, qurs)) {
                                last_query_result.push_back(word);
                                last_query_result_indexes.push_back(j);
                                cout << word << " : " << last_query_result_indexes.size() << endl;
                            }
                        }
                        catch (const exception& e) {

                        }
                    }
                }
            }
            queryWas = true;
        }
    };
    //очищаем выбранные параметры запросов
    void clearQuery() {
        last_length = 0;
        last_query_result_indexes.clear();
        qurs.clear();
        last_query_result.clear();
        queryWas = false;
    };
    //добавляем новый запрос на поиск буквы на позиции
    void addNewQuery(int pos, char letter) {
        qurs.push_back(Query(pos, letter));
    };
    //выбрать слово в кроссворд, которое стоит на определенной позицции
    bool choose(int position) {
        if (position > last_query_result.size() || position <= 0) {
            cout << "Invalid position. Try again" << endl;
            system("pause");
            return false;
        }
        string elem;
        //это тип, который отвечает за свойства консоли, нужен для того чтобы цвет у слова поменять
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        for (int i = 0; i < wordcontainer.size(); ++i) {
            if (wordcontainer[i].first == last_length) {
                //нашли массив со словами нужной длинны
                int index = last_query_result_indexes[position - 1];
                //в цикле нашли слово на позиции
                for (int j = 0; j < last_query_result_indexes.size(); ++j) {
                    if (last_query_result_indexes[j] > index) {
                        last_query_result_indexes[j] = last_query_result_indexes[j] - 1;
                    }
                }
                //переместили выбранное слово из словаря в кроссворд (удалили в базе, вставили в использованные)
                elem = wordcontainer[i].second.at(index);
                wordcontainer[i].second.erase(wordcontainer[i].second.begin() + index);
                used[i].second.push_back(elem);
                last_query_result.erase(last_query_result.begin() + position - 1);
                last_query_result_indexes.erase(last_query_result_indexes.begin() + position - 1);
                cout << "'";
                SetConsoleTextAttribute(hConsole, 14);
                cout << elem;
                SetConsoleTextAttribute(hConsole, 7);
                cout << "' was chosen for crossword" << endl;
                return true;
            }
        }
        return false;
    };
    //возвращает строку с информацией о кроссворде в виде:
    //4: СЛОН МЫШЬ
    //5: ЧАШКА ЛОЖКА
    string toString() {
        string answer;
        for (int i = 0; i < used.size(); ++i) {
            if (used[i].second.size() == 0) {
                continue;
            }
            //append присоединяет в конец текущей строки, указаную в параметре ex. string a = "a"; a.append("b"); => a == "ab"
            answer.append(to_string(used[i].first));
            answer.append(" : ");
            for (int j = 0; j < used[i].second.size(); j++) {
                answer.append(used[i].second[j]);
                if (j < used[i].second.size() - 1) {
                    answer.append(" ");
                }
            }
            answer.append("\n");
        }
        //удаляем перенос строки в конце
        return answer.substr(0, answer.size() - 1);
    }
    //печатает строку, которую возвращает toString() в консоль
    void print() {
        cout << toString() << endl;
    };
    //печатает строку, которую возвращает toString() в файл с именем crossword.txt
    void exportToFile() {
        ofstream fout("crossword.txt");
        fout << toString();
    };
    //показывает слова, которые подходят искомым параметрам при последнем поиске (слова которые еще можно выбрать для словаря) 
    void showAvailable() {
        for (int i = 0; i < last_query_result.size(); ++i) {
            cout << last_query_result[i] << " : " << (i + 1) << endl;
        }
    };
    void delCross() {
        for (int i = 0; i < used.size(); ++i) {
            while (used[i].second.size() > 0) {
                add_to_section(used[i].first, used[i].second.back());
                used[i].second.pop_back();
            }
        }
    }
    //добавляет новое слово в базу слов
    void add(string line) {
        arrange(line);
    };

private:
    //проверяет, подходит ли слово к списку всех запросов к слову на поиск
    bool check_for_queries(string word, vector<Query> arguments) {
        for (int i = 0; i < arguments.size(); ++i) {
            if (!arguments[i].fits(word)) {
                return false;
            }
        }
        return true;
    };
    //решает в какой массив записать новое слово
    void arrange(string line) {
        int length = line.size();
        if (!add_to_section(length, line)) {
            throw std::exception("Error while adding");
        }
    };
    //проверяет, существует ли массив со словами, длина которых равна "len"
    bool contains_length(int len) {
        for (int i = 0; i < wordcontainer.size(); ++i) {
            if (wordcontainer[i].first == len) {
                return true;
            }
        }
        return false;
    };
    //добавляет новое слово в базу
    bool add_to_section(int size, string line) {
        //переводим строку в верхний регистр
        transform(line.begin(), line.end(), line.begin(), ::toupper);
        //если массив со словами такой же длинны уже есть, добавляем слово туда
        if (contains_length(size)) {
            for (int i = 0; i < wordcontainer.size(); ++i) {
                if (wordcontainer[i].first == size) {
                    wordcontainer[i].second.push_back(line);
                    //сортируем массив слов
                    sort(wordcontainer[i].second.begin(), wordcontainer[i].second.end());
                    return true;
                }
            }     
        }
        //если не нашли, создаем новый массив и добавляем его в список массивов, и вставляем туда слово
        else {
            words w_temp;
            w_temp.push_back(line);
            wordcontainer.push_back(section(size, w_temp));     
            sort(wordcontainer.begin(), wordcontainer.end());
            words used_w_temp;
            used.push_back(section(size, used_w_temp));
            sort(used.begin(), used.end());
            return true;
        }
        return false;
    };

    dictionary wordcontainer;
    dictionary used;
    words last_query_result;
    vector<int> last_query_result_indexes;
    vector<Query> qurs;
    int last_length;
    bool queryWas;
};
void askForDelete(Dictionary * dict) {
    cout << "Ви бажаете очистити список слів, вибраних для кросворду? Y/N" << endl;
    char a;
    cin >> a;
    if (a == 'Y') {
        dict->delCross();
        cout << "Усі слова були видалені" << endl;
        system("pause");
    }
}
//функция, которая помогает пользователю запустить процесс поиска слова
void startQueryAlgo(Dictionary * dict) {
    system("cls");
    int size;
    dict->clearQuery();
    cout << "Введіть розмір шуканого слова:" << endl;
    cin >> size;
    cout << "Далі виконуйте запити у вигляді\nлітера: а\nпозиція: 4" << endl << "Якщо ви хочете закінчити вибір обмежень введіть '-1' у позиції" << endl;
    char b = 0;
    int pos = 0;
    while (pos != -1) {
        cout << "Літера:" << endl;
        cin >> hex >> b;
        cout << "Позиція:" << endl;
        cin >> pos;
        if (pos == -1) {
            break;
        }
        if (pos < 1 || pos > size) {
            cout << "Позиція літери була введено не правильно. Повторіть, будь ласка!" << endl;
            continue;
        }
        dict->addNewQuery(pos - 1, b);

        cout << "Літера '" << b << "' буде шукатись у " << pos << " позиції" << endl;
    }
    system("cls");
    dict->execute_query(size);
    int exit = 0;
    cout << "Поруч зі словами розташовані їх індекси у виборці. Щоб додати слово до кросворду введіть бажаний індекс. Введіть число менше за нкль або нуль для того щоб завершити вибірку." << endl;
    system("pause");
    system("cls");
    while (exit != -1) {
        dict->showAvailable();
        cin >> dec >> exit;
        if (exit == -1) {
            break;
        }
        dict->choose(exit);
    }
    cout << "Виконано" << endl;
    system("pause");
    system("cls");
};
//функция, которая запускат главные методы
int starter() {
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    string path;
    cout << "Введіть шлях до файлу-словника в форматі '../приклад.txt'. Якщо файл-словник знаходиться у директорії програми, вам варто лише вказати його назву та розширення." << endl;
    cin >> path;
    //проверяем существует ли файл
    ifstream check(path);
    if (!check.good()) {
        cout << "Вибачте, але програма не змогла знайти вказаний вами файл, спробуйте ще раз." << endl;
        system("pause");
        system("cls");
        starter();
        return 0;
    }
    check.close();
    cout << "Знайдено. Завантажуємо слова..." << endl;
    Dictionary example(path);
    system("cls");
    string line;
    int code = -1;
    while (code != 0) {
        cout << "Введіть цифру для виконання певної функції зі словником:" << endl << "Цифра 0 : Завершити програму" << endl << "Цифра 1 : Виконати алгоритм пошуку слів та їх вибору для кросворду" << endl << "Цифра 2 : Відобразити слова, що були вибрані для кросворду" << endl << "Цифра 3 : Завантажити вибрані слова до файлу" << endl << "Цифра 4 : Додати слово до словника" << endl;
        cin >> code;
        switch (code) {
            case 1: startQueryAlgo(&example); break;
            case 2: system("cls"); example.print(); askForDelete(&example); break;
            case 3: example.exportToFile(); cout << "Кросворд був експортований до файлу" << endl; system("pause"); break;
            case 4: cout << "Введіть слово, щоб додати його до бази даних." << endl; cin >> line; example.add(line); break;
            case 0: example.exportToFile(); return 0;
            default: code = -1; break;
        }
        system("cls");
    }
    system("pause");
};

int main()
{
    return starter();
};

