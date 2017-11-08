#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <sstream>

using namespace std;
using namespace std::chrono;

class Logger {
public:
    Logger() {
        logStream = ofstream("log.txt");
    }

    void log(string message) {
        logStream << message;
    }

    void close() {
        logStream.close();
    }
private:
    ofstream logStream;
};

class SortableCollection {
public:
    SortableCollection() {
        coll = new int[0];
        length = 0;
        size = 0;
        logFile = Logger();
    }

    SortableCollection(int len) {
        coll = new int[len];
        length = 0;
        size = len;
        logFile = Logger();
    }

    SortableCollection(int integers[], int len) {
        coll = new int[len];
        for (int i = 0; i < len; ++i) {
            coll[i] = integers[i];
        }
        length = len;
        size = len;
        logFile = Logger();
    }

    SortableCollection(string filename) {
        ifstream fin(filename);
        if (fin.good()) {
            int sizer, element;
            fin >> sizer;
            coll = new int[sizer];
            size = sizer;
            length = 0;
            for (int i = 0; i < sizer; ++i) {
                fin >> element;
                coll[length++] = element;
            }
            logFile = Logger();
        }
        else {
            cout << "File was not found. Creating default array size 0" << endl;
            coll = new int[0];
            length = 0;
            size = 0;
            logFile = Logger();
        }
        fin.close();
    }

    void add(int element) {
        resize(length + 1);
        coll[length++] = element;
    }

    string format() {
        string arr = "[";
        for (int i = 0; i < length; ++i) {
            arr.append(to_string(coll[i]));
            if (i < length - 1) {
                arr.append(", ");
            }
        }
        arr.append("]");
        return arr;
    }

    void print() {
        cout << format() << endl;
    }

    void bubblesort() {
        int * tmp = new int[size];
        for (int i = 0; i < size; ++i) {
            tmp[i] = coll[i];
        }
        string forLogger;
        //
        int step = 0;
        cout << step++ << " : " << format() << endl;
        getchar();
        getchar();
        //
        forLogger.append("=============================================\r\n");
        forLogger.append("Excecuting BubbleSort...\r\n");
        forLogger.append("=============================================\r\n");
        forLogger.append("Current array: ");
        forLogger.append(format());
        forLogger.append("\r\n");
        forLogger.append("---------------------\r\n");
        forLogger.append("Running from end\r\n");
        forLogger.append("---------------------\r\n");
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        for (int i = 0; i < length; ++i) {
            for (int j = length - 1; j > i; --j) {
                forLogger.append("Cursor is above '");
                forLogger.append(to_string(coll[j]));
                forLogger.append("'\r\n");
                if (coll[j - 1] > coll[j]) {
                    forLogger.append("Found that '");
                    forLogger.append(to_string(coll[j]));
                    forLogger.append("' is less than '");
                    forLogger.append(to_string(coll[j - 1]));
                    forLogger.append("'. Swapping...\r\nCurrent array: ");
                    swapelements(j - 1, j);
                    //
                    cout << step++ << " : " << format() << endl;
                    getchar();
                    //
                    forLogger.append(format());
                    forLogger.append("\r\n#\r\n");
                }
                else {
                    forLogger.append("Skipping...\r\n#\r\n");
                }
            }
        }
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(t2 - t1).count();
        forLogger.append("############\r\nBubbleSort was successfully done : ");
        forLogger.append(to_string(duration));
        forLogger.append(" ms\r\n");
        logFile.log(forLogger);
        cout << "BubbleSort was successfully done: " << to_string(duration) << " ms, " << step << " steps." << endl;
        for (int i = 0; i < size; ++i) {
            coll[i] = tmp[i];
        }
    }

    void mergesort() {
        int * tmp = new int[size];
        for (int i = 0; i < size; ++i) {
            tmp[i] = coll[i];
        }
        string forLogger;
        forLogger.append("=============================================\r\n");
        forLogger.append("Excecuting MergeSort...\r\n");
        forLogger.append("=============================================\r\n");
        forLogger.append("Current array: ");
        forLogger.append(format());
        forLogger.append("\r\n");
        forLogger.append("-----------------------------------------------------\r\n");
        //
        int step = 0;
        cout << step++ << " : " << format() << endl;
        getchar();
        getchar();
        //
        logFile.log(forLogger);
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        mergeSort(0, length - 1);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        forLogger.clear();
        auto duration = duration_cast<milliseconds>(t2 - t1).count();
        forLogger.append("MergeSort was successfully done : ");
        forLogger.append(to_string(duration));
        forLogger.append(" ms\r\n");
        logFile.log(forLogger);
        cout << "MergeSort was successfully done: " << to_string(duration) << " ms, " << c_sort << " steps." << endl;
        for (int i = 0; i < size; ++i) {
            coll[i] = tmp[i];
        }
    }

    void insertionsort() {
        int * tmp = new int[size];
        for (int i = 0; i < size; ++i) {
            tmp[i] = coll[i];
        }
        string forLogger;
        forLogger.append("=============================================\r\n");
        forLogger.append("Excecuting InsertionSort...\r\n");
        forLogger.append("=============================================\r\n");
        forLogger.append("Current array: ");
        forLogger.append(format());
        forLogger.append("\r\n");
        forLogger.append("-----------------------------------------------------\r\n");
        //
        int step = 0;
        cout << step++ << " : " << format() << endl;
        getchar();
        getchar();
        //
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        for (int i = 1; i < length; ++i) {
            int temp = coll[i];
            forLogger.append("'");
            forLogger.append(to_string(temp));
            forLogger.append("' was chosen\r\n");
            int index = i - 1;
            forLogger.append("Current array: ");
            forLogger.append(format());
            forLogger.append("\r\n");
            while (coll[index] > temp && index >= 0) {
                coll[index + 1] = coll[index];
                --index;
            }
            forLogger.append("Element was inserted at '");
            coll[index + 1] = temp;
            //
            int step = 0;
            cout << step++ << " : " << format() << endl;
            getchar();
            //
            forLogger.append(to_string(index + 1));
            forLogger.append("'\r\n");
            forLogger.append("Current array: ");
            forLogger.append(format());
            forLogger.append("\r\n");
            forLogger.append("-----------------------------------------------\r\n");
        }
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        forLogger.append("InsertionSort was successfully done : ");
        auto duration = duration_cast<milliseconds>(t2 - t1).count();
        forLogger.append(to_string(duration));
        forLogger.append(" ms\r\n");
        logFile.log(forLogger);
        cout << "InsertionSort was successfully done: " << to_string(duration) << " ms, " << step << " steps." << endl;
        for (int i = 0; i < size; ++i) {
            coll[i] = tmp[i];
        }
    }

    void quicksort() {
        int * tmp = new int[size];
        for (int i = 0; i < size; ++i) {
            tmp[i] = coll[i];
        }
        string forLogger;
        forLogger.append("=============================================\r\n");
        forLogger.append("Excecuting QuickSort...\r\n");
        forLogger.append("=============================================\r\n");
        forLogger.append("Current array: ");
        forLogger.append(format());
        forLogger.append("\r\n");
        forLogger.append("-----------------------------------------------------\r\n");
        logFile.log(forLogger);
        //
        int step = 0;
        cout << step << " : " << format() << endl;
        getchar();
        getchar();
        //
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        qusort(0, length - 1);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        forLogger.clear();
        forLogger.append("Result array: ");
        forLogger.append(format());
        forLogger.append("\r\n");
        forLogger.append("-----------------------------------------------------\r\n");
        forLogger.append("QuickSort was successfully done : ");
        auto duration = duration_cast<milliseconds>(t2 - t1).count();
        forLogger.append(to_string(duration));
        forLogger.append(" ms\r\n");
        logFile.log(forLogger);
        cout << "QuickSort was successfully done: " << to_string(duration) << " ms, " << c_sort << " steps." << endl;;
        for (int i = 0; i < size; ++i) {
            coll[i] = tmp[i];
        }
    }

    void combsort() {
        int * tmp = new int[size];
        for (int i = 0; i < size; ++i) {
            tmp[i] = coll[i];
        }
        string forLogger;
        forLogger.append("=============================================\r\n");
        forLogger.append("Excecuting CombSort...\r\n");
        forLogger.append("=============================================\r\n");
        forLogger.append("Current array: ");
        forLogger.append(format());
        forLogger.append("\r\n");
        forLogger.append("-----------------------------------------------------\r\n");
        //
        int step = 0;
        cout << step++ << " : " << format() << endl;
        getchar();
        getchar();
        //
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        int range = length - 1;
        bool isSwapped;
        while (range >= 1) {
            forLogger.append("Gap beetween elements is: '");
            forLogger.append(to_string(range));
            forLogger.append("'\r\n");
            for (int i = 0; (i + range) < length; ++i) {
                forLogger.append("Checking elements at '");
                forLogger.append(to_string(i));
                forLogger.append("' and '");
                forLogger.append(to_string(i + range));
                forLogger.append("'...\r\n");
                if (coll[i] > coll[i + range]) {
                    forLogger.append("First element was greater than second. Swaping...\r\n");
                    swapelements(i, i + range);
                    //
                    cout << step++ << " : " << format() << endl;
                    getchar();
                    //
                    forLogger.append("Current array: ");
                    forLogger.append(format());
                    forLogger.append("\r\n");
                }
            }
            range /= factor;
            forLogger.append("Gap was reduced by dividing on factor value\r\n----------------------------------------------\r\n");
        }
        forLogger.append("Using modified BubbleSort to complete current sorting...\r\n");
        for (int i = 0; i < length - 1; ++i) {
            isSwapped = false;
            for (int j = 0; j < (length - 1 - i); ++j) {
                if (coll[j] > coll[j + 1]) {
                    forLogger.append("Elements at '");
                    forLogger.append(to_string(j));
                    forLogger.append("' and '");
                    forLogger.append(to_string(j + 1));
                    forLogger.append("' were swapped\r\n");
                    swapelements(j, j + 1);
                    //
                    cout << step++ << " : " << format() << endl;
                    getchar();
                    //
                    forLogger.append("Current array: ");
                    forLogger.append(format());
                    forLogger.append("\r\n");
                    forLogger.append("-----------------------------------------------------\r\n");
                    isSwapped = true;
                }
            }
            if (!isSwapped) {
                forLogger.append("There wasn`t any swapping during full iteration. Stopping...\r\n");
                break;
            }
        }
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        forLogger.append("CombSort was successfully done : ");
        auto duration = duration_cast<milliseconds>(t2 - t1).count();
        forLogger.append(to_string(duration));
        forLogger.append(" ms\r\n");
        logFile.log(forLogger);
        cout << "CombSort was successfully done: " << to_string(duration) << " ms, " << step << " steps." << endl;
        for (int i = 0; i < size; ++i) {
            coll[i] = tmp[i];
        }
    }

    void selectionsort() {
        int * tmp = new int[size];
        for (int i = 0; i < size; ++i) {
            tmp[i] = coll[i];
        }
        string forLogger;
        forLogger.append("=============================================\r\n");
        forLogger.append("Excecuting SelectionSort...\r\n");
        forLogger.append("=============================================\r\n");
        forLogger.append("Current array: ");
        forLogger.append(format());
        forLogger.append("\r\n");
        forLogger.append("-----------------------------------------------------\r\n");
        //
        int step = 0;
        cout << step++ << " : " << format() << endl;
        getchar();
        getchar();
        //
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        for (int i = 0; i < length - 1; ++i) {
            forLogger.append("'");
            forLogger.append(to_string(coll[i]));
            forLogger.append("' was selected for sorting\r\n");
            int pivot = i;
            for (int j = i + 1; j < length; ++j) {
                pivot = (coll[j] < coll[pivot]) ? j : pivot;
            }
            forLogger.append("'");
            forLogger.append(to_string(coll[pivot]));
            forLogger.append("' element for swap\r\n");
            swapelements(i, pivot);
            //
            cout << step++ << " : " << format() << endl;
            getchar();
            //
            forLogger.append("Current array: ");
            forLogger.append(format());
            forLogger.append("\r\n");
            forLogger.append("-----------------------------------------------\r\n");
        }
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        forLogger.append("SelectionSort was successfully done : ");
        auto duration = duration_cast<milliseconds>(t2 - t1).count();
        forLogger.append(to_string(duration));
        forLogger.append(" ms\r\n");
        logFile.log(forLogger);
        cout << "SelectionSort was successfully done: " << to_string(duration) << " ms, " << step << " steps." << endl;
        for (int i = 0; i < size; ++i) {
            coll[i] = tmp[i];
        }
    }

    void randomize(int low, int high) {
        srand(time(NULL));
        for (int i = 0; i < size; ++i) {
            coll[i] = rand() % high + low;
        }
        length = size;
    }

    void printToFile(string filename = "SortedArray.txt") {
        ofstream fout(filename);
        fout << length << endl;
        for (int i = 0; i < length; ++i) {
            fout << coll[i];
            if (i < length - 1) {
                fout << " ";
            }
        }
        fout.close();
    }

private:
    void qusort(int start, int end) {
        string forLogger;
        //
        c_sort = 1;
        //
        if (start >= end) {
            return;
        }
        int leftPos = start;
        int rightPos = end;
        int currentPos = leftPos - (leftPos - rightPos) / 2;
        forLogger.append("Pivot element was selected at position '");
        forLogger.append(to_string(currentPos));
        forLogger.append("'\r\n");
        while (leftPos < rightPos) {
            while (leftPos < currentPos && (coll[leftPos] <= coll[currentPos])) {
                leftPos++;
            }
            forLogger.append("Greater element was found at position '");
            forLogger.append(to_string(leftPos));
            forLogger.append("'\r\n");
            while (rightPos > currentPos && (coll[currentPos] <= coll[rightPos])) {
                rightPos--;
            }
            forLogger.append("Less element was found at position '");
            forLogger.append(to_string(rightPos));
            forLogger.append("'\r\n");
            if (leftPos < rightPos) {
                forLogger.append("Swapping into sections...\r\n");
                swapelements(leftPos, rightPos);
                //
                cout << c_sort++ << " : " << format() << endl;
                getchar();
                //
                forLogger.append("Current array: [");
                for (int i = start; i <= end; ++i) {
                    forLogger.append(to_string(coll[i]));
                    if (i < end) {
                        forLogger.append(", ");
                    }
                }
                forLogger.append("]\r\n");
                if (leftPos == currentPos) {
                    currentPos = rightPos;
                }
                else {
                    if (rightPos == currentPos) {
                        currentPos = leftPos;
                    }
                }
            }
        }
        forLogger.append("Array splits into [");
        for (int i = start; i <= currentPos; ++i) {
            forLogger.append(to_string(coll[i]));
            if (i < currentPos) {
                forLogger.append(", ");
            }
        }
        forLogger.append("] and [");
        for (int i = currentPos + 1; i <= end; ++i) {
            forLogger.append(to_string(coll[i]));
            if (i < end) {
                forLogger.append(", ");
            }
        }
        forLogger.append("]\r\n--------------------------------------------------------------------------------------------------\r\n");
        logFile.log(forLogger);
        qusort(start, currentPos);
        qusort(currentPos + 1, end);
    }

    void merge(int start, int splitindex, int end) {
        c_sort = 1;
        string forLogger;
        forLogger.append("Creating temporary sorted list for elements...\r\n#######################################\r\n");
        int leftPos = start;
        int rightPos = splitindex + 1;
        int currentPos = 0;
        int * temp = new int[end - start + 1];
        while (rightPos <= end && leftPos <= splitindex) {
            forLogger.append("Comparing '");
            forLogger.append(to_string(coll[leftPos]));
            forLogger.append("' with '");
            forLogger.append(to_string(coll[rightPos]));
            forLogger.append("'\r\n");
            if (coll[leftPos] < coll[rightPos]) {
                forLogger.append("'");
                forLogger.append(to_string(coll[leftPos]));
                forLogger.append("' was chosen as next element\r\n");
                temp[currentPos++] = coll[leftPos++];
            }
            else {
                forLogger.append("'");
                forLogger.append(to_string(coll[rightPos]));
                forLogger.append("' was chosen as next element\r\n");
                temp[currentPos++] = coll[rightPos++];

            }
            forLogger.append("Current list: [");
            for (int i = 0; i < currentPos; ++i) {
                forLogger.append(to_string(temp[i]));
                if (i < currentPos - 1) {
                    forLogger.append(", ");
                }
            }
            forLogger.append("]\r\n");
        }
        while (rightPos <= end) {
            forLogger.append("Pushing rest elements to list...\r\n");
            temp[currentPos++] = coll[rightPos++];
        }
        while (leftPos <= splitindex) {
            forLogger.append("Pushing rest elements to list...\r\n");
            temp[currentPos++] = coll[leftPos++];
        }
        forLogger.append("Current list: [");
        for (int i = 0; i < currentPos; ++i) {
            forLogger.append(to_string(temp[i]));
            if (i < currentPos - 1) {
                forLogger.append(", ");
            }
        }
        forLogger.append("]\r\nMerging sorted list with main array...\r\n");
        forLogger.append("Before: ");
        forLogger.append(format());
        forLogger.append("\r\nAfter: ");
        for (int i = 0; i < (end - start + 1); i++) {
            coll[start + i] = temp[i];
            //
            cout << c_sort++ << " : " << format() << endl;
            getchar();
            //
        }
        forLogger.append(format());
        forLogger.append("\r\n######################################\r\n");
        logFile.log(forLogger);
        delete[] temp;
    }

    void mergeSort(int start, long end) {
        string forLogger;
        if (start < end) {
            int splitindex = (start + end) >> 1;
            forLogger.append("'");
            forLogger.append(to_string(coll[splitindex]));
            forLogger.append("' was selected as pivot in range ");
            forLogger.append(to_string(start));
            forLogger.append(":");
            forLogger.append(to_string(end));
            forLogger.append(". Array splits into [");
            for (int i = start; i <= splitindex; ++i) {
                forLogger.append(to_string(coll[i]));
                if (i < splitindex) {
                    forLogger.append(", ");
                }
            }
            forLogger.append("] and [");
            for (int i = splitindex + 1; i <= end; ++i) {
                forLogger.append(to_string(coll[i]));
                if (i < end) {
                    forLogger.append(", ");
                }
            }
            forLogger.append("]\r\n#\r\n");
            logFile.log(forLogger);
            mergeSort(start, splitindex);
            mergeSort(splitindex + 1, end);
            forLogger.append("----------------------------------------\r\nStart merging...\r\n----------------------------------------\r\n");
            logFile.log(forLogger);
            merge(start, splitindex, end);
        }
    }

    void swapelements(int i, int j) {
        int temp = coll[i];
        coll[i] = coll[j];
        coll[j] = temp;
    }

    void resize(int sizeble) {
        if (sizeble - size > 0) {
            int oldsize = size;
            int newsize = oldsize + (oldsize >> 1);
            if (newsize - sizeble < 0) {
                newsize = sizeble;
            }
            copy(newsize);
        }
    }

    void copy(int size) {
        int * answer = new int[length];
        for (int i = 0; i < length; ++i) {
            answer[i] = coll[i];
        }
        coll = new int[size];
        for (int i = 0; i < length; ++i) {
            coll[i] = answer[i];
        }
        this->size = size;
    }

    int * coll;
    int length;
    int size;
    const double factor = 1.247330950103979;
    Logger logFile;
    int c_sort = 0;
};

SortableCollection * filereader() {
    system("cls");
    cout << "Enter file path:" << endl;
    string name;
    cin >> name;
    return new SortableCollection(name);
}

SortableCollection * selectSize() {
    system("cls");
    cout << "Enter array size:" << endl;
    int size;
    cin >> size;
    return new SortableCollection(size);
}

void random(SortableCollection * coll) {
    system("cls");
    cout << "Enter min and max values of random numbers (separate values by whitespace)" << endl;
    int x, y;
    cin >> x >> y;
    if (x >= y) {
        cout << "Bad values. Try again." << endl;
        random(coll);
    }
    coll->randomize(x, y);
}

void addElementsToCollection(SortableCollection * coll) {
    string line;
    while (getline(cin, line)) {
        if (line.empty()) { 
            continue; 
        }
        istringstream is(line);
        int val;
        while (is >> val) {
            coll->add(val);
        }
        break;
    }
    system("pause");
}

void run() {
    SortableCollection *(*ReadMyFile)() = filereader;
    SortableCollection *(*FixedLen)() = selectSize;
    void(*Randomizer)(SortableCollection * coll) = random;
    int keyCode;
    cout << "Sorting algorithms were prepaired by Yulia Troyanova." << endl;
    cout << "1 or default. Create new empty array" << endl << "2. Create array from file" << endl << "3. Create array with predefined size" << endl;
    cin >> keyCode;
    SortableCollection * coll;
    switch (keyCode) {
    case 1:
    default:
        coll = new SortableCollection(0); break;
    case 2:
        coll = ReadMyFile(); break;
    case 3:
        coll = FixedLen(); break;
    }
    system("cls");
    int exitCode = -1;
    while (exitCode != 0) {
        cout << "0. Exit" << endl << "1. Add element to collection" << endl << "2. Print collection on screen" << endl << "3. Print collection to file" << endl << "4. Randomize all elements of collection" << endl << "5. Bubble Sort" << endl;
        cout << "6. Merge Sort" << endl << "7. Insertion Sort" << endl << "8. Quick Sort" << endl << "9. Selection Sort" << endl << "10. Comb Sort" << endl;
        cin >> exitCode;
        switch (exitCode) {
        case 1: addElementsToCollection(coll); break;
        case 2: system("cls"); coll->print(); system("pause"); break;
        case 3: coll->printToFile(); system("pause"); break;
        case 4: Randomizer(coll); break;
        case 5: coll->bubblesort(); system("pause"); break;
        case 6: coll->mergesort(); system("pause"); break;
        case 7: coll->insertionsort(); system("pause"); break;
        case 8: coll->quicksort(); system("pause"); break;
        case 9: coll->selectionsort(); system("pause"); break;
        case 10: coll->combsort(); system("pause"); break;
        case 0: return;
        default: exitCode = -1; break;
        }
        system("cls");
    }
}

int main()
{
    run();
    return 0;
}

