#include <cstdlib>
#include <functional>
#include "../common/testing.h"
#include "scheduler.h"

// ---------- Тесты ----------

void testHeap() {
    Heap<int, less<int>> minHeap;
    check(minHeap.empty() && minHeap.size() == 0, "новая куча пуста");
    checkThrows([&] { minHeap.top(); }, "top пустой кучи");
    checkThrows([&] { minHeap.pop(); }, "pop пустой кучи");

    for (int x : {5, 3, 8, 1, 9, 1, 7}) minHeap.push(x);
    check(minHeap.size() == 7, "размер после добавления");
    check(minHeap.top() == 1, "top возвращает наименьший");
    check(minHeap.size() == 7, "top не удаляет элемент");

    vector<int> result;
    while (!minHeap.empty()) result.push_back(minHeap.pop());
    check(result == vector<int>({1, 1, 3, 5, 7, 8, 9}), "pop отдаёт по возрастанию (с повторами)");

    Heap<int, greater<int>> maxHeap;
    for (int x : {5, 3, 8, 1}) maxHeap.push(x);
    check(maxHeap.pop() == 8 && maxHeap.pop() == 5, "то же правило наоборот: сначала наибольший");

    Heap<string, less<string>> words;
    words.push("pear");
    words.push("apple");
    words.push("fig");
    check(words.pop() == "apple" && words.pop() == "fig", "куча работает и со строками");

    Heap<int, less<int>> single;
    single.push(42);
    check(single.pop() == 42 && single.empty(), "куча из одного элемента");

    Heap<int, less<int>> big;
    for (int i = 0; i < 100; i++) big.push((i * 37) % 101);
    bool sorted = true;
    int previous = -1;
    while (!big.empty()) {
        int x = big.pop();
        if (x < previous) sorted = false;
        previous = x;
    }
    check(sorted, "100 элементов извлекаются по порядку");
}

vector<int> ids(vector<Task> tasks) {
    vector<int> result;
    for (Task t : tasks) result.push_back(t.id);
    return result;
}

// Одни и те же 5 задач: id получатся 1..5
template <typename Rule>
void fill(Scheduler<Rule>& scheduler) {
    scheduler.addTask("Написать отчёт", 2, 5);
    scheduler.addTask("Починить сервер", 5, 3);
    scheduler.addTask("Ответить на письма", 1, 1);
    scheduler.addTask("Подготовить презентацию", 4, 7);
    scheduler.addTask("Созвон с клиентом", 5, 2);
}

void testScheduler() {
    Scheduler<ByPriority> empty;
    check(!empty.hasTasks() && empty.pendingCount() == 0, "пустой планировщик");
    checkThrows([&] { empty.peekNext(); }, "peekNext без задач");
    checkThrows([&] { empty.executeNext(); }, "executeNext без задач");
    check(empty.executeAll().empty(), "executeAll без задач");

    Scheduler<ByPriority> byPriority;
    Scheduler<ByDeadline> byDeadline;
    fill(byPriority);
    fill(byDeadline);
    check(byPriority.pendingCount() == 5, "количество ожидающих задач");
    check(byPriority.addTask("ещё одна", 0, 9) == 6, "id выдаются по порядку и не повторяются");
    check(byPriority.pendingCount() == 6, "после добавления задач стало больше");

    check(byDeadline.peekNext().id == 3, "peekNext по сроку");
    check(byDeadline.pendingCount() == 5, "peekNext не удаляет задачу");
    check(byDeadline.executeNext().id == 3, "executeNext возвращает выполненную задачу");
    check(byDeadline.pendingCount() == 4, "executeNext удаляет задачу из очереди");

    Scheduler<ByPriority> p;
    Scheduler<ByDeadline> d;
    fill(p);
    fill(d);
    check(ids(p.executeAll()) == vector<int>({2, 5, 4, 1, 3}), "порядок по приоритету (равные: кто раньше добавлен)");
    check(ids(d.executeAll()) == vector<int>({3, 5, 2, 1, 4}), "порядок по сроку");
    check(!p.hasTasks() && !d.hasTasks(), "после executeAll очередь пуста");
}

void runTests() {
    testHeap();
    testScheduler();
    printResult();
}

// ---------- Демонстрация ----------

void print(Task t) {
    cout << "#" << t.id << " " << t.name << " (приоритет " << t.priority << ", срок: день " << t.deadline << ")" << endl;
}

// Один и тот же код для любого правила: правило передаётся параметром шаблона
template <typename Rule>
void demoWithRule(string title) {
    cout << endl << "--- " << title << " ---" << endl;
    Scheduler<Rule> scheduler;
    fill(scheduler);
    cout << "Ожидает задач: " << scheduler.pendingCount() << endl;

    cout << "Следующая (не удаляя): ";
    print(scheduler.peekNext());
    cout << "Ожидает задач: " << scheduler.pendingCount() << endl;

    cout << "Выполняем: ";
    print(scheduler.executeNext());
    cout << "Ожидает задач: " << scheduler.pendingCount() << endl;

    cout << "Остальные по порядку:" << endl;
    for (Task t : scheduler.executeAll()) {
        cout << "  ";
        print(t);
    }
    cout << "Ожидает задач: " << scheduler.pendingCount() << endl;
}

void demoEmpty() {
    cout << endl << "--- Пустой планировщик ---" << endl;
    Scheduler<ByPriority> scheduler;
    cout << "Есть задачи: " << (scheduler.hasTasks() ? "да" : "нет") << endl;
    try {
        scheduler.executeNext();
    } catch (exception& e) {
        cout << "Ошибка (так и задумано): " << e.what() << endl;
    }
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");  // чтобы русский текст нормально выводился в консоли Windows
#endif
    cout << "=== Тесты ===" << endl;
    runTests();
    cout << endl << "=== Демонстрация ===";
    demoWithRule<ByPriority>("Правило: сначала больший приоритет");
    demoWithRule<ByDeadline>("Правило: сначала ранний срок");
    demoEmpty();
    return failed == 0 ? 0 : 1;
}
