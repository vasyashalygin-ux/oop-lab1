#pragma once
// Иерархия графических объектов: Shape (абстрактный) -> Circle, Rectangle, Segment, Group.
// Наследование выражает отношение "является": окружность ЯВЛЯЕТСЯ графическим объектом.
#include <memory>
#include <string>
#include <vector>

#include "geometry.h"

class Group;

// Общий контракт графического объекта.
class Shape {
public:
    virtual ~Shape() = default;

    virtual Rect boundingBox() const = 0;                 // ограничивающий прямоугольник
    virtual void move(const Vector2D& v) = 0;             // перемещение на вектор
    virtual bool contains(const Point2D& p) const = 0;    // принадлежит ли точка объекту
    virtual std::string toString() const = 0;             // текстовое представление

    // Символ для отрисовки точки p в консоли.
    // У простых фигур — свой символ, у группы — символ верхнего вложенного объекта.
    virtual char symbolAt(const Point2D& p) const = 0;

    // true, если other — это сам объект или объект, вложенный в него.
    // Группа переопределяет и проверяет детей (нужно для запрета циклов).
    virtual bool includes(const Shape* other) const;

    bool isInGroup() const;

private:
    friend class Group;            // только группа может отметить объект как "свой"
    const Group* group_ = nullptr; // группа, которой принадлежит объект (не владеет!)
};

class Circle : public Shape {
public:
    Circle(const Point2D& center, double radius);  // radius <= 0 -> исключение

    Point2D center() const;
    double radius() const;

    Rect boundingBox() const override;
    void move(const Vector2D& v) override;
    bool contains(const Point2D& p) const override;
    std::string toString() const override;
    char symbolAt(const Point2D& p) const override;

private:
    Point2D center_;
    double radius_;
};

// Графический объект "прямоугольник". Внутри хранит значение Rect (композиция).
class Rectangle : public Shape {
public:
    explicit Rectangle(const Rect& area);

    Rect boundingBox() const override;
    void move(const Vector2D& v) override;
    bool contains(const Point2D& p) const override;
    std::string toString() const override;
    char symbolAt(const Point2D& p) const override;

private:
    Rect area_;
};

class Segment : public Shape {
public:
    // a == b или tolerance < 0 -> исключение
    Segment(const Point2D& a, const Point2D& b, double tolerance = 0.5);

    double distanceTo(const Point2D& p) const;  // расстояние от точки до отрезка

    Rect boundingBox() const override;
    void move(const Vector2D& v) override;
    bool contains(const Point2D& p) const override;
    std::string toString() const override;
    char symbolAt(const Point2D& p) const override;

private:
    Point2D a_;
    Point2D b_;
    double tolerance_;
};

// Составной объект (паттерн "Компоновщик"). Сам является Shape,
// поэтому может лежать в редакторе и в другой группе.
class Group : public Shape {
public:
    // Группа не бывает пустой — иначе у неё нет ограничивающего прямоугольника.
    explicit Group(const std::vector<std::shared_ptr<Shape>>& items);
    ~Group() override;
    // Копирование запрещено: копия содержала бы те же объекты, и один объект
    // оказался бы сразу в двух группах.
    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    // Ошибка (исключение), если: nullptr; объект уже в какой-то группе;
    // объект — это сама группа или содержит её (цикл).
    void add(const std::shared_ptr<Shape>& item);
    size_t size() const;

    Rect boundingBox() const override;
    void move(const Vector2D& v) override;
    bool contains(const Point2D& p) const override;
    std::string toString() const override;
    char symbolAt(const Point2D& p) const override;
    bool includes(const Shape* other) const override;

private:
    std::vector<std::shared_ptr<Shape>> items_;  // порядок = порядок наложения внутри группы
};
