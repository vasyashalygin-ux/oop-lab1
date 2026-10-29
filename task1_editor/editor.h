#pragma once
// Редактор: хранит объекты разных типов в ОДНОЙ коллекции через общий тип Shape.
// Порядок в коллекции = порядок наложения (последний добавленный — сверху).
#include <memory>
#include <string>
#include <vector>

#include "shapes.h"

class Editor {
public:
    // Ошибка, если: nullptr; объект уже в редакторе; объект лежит в группе;
    // объект (группа) содержит что-то, что уже есть в редакторе.
    void add(const std::shared_ptr<Shape>& shape);

    bool remove(const std::shared_ptr<Shape>& shape);                    // false, если объекта нет
    bool move(const std::shared_ptr<Shape>& shape, const Vector2D& v);   // false, если объекта нет

    // Верхний объект, содержащий точку, или nullptr.
    std::shared_ptr<Shape> topAt(const Point2D& p) const;

    // Объекты, чей ограничивающий прямоугольник пересекается с областью.
    std::vector<std::shared_ptr<Shape>> inArea(const Rect& area) const;

    // Перемещает все объекты из inArea(area). Возвращает их количество.
    size_t moveInArea(const Rect& area, const Vector2D& v);

    size_t size() const;

    // Рисунок в консоли: каждая клетка (x, y) с целыми координатами
    // закрашивается символом верхнего объекта (используется topAt).
    std::string render(const Rect& viewport) const;

    std::string toString() const;  // список объектов снизу вверх

private:
    std::vector<std::shared_ptr<Shape>> shapes_;
};
