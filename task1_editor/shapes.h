#pragma once
#include <memory>
#include <vector>
#include "geometry.h"

class Group;

// Общий тип для всех графических объектов. Редактор и группа работают только с ним.
class Shape {
    friend class Group;       // менять parent может только группа
    Group* parent = nullptr;  // группа, в которой лежит объект. Не владеет им!

public:
    virtual ~Shape() {}

    virtual Rect boundingBox() const = 0;
    virtual void move(Vector v) = 0;
    virtual bool contains(Point p) const = 0;
    virtual string toString() const = 0;
    virtual char symbol() const = 0;  // чем рисовать объект в консоли

    bool isInGroup() const { return parent != nullptr; }
};

using ShapePtr = shared_ptr<Shape>;

class Circle : public Shape {
    Point center;
    double radius;

public:
    Circle(Point center, double radius) : center(center), radius(radius) {
        if (radius <= 0) throw invalid_argument("Радиус должен быть положительным");
    }

    Rect boundingBox() const override {
        return Rect(center + Vector(-radius, -radius), center + Vector(radius, radius));
    }
    void move(Vector v) override { center = center + v; }
    bool contains(Point p) const override { return (p - center).length() <= radius; }
    string toString() const override { return "Circle(центр " + center.toString() + ", радиус " + str(radius) + ")"; }
    char symbol() const override { return 'O'; }
};

class Rectangle : public Shape {
    Rect area;

public:
    Rectangle(Rect area) : area(area) {}

    Rect boundingBox() const override { return area; }
    void move(Vector v) override { area = area + v; }
    bool contains(Point p) const override { return area.contains(p); }
    string toString() const override { return "Rectangle" + area.toString(); }
    char symbol() const override { return 'R'; }
};

// Допустимое расстояние от точки до отрезка, чтобы считать, что точка на нём
const double TOLERANCE = 0.5;

class Segment : public Shape {
    Point a, b;

public:
    Segment(Point a, Point b) : a(a), b(b) {
        if (a == b) throw invalid_argument("Концы отрезка совпадают");
    }

    Rect boundingBox() const override {
        return Rect(Point(min(a.x(), b.x()), min(a.y(), b.y())),
                    Point(max(a.x(), b.x()), max(a.y(), b.y())));
    }
    void move(Vector v) override {
        a = a + v;
        b = b + v;
    }
    bool contains(Point p) const override {
        Vector ab = b - a;
        Vector ap = p - a;
        double t = ap.dot(ab) / ab.dot(ab);  // 0 - проекция точки в a, 1 - в b
        t = max(0.0, min(1.0, t));           // не выходим за концы отрезка
        Point nearest = a + ab * t;          // ближайшая к p точка отрезка
        return (p - nearest).length() <= TOLERANCE;
    }
    string toString() const override { return "Segment(" + a.toString() + " - " + b.toString() + ")"; }
    char symbol() const override { return '*'; }
};

// Составной объект. Сам является Shape, поэтому может лежать в другой группе.
class Group : public Shape {
    vector<ShapePtr> items;

public:
    // Группа не бывает пустой, поэтому у неё всегда есть ограничивающий прямоугольник
    explicit Group(ShapePtr first) { add(first); }

    ~Group() {
        for (ShapePtr s : items) s->parent = nullptr;  // объекты снова свободны
    }

    void add(ShapePtr s) {
        if (s == nullptr) throw invalid_argument("Пустой объект");
        if (s->isInGroup()) throw invalid_argument("Объект уже лежит в группе");
        // Идём от этой группы вверх по родителям. Если встретим s, получится цикл
        for (Group* g = this; g != nullptr; g = g->parent) {
            if (g == s.get()) throw invalid_argument("Группа не может содержать саму себя");
        }
        s->parent = this;
        items.push_back(s);
    }

    Rect boundingBox() const override {
        Rect box = items[0]->boundingBox();
        for (ShapePtr s : items) box = box | s->boundingBox();
        return box;
    }
    void move(Vector v) override {
        for (ShapePtr s : items) s->move(v);
    }
    bool contains(Point p) const override {
        for (ShapePtr s : items) {
            if (s->contains(p)) return true;
        }
        return false;
    }
    string toString() const override {
        string result = "Group{";
        for (size_t i = 0; i < items.size(); i++) {
            if (i > 0) result += "; ";
            result += items[i]->toString();
        }
        return result + "}";
    }
    char symbol() const override { return 'G'; }
};
