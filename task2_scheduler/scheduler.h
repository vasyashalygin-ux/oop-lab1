#pragma once
// Планировщик — предметная логика. Использует BinaryHeap как хранилище,
// не вмешиваясь в её устройство. Правило порядка Rule — параметр шаблона.
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "binary_heap.h"
#include "task.h"

template <typename Rule>
class Scheduler {
public:
    // Идентификатор должен быть уникален среди ожидающих задач.
    void addTask(const Task& task) {
        if (ids_.count(task.id()) > 0) {
            throw std::invalid_argument("Задача с id " + std::to_string(task.id()) + " уже есть");
        }
        heap_.push(task);
        ids_.insert(task.id());
    }

    // Следующая задача без удаления из очереди.
    const Task& peekNext() const {
        if (!hasTasks()) {
            throw std::logic_error("Нет ожидающих задач");
        }
        return heap_.top();
    }

    // Выполнение следующей задачи: она удаляется из очереди и возвращается.
    Task executeNext() {
        if (!hasTasks()) {
            throw std::logic_error("Нет ожидающих задач");
        }
        Task task = heap_.pop();
        ids_.erase(task.id());
        return task;
    }

    // Последовательно выполняет все задачи; возвращает их в порядке выполнения.
    std::vector<Task> executeAll() {
        std::vector<Task> done;
        while (hasTasks()) {
            done.push_back(executeNext());
        }
        return done;
    }

    std::size_t pendingCount() const { return heap_.size(); }
    bool hasTasks() const { return !heap_.empty(); }

private:
    BinaryHeap<Task, Rule> heap_;
    std::set<int> ids_;  // вспомогательно: для быстрой проверки уникальности id
};
