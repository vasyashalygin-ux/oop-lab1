#pragma once
#include "shapes.h"

// Хранит объекты разных типов в одном списке через общий тип Shape.
// Порядок в списке = порядок наложения: последний добавленный лежит сверху.
class Editor {
    vector<ShapePtr> shapes;

    bool has(ShapePtr s) const { return find(shapes.begin(), shapes.end(), s) != shapes.end(); }

public:
    int count() const { return shapes.size(); }

    void add(ShapePtr s) {
        if (s == nullptr) throw invalid_argument("Пустой объект");
        if (has(s)) throw invalid_argument("Объект уже есть в редакторе");
        if (s->isInGroup()) throw invalid_argument("Объект лежит в группе, добавляйте группу целиком");
        shapes.push_back(s);
    }

    void remove(ShapePtr s) {
        if (!has(s)) throw invalid_argument("Объекта нет в редакторе");
        shapes.erase(find(shapes.begin(), shapes.end(), s));
    }

    void move(ShapePtr s, Vector v) {
        if (!has(s)) throw invalid_argument("Объекта нет в редакторе");
        s->move(v);
    }

    // Верхний объект, содержащий точку (или nullptr)
    ShapePtr topAt(Point p) const {
        for (int i = shapes.size() - 1; i >= 0; i--) {
            if (shapes[i]->contains(p)) return shapes[i];
        }
        return nullptr;
    }

    // Объекты, чей ограничивающий прямоугольник пересекается с областью
    vector<ShapePtr> inArea(Rect area) const {
        vector<ShapePtr> result;
        for (ShapePtr s : shapes) {
            if ((s->boundingBox() & area).has_value()) result.push_back(s);
        }
        return result;
    }

    // Двигает всё, что нашёл inArea. Возвращает, сколько объектов подвинуто
    int moveInArea(Rect area, Vector v) {
        vector<ShapePtr> found = inArea(area);
        for (ShapePtr s : found) s->move(v);
        return found.size();
    }

    // Рисунок: каждая клетка (x, y) закрашена символом верхнего объекта
    string render(int width, int height) const {
        string result;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                ShapePtr s = topAt(Point(x, y));
                result += (s != nullptr) ? s->symbol() : ' ';
            }
            result += "\n";
        }
        return result;
    }
};
