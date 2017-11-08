// CrossWord.cpp: ���������� ����� ����� ��� ����������� ����������.
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

//�������� ��� vector<string> �������� ������ "words" ��� �������� � ��� � ���������� ����
#define words vector<string>
#define section pair<int, words>
#define dictionary vector<section>

using namespace std;

//�����, ������� ������ � ������������ �����
class Dictionary {
public:
    //�����������: ��������� ���� � ��������� ������ � ������ ������ �����
    //��� ������� �����, �� �������� ������� arrange(line)
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
    //����� ������� ������ ���������� � �������� (������: ����� ����� "�" �� ������ �������)
    class Query {
    public:
        Query(int pos, char let) {
            position = pos;
            letter = toupper(let);
        };
        //�������, ������� ����������, �������� �� ������� ������, ������ ������� (�������� ���� �� ������ �� �������)
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
    //������ ������� ��������� ����� �� ���� ������
    void execute_query(int length) {
        //��������� ���� �� ������ �� ������� ������ "length"
        if (contains_length(length)) {
            last_length = length;
            words * temp;
            //���� ��, �� ������� ������ � ������� �������
            for (int i = 0; i < wordcontainer.size(); ++i) {
                if (wordcontainer[i].first == length) {
                    temp = &wordcontainer[i].second;
                    //����� ���� ��� �����, ��������� ������ �����, �������� �� ���
                    for (int j = 0; j < temp->size(); ++j) {
                        string word = temp->at(j);
                        try {
                            //���� �������� (������� check_for_queries(word, qurs), �� ��������� ����� � ������ ���������)
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
    //������� ��������� ��������� ��������
    void clearQuery() {
        last_length = 0;
        last_query_result_indexes.clear();
        qurs.clear();
        last_query_result.clear();
        queryWas = false;
    };
    //��������� ����� ������ �� ����� ����� �� �������
    void addNewQuery(int pos, char letter) {
        qurs.push_back(Query(pos, letter));
    };
    //������� ����� � ���������, ������� ����� �� ������������ ��������
    bool choose(int position) {
        if (position > last_query_result.size() || position <= 0) {
            cout << "Invalid position. Try again" << endl;
            system("pause");
            return false;
        }
        string elem;
        //��� ���, ������� �������� �� �������� �������, ����� ��� ���� ����� ���� � ����� ��������
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        for (int i = 0; i < wordcontainer.size(); ++i) {
            if (wordcontainer[i].first == last_length) {
                //����� ������ �� ������� ������ ������
                int index = last_query_result_indexes[position - 1];
                //� ����� ����� ����� �� �������
                for (int j = 0; j < last_query_result_indexes.size(); ++j) {
                    if (last_query_result_indexes[j] > index) {
                        last_query_result_indexes[j] = last_query_result_indexes[j] - 1;
                    }
                }
                //����������� ��������� ����� �� ������� � ��������� (������� � ����, �������� � ��������������)
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
    //���������� ������ � ����������� � ���������� � ����:
    //4: ���� ����
    //5: ����� �����
    string toString() {
        string answer;
        for (int i = 0; i < used.size(); ++i) {
            if (used[i].second.size() == 0) {
                continue;
            }
            //append ������������ � ����� ������� ������, �������� � ��������� ex. string a = "a"; a.append("b"); => a == "ab"
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
        //������� ������� ������ � �����
        return answer.substr(0, answer.size() - 1);
    }
    //�������� ������, ������� ���������� toString() � �������
    void print() {
        cout << toString() << endl;
    };
    //�������� ������, ������� ���������� toString() � ���� � ������ crossword.txt
    void exportToFile() {
        ofstream fout("crossword.txt");
        fout << toString();
    };
    //���������� �����, ������� �������� ������� ���������� ��� ��������� ������ (����� ������� ��� ����� ������� ��� �������) 
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
    //��������� ����� ����� � ���� ����
    void add(string line) {
        arrange(line);
    };

private:
    //���������, �������� �� ����� � ������ ���� �������� � ����� �� �����
    bool check_for_queries(string word, vector<Query> arguments) {
        for (int i = 0; i < arguments.size(); ++i) {
            if (!arguments[i].fits(word)) {
                return false;
            }
        }
        return true;
    };
    //������ � ����� ������ �������� ����� �����
    void arrange(string line) {
        int length = line.size();
        if (!add_to_section(length, line)) {
            throw std::exception("Error while adding");
        }
    };
    //���������, ���������� �� ������ �� �������, ����� ������� ����� "len"
    bool contains_length(int len) {
        for (int i = 0; i < wordcontainer.size(); ++i) {
            if (wordcontainer[i].first == len) {
                return true;
            }
        }
        return false;
    };
    //��������� ����� ����� � ����
    bool add_to_section(int size, string line) {
        //��������� ������ � ������� �������
        transform(line.begin(), line.end(), line.begin(), ::toupper);
        //���� ������ �� ������� ����� �� ������ ��� ����, ��������� ����� ����
        if (contains_length(size)) {
            for (int i = 0; i < wordcontainer.size(); ++i) {
                if (wordcontainer[i].first == size) {
                    wordcontainer[i].second.push_back(line);
                    //��������� ������ ����
                    sort(wordcontainer[i].second.begin(), wordcontainer[i].second.end());
                    return true;
                }
            }     
        }
        //���� �� �����, ������� ����� ������ � ��������� ��� � ������ ��������, � ��������� ���� �����
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
    cout << "�� ������� �������� ������ ���, �������� ��� ���������? Y/N" << endl;
    char a;
    cin >> a;
    if (a == 'Y') {
        dict->delCross();
        cout << "�� ����� ���� �������" << endl;
        system("pause");
    }
}
//�������, ������� �������� ������������ ��������� ������� ������ �����
void startQueryAlgo(Dictionary * dict) {
    system("cls");
    int size;
    dict->clearQuery();
    cout << "������ ����� �������� �����:" << endl;
    cin >> size;
    cout << "��� ��������� ������ � ������\n�����: �\n�������: 4" << endl << "���� �� ������ �������� ���� �������� ������ '-1' � �������" << endl;
    char b = 0;
    int pos = 0;
    while (pos != -1) {
        cout << "˳����:" << endl;
        cin >> hex >> b;
        cout << "�������:" << endl;
        cin >> pos;
        if (pos == -1) {
            break;
        }
        if (pos < 1 || pos > size) {
            cout << "������� ����� ���� ������� �� ���������. ��������, ���� �����!" << endl;
            continue;
        }
        dict->addNewQuery(pos - 1, b);

        cout << "˳���� '" << b << "' ���� �������� � " << pos << " �������" << endl;
    }
    system("cls");
    dict->execute_query(size);
    int exit = 0;
    cout << "����� � ������� ���������� �� ������� � �������. ��� ������ ����� �� ��������� ������ ������� ������. ������ ����� ����� �� ���� ��� ���� ��� ���� ��� ��������� ������." << endl;
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
    cout << "��������" << endl;
    system("pause");
    system("cls");
};
//�������, ������� �������� ������� ������
int starter() {
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    string path;
    cout << "������ ���� �� �����-�������� � ������ '../�������.txt'. ���� ����-������� ����������� � �������� ��������, ��� ����� ���� ������� ���� ����� �� ����������." << endl;
    cin >> path;
    //��������� ���������� �� ����
    ifstream check(path);
    if (!check.good()) {
        cout << "�������, ��� �������� �� ������ ������ �������� ���� ����, ��������� �� ���." << endl;
        system("pause");
        system("cls");
        starter();
        return 0;
    }
    check.close();
    cout << "��������. ����������� �����..." << endl;
    Dictionary example(path);
    system("cls");
    string line;
    int code = -1;
    while (code != 0) {
        cout << "������ ����� ��� ��������� ����� ������� � ���������:" << endl << "����� 0 : ��������� ��������" << endl << "����� 1 : �������� �������� ������ ��� �� �� ������ ��� ���������" << endl << "����� 2 : ³��������� �����, �� ���� ������ ��� ���������" << endl << "����� 3 : ����������� ������ ����� �� �����" << endl << "����� 4 : ������ ����� �� ��������" << endl;
        cin >> code;
        switch (code) {
            case 1: startQueryAlgo(&example); break;
            case 2: system("cls"); example.print(); askForDelete(&example); break;
            case 3: example.exportToFile(); cout << "�������� ��� ������������� �� �����" << endl; system("pause"); break;
            case 4: cout << "������ �����, ��� ������ ���� �� ���� �����." << endl; cin >> line; example.add(line); break;
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

