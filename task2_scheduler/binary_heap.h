#pragma once
// Обобщённая двоичная куча (приоритетная очередь).
// Ничего не знает о задачах: тип элементов T и правило порядка Compare —
// параметры шаблона.
//
// Соглашение о Compare: compare(a, b) == true  <=>  a приоритетнее b
// (a должен оказаться ближе к вершине кучи).
//   std::less<int>    -> на вершине минимум;
//   std::greater<int> -> на вершине максимум.
//
// Физическое представление: полное бинарное дерево, уложенное в std::vector
// по уровням. Для элемента с индексом i:
//   родитель  = (i - 1) / 2,  левый сын = 2i + 1,  правый сын = 2i + 2.
// Полнота дерева выполняется автоматически: элементы лежат в массиве без "дыр".
//
// Инвариант кучи: ни один элемент не приоритетнее своего родителя,
// т.е. для всех i > 0: !compare(data[i], data[parent(i)]).
// После push (просеивание вверх) и pop (просеивание вниз) он восстанавливается.
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T, typename Compare>
class BinaryHeap {
public:
    explicit BinaryHeap(Compare compare = Compare()) : compare_(compare) {}

    // Добавление: кладём в конец и поднимаем, пока он приоритетнее родителя.
    void push(const T& value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    // Элемент с наивысшим приоритетом без удаления.
    const T& top() const {
        if (empty()) {
            throw std::out_of_range("Куча пуста");
        }
        return data_.front();
    }

    // Извлечение: вершину заменяем последним элементом и опускаем его вниз.
    T pop() {
        if (empty()) {
            throw std::out_of_range("Куча пуста");
        }
        T result = data_.front();
        data_.front() = data_.back();
        data_.pop_back();
        if (!empty()) {
            siftDown(0);
        }
        return result;
    }

    std::size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

    // Проверка инварианта (используется в тестах).
    bool isValid() const {
        for (std::size_t i = 1; i < data_.size(); ++i) {
            if (compare_(data_[i], data_[parent(i)])) {
                return false;
            }
        }
        return true;
    }

private:
    std::size_t parent(std::size_t i) const { return (i - 1) / 2; }
    std::size_t left(std::size_t i) const { return 2 * i + 1; }
    std::size_t right(std::size_t i) const { return 2 * i + 2; }

    void siftUp(std::size_t i) {
        while (i > 0 && compare_(data_[i], data_[parent(i)])) {
            std::swap(data_[i], data_[parent(i)]);
            i = parent(i);
        }
    }

    void siftDown(std::size_t i) {
        while (true) {
            std::size_t best = i;
            if (left(i) < data_.size() && compare_(data_[left(i)], data_[best])) {
                best = left(i);
            }
            if (right(i) < data_.size() && compare_(data_[right(i)], data_[best])) {
                best = right(i);
            }
            if (best == i) {
                return;  // оба сына не приоритетнее — инвариант восстановлен
            }
            std::swap(data_[i], data_[best]);
            i = best;
        }
    }

    std::vector<T> data_;  // вспомогательное хранилище (разрешено заданием)
    Compare compare_;
};
