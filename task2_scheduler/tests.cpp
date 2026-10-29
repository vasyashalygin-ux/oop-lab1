#include "tests.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

#include "../common/testing.h"
#include "rules.h"
#include "scheduler.h"

namespace {

// Ещё одно правило для строк — показывает, что куча не привязана к Task.
struct ShorterFirst {
    bool operator()(const std::string& a, const std::string& b) const {
        return a.size() < b.size();
    }
};

std::vector<int> ids(const std::vector<Task>& tasks) {
    std::vector<int> result;
    for (const auto& t : tasks) {
        result.push_back(t.id());
    }
    return result;
}

void testHeap() {
    BinaryHeap<int, std::less<int>> minHeap;
    check(minHeap.empty() && minHeap.size() == 0, "новая куча пуста");
    checkThrows([&] { minHeap.top(); }, "top пустой кучи -> исключение");
    checkThrows([&] { minHeap.pop(); }, "pop пустой кучи -> исключение");

    for (int v : {5, 3, 8, 1, 9, 1, 7}) {
        minHeap.push(v);
        check(minHeap.isValid(), "инвариант после push");
    }
    check(minHeap.size() == 7, "size после добавления");
    check(minHeap.top() == 1, "top возвращает минимум");
    check(minHeap.size() == 7, "top не удаляет элемент");

    std::vector<int> popped;
    while (!minHeap.empty()) {
        popped.push_back(minHeap.pop());
        check(minHeap.isValid(), "инвариант после pop");
    }
    check(popped == std::vector<int>({1, 1, 3, 5, 7, 8, 9}), "min-куча: извлечение по возрастанию (с дубликатами)");

    // Та же реализация, другое правило.
    BinaryHeap<int, std::greater<int>> maxHeap;
    for (int v : {5, 3, 8, 1}) maxHeap.push(v);
    check(maxHeap.pop() == 8 && maxHeap.pop() == 5, "max-куча: извлечение по убыванию");

    // Та же реализация, другой тип элементов.
    BinaryHeap<std::string, ShorterFirst> strHeap;
    strHeap.push("ccc");
    strHeap.push("a");
    strHeap.push("bb");
    check(strHeap.pop() == "a" && strHeap.pop() == "bb", "куча строк с собственным правилом");

    // Один элемент.
    BinaryHeap<int, std::less<int>> single;
    single.push(42);
    check(single.pop() == 42 && single.empty(), "куча из одного элемента");

    // Большой случайный набор: результат совпадает с сортировкой.
    std::srand(1);
    BinaryHeap<int, std::less<int>> big;
    std::vector<int> values;
    for (int i = 0; i < 1000; ++i) {
        int v = std::rand() % 100;
        values.push_back(v);
        big.push(v);
    }
    check(big.isValid(), "инвариант на 1000 элементах");
    std::sort(values.begin(), values.end());
    std::vector<int> fromHeap;
    while (!big.empty()) fromHeap.push_back(big.pop());
    check(fromHeap == values, "1000 элементов извлекаются в отсортированном порядке");
}

void testTask() {
    checkThrows([] { Task(1, "", 1, 1); }, "задача без названия -> исключение");
    checkThrows([] { Task(1, "x", 1, -1); }, "отрицательный срок -> исключение");
    Task t(7, "Отчёт", 3, 10);
    check(t.id() == 7 && t.name() == "Отчёт" && t.priority() == 3 && t.deadline() == 10, "геттеры задачи");
    check(t.toString().find("#7") != std::string::npos, "toString задачи");
}

void testRules() {
    Task a(1, "a", 5, 10), b(2, "b", 3, 2), c(3, "c", 5, 2);
    check(ByPriority()(a, b) && !ByPriority()(b, a), "ByPriority: больший приоритет раньше");
    check(ByPriority()(a, c), "ByPriority: при равенстве — меньший id раньше");
    check(ByDeadline()(b, a), "ByDeadline: ранний срок раньше");
    check(ByDeadline()(b, c), "ByDeadline: при равенстве — меньший id раньше");
    check(!ByPriority()(a, a), "правило строгое: элемент не приоритетнее сам себя");
}

std::vector<Task> sampleTasks() {
    return {Task(1, "Написать отчёт", 2, 5), Task(2, "Починить сервер", 5, 3), Task(3, "Ответить на письма", 1, 1),
            Task(4, "Подготовить презентацию", 4, 7), Task(5, "Созвон с клиентом", 5, 2)};
}

void testScheduler() {
    Scheduler<ByPriority> empty;
    check(!empty.hasTasks() && empty.pendingCount() == 0, "пустой планировщик");
    checkThrows([&] { empty.peekNext(); }, "peekNext у пустого -> исключение");
    checkThrows([&] { empty.executeNext(); }, "executeNext у пустого -> исключение");
    check(empty.executeAll().empty(), "executeAll у пустого -> пустой список");

    Scheduler<ByPriority> byPriority;
    Scheduler<ByDeadline> byDeadline;
    for (const auto& t : sampleTasks()) {
        byPriority.addTask(t);
        byDeadline.addTask(t);
    }
    check(byPriority.pendingCount() == 5, "количество ожидающих задач");
    checkThrows([&] { byPriority.addTask(Task(1, "дубль", 1, 1)); }, "повторный id -> исключение");
    check(byPriority.pendingCount() == 5, "после ошибки состав не изменился");

    check(byPriority.peekNext().id() == 2, "peekNext по приоритету");
    check(byPriority.pendingCount() == 5, "peekNext не удаляет задачу");
    check(byDeadline.peekNext().id() == 3, "peekNext по сроку");

    check(byPriority.executeNext().id() == 2, "executeNext возвращает выполненную задачу");
    check(byPriority.pendingCount() == 4, "executeNext удаляет задачу");
    byPriority.addTask(Task(2, "Снова сервер", 0, 9));
    check(byPriority.pendingCount() == 5, "id выполненной задачи можно использовать снова");

    check(ids(byPriority.executeAll()) == std::vector<int>({5, 4, 1, 3, 2}),
          "порядок по приоритету (равные приоритеты — по id)");
    check(ids(byDeadline.executeAll()) == std::vector<int>({3, 5, 2, 1, 4}), "порядок по сроку");
    check(!byPriority.hasTasks() && !byDeadline.hasTasks(), "после executeAll очередь пуста");
}

}  // namespace

void runTask2Tests() {
    testHeap();
    testTask();
    testRules();
    testScheduler();
}
