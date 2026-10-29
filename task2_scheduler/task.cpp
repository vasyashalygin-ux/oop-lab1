#include "task.h"

#include <stdexcept>

Task::Task(int id, const std::string& name, int priority, int deadline)
    : id_(id), name_(name), priority_(priority), deadline_(deadline) {
    if (name.empty()) {
        throw std::invalid_argument("У задачи должно быть название");
    }
    if (deadline < 0) {
        throw std::invalid_argument("Срок выполнения не может быть отрицательным");
    }
}

int Task::id() const { return id_; }
const std::string& Task::name() const { return name_; }
int Task::priority() const { return priority_; }
int Task::deadline() const { return deadline_; }

std::string Task::toString() const {
    return "#" + std::to_string(id_) + " \"" + name_ + "\" (приоритет " + std::to_string(priority_) +
           ", срок: день " + std::to_string(deadline_) + ")";
}
