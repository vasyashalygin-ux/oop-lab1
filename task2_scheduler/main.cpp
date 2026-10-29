// Лабораторная работа 1, задание 2, вариант 4: планировщик задач на двоичной куче.
#include <iostream>
#include <string>
#include <vector>

#include "../common/testing.h"
#include "rules.h"
#include "scheduler.h"
#include "tests.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace {

std::vector<Task> makeTasks() {
    return {Task(1, "Написать отчёт", 2, 5), Task(2, "Починить сервер", 5, 3), Task(3, "Ответить на письма", 1, 1),
            Task(4, "Подготовить презентацию", 4, 7), Task(5, "Созвон с клиентом", 5, 2)};
}

// Одна функция демонстрации для любого правила — правило задаётся параметром шаблона.
template <typename Rule>
void demoWithRule(const std::string& title) {
    std::cout << "\n--- Правило: " << title << " ---\n";
    Scheduler<Rule> scheduler;
    for (const auto& task : makeTasks()) {
        scheduler.addTask(task);
        std::cout << "Добавлена: " << task.toString() << "\n";
    }
    std::cout << "Ожидает задач: " << scheduler.pendingCount() << "\n";
    std::cout << "Следующая (без удаления): " << scheduler.peekNext().toString() << "\n";
    std::cout << "Ожидает задач: " << scheduler.pendingCount() << "\n";

    std::cout << "Выполняем одну: " << scheduler.executeNext().toString() << "\n";
    std::cout << "Ожидает задач: " << scheduler.pendingCount() << "\n";

    std::cout << "Выполняем остальные по порядку:\n";
    for (const auto& task : scheduler.executeAll()) {
        std::cout << "  выполнена " << task.toString() << "\n";
    }
    std::cout << "Ожидает задач: " << scheduler.pendingCount() << "\n";
}

void demoEmpty() {
    std::cout << "\n--- Пустой планировщик ---\n";
    Scheduler<ByPriority> scheduler;
    std::cout << "Есть задачи: " << (scheduler.hasTasks() ? "да" : "нет") << "\n";
    try {
        scheduler.peekNext();
    } catch (const std::exception& e) {
        std::cout << "peekNext: " << e.what() << "\n";
    }
    try {
        scheduler.executeNext();
    } catch (const std::exception& e) {
        std::cout << "executeNext: " << e.what() << "\n";
    }
}

}  // namespace

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << "=== Автотесты ===\n";
    runTask2Tests();
    bool ok = printTestSummary();

    std::cout << "\n=== Демонстрация ===";
    demoWithRule<ByPriority>("сначала больший приоритет");
    demoWithRule<ByDeadline>("сначала ранний срок");
    demoEmpty();
    return ok ? 0 : 1;
}
