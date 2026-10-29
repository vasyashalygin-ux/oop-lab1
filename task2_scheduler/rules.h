#pragma once
// Правила порядка выполнения задач — функторы, которые передаются в кучу
// параметром шаблона. Новое правило = новый функтор, куча не меняется.
//
// Эквивалентные элементы: если по основному критерию задачи равны,
// раньше выполняется задача с меньшим id. Так порядок всегда однозначный.
#include "task.h"

// Сначала задачи с большим приоритетом.
struct ByPriority {
    bool operator()(const Task& a, const Task& b) const {
        if (a.priority() != b.priority()) {
            return a.priority() > b.priority();
        }
        return a.id() < b.id();
    }
};

// Сначала задачи с более ранним сроком.
struct ByDeadline {
    bool operator()(const Task& a, const Task& b) const {
        if (a.deadline() != b.deadline()) {
            return a.deadline() < b.deadline();
        }
        return a.id() < b.id();
    }
};
