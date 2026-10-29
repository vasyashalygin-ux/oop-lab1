#pragma once
// Минимальный "фреймворк" для автотестов, которые запускаются из main.
#include <iostream>
#include <string>

inline int g_passed = 0;
inline int g_failed = 0;

// Проверка условия: печатает только упавшие тесты, чтобы вывод был коротким.
inline void check(bool condition, const std::string& name) {
    if (condition) {
        ++g_passed;
    } else {
        ++g_failed;
        std::cout << "  [FAIL] " << name << "\n";
    }
}

// Проверка, что код бросает исключение (некорректные данные должны отвергаться явно).
template <typename Func>
void checkThrows(Func func, const std::string& name) {
    try {
        func();
        check(false, name + " (исключение не брошено)");
    } catch (const std::exception&) {
        check(true, name);
    }
}

// Итог: возвращает true, если все тесты прошли.
inline bool printTestSummary() {
    std::cout << "Тестов пройдено: " << g_passed << ", провалено: " << g_failed << "\n";
    return g_failed == 0;
}
