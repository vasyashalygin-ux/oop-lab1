#pragma once
#include <exception>
#include <functional>
#include <iostream>
#include <string>
using namespace std;

int passed = 0;
int failed = 0;

// Проверяет условие. Печатает только упавшие тесты.
void check(bool condition, string name) {
    if (condition) {
        passed++;
    } else {
        failed++;
        cout << "  [FAIL] " << name << endl;
    }
}

// Проверяет, что код бросает исключение.
void checkThrows(function<void()> code, string name) {
    try {
        code();
        check(false, name + " (исключение не брошено)");
    } catch (exception&) {
        check(true, name);
    }
}

void printResult() {
    cout << "Тестов пройдено: " << passed << ", провалено: " << failed << endl;
}
