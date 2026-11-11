#pragma once
#include <stdexcept>
#include <vector>
using namespace std;

// Двоичная куча. Ничего не знает про задачи: тип элементов T и правило порядка
// Before задаются параметрами шаблона.
//
// Правило: before(a, b) == true означает "a важнее b", то есть a должен быть ближе к вершине.
//   less<int>    -> на вершине наименьший
//   greater<int> -> на вершине наибольший
//
// Куча хранится в vector как полное бинарное дерево, уложенное по уровням.
// Для элемента с индексом i: родитель (i - 1) / 2, дети 2i + 1 и 2i + 2.
// Элементы лежат в vector подряд, без пропусков, поэтому дерево всегда полное.
//
// Инвариант: ни один элемент не важнее своего родителя.
// push и pop сначала меняют vector, а потом возвращают инвариант просеиванием.
template <typename T, typename Before>
class Heap {
    vector<T> data;
    Before before;

    // Поднимаем элемент, пока он важнее родителя
    void siftUp(int i) {
        while (i > 0 && before(data[i], data[(i - 1) / 2])) {
            swap(data[i], data[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }

    // Опускаем элемент, пока какой-то из детей важнее него
    void siftDown(int i) {
        while (true) {
            int best = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            if (left < size() && before(data[left], data[best])) best = left;
            if (right < size() && before(data[right], data[best])) best = right;
            if (best == i) return;
            swap(data[i], data[best]);
            i = best;
        }
    }

public:
    int size() const { return data.size(); }
    bool empty() const { return data.empty(); }

    void push(T value) {
        data.push_back(value);  // кладём в конец и поднимаем
        siftUp(size() - 1);
    }

    // Самый важный элемент, не удаляя его
    T top() const {
        if (empty()) throw out_of_range("Куча пуста");
        return data[0];
    }

    // Достаём самый важный элемент: на его место ставим последний и опускаем
    T pop() {
        T result = top();
        data[0] = data.back();
        data.pop_back();
        if (!empty()) siftDown(0);
        return result;
    }
};
