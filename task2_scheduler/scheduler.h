#pragma once
#include <string>
#include <vector>
#include "heap.h"

struct Task {
    int id;          // уникальный: выдаёт планировщик
    string name;
    int priority;    // чем больше, тем важнее
    int deadline;    // срок (номер дня): чем меньше, тем срочнее
};

// Правила порядка. Куча получает правило параметром шаблона, сама она не меняется.
// Если задачи равны по главному признаку, раньше идёт добавленная раньше (меньший id).

// Сначала задачи с большим приоритетом
struct ByPriority {
    bool operator()(Task a, Task b) const {
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.id < b.id;
    }
};

// Сначала задачи с более ранним сроком
struct ByDeadline {
    bool operator()(Task a, Task b) const {
        if (a.deadline != b.deadline) return a.deadline < b.deadline;
        return a.id < b.id;
    }
};

template <typename Rule>
class Scheduler {
    Heap<Task, Rule> waiting;  // ожидающие задачи
    int nextId = 1;

public:
    // Возвращает id новой задачи
    int addTask(string name, int priority, int deadline) {
        waiting.push(Task{nextId, name, priority, deadline});
        return nextId++;
    }

    // Следующая задача, не удаляя её из очереди
    Task peekNext() const { return waiting.top(); }

    // Выполняет следующую задачу: убирает её из очереди и возвращает
    Task executeNext() { return waiting.pop(); }

    // Выполняет все задачи по порядку
    vector<Task> executeAll() {
        vector<Task> done;
        while (hasTasks()) done.push_back(executeNext());
        return done;
    }

    int pendingCount() const { return waiting.size(); }
    bool hasTasks() const { return !waiting.empty(); }
};
