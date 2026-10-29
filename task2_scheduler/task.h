#pragma once
// Предметная область: задача. Иммутабельное значение (только геттеры).
#include <string>

class Task {
public:
    // name пустое или deadline < 0 -> исключение
    Task(int id, const std::string& name, int priority, int deadline);

    int id() const;
    const std::string& name() const;
    int priority() const;   // чем больше, тем важнее
    int deadline() const;   // срок выполнения — номер дня (чем меньше, тем раньше)

    std::string toString() const;

private:
    int id_;
    std::string name_;
    int priority_;
    int deadline_;
};
