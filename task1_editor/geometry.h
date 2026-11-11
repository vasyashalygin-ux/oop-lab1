#pragma once
#include <algorithm>
#include <cmath>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
using namespace std;

// Vector, Point и Rect иммутабельны: поля приватные, сеттеров нет,
// каждая операция возвращает новый объект.

// Число в строку без лишних нулей: 2.5 -> "2.5", 3 -> "3"
string str(double value) {
    ostringstream out;
    out << value;
    return out.str();
}

class Vector {
    double x_, y_;

public:
    Vector(double x, double y) : x_(x), y_(y) {}

    double x() const { return x_; }
    double y() const { return y_; }
    double length() const { return sqrt(x_ * x_ + y_ * y_); }
    double dot(Vector other) const { return x_ * other.x_ + y_ * other.y_; }

    Vector operator+(Vector other) const { return Vector(x_ + other.x_, y_ + other.y_); }
    Vector operator-(Vector other) const { return Vector(x_ - other.x_, y_ - other.y_); }
    Vector operator*(double k) const { return Vector(x_ * k, y_ * k); }
    Vector operator/(double k) const {
        if (k == 0) throw invalid_argument("Деление вектора на ноль");
        return Vector(x_ / k, y_ / k);
    }

    bool operator==(Vector other) const { return x_ == other.x_ && y_ == other.y_; }
    bool operator!=(Vector other) const { return !(*this == other); }
};

class Point {
    double x_, y_;

public:
    Point(double x, double y) : x_(x), y_(y) {}

    double x() const { return x_; }
    double y() const { return y_; }

    Point operator+(Vector v) const { return Point(x_ + v.x(), y_ + v.y()); }
    Vector operator-(Point other) const { return Vector(x_ - other.x_, y_ - other.y_); }

    bool operator==(Point other) const { return x_ == other.x_ && y_ == other.y_; }
    bool operator!=(Point other) const { return !(*this == other); }

    string toString() const { return "(" + str(x_) + ", " + str(y_) + ")"; }
};

// Прямоугольная область со сторонами вдоль осей. Ось y направлена вниз, как в консоли.
class Rect {
    Point topLeft_, bottomRight_;

public:
    Rect(Point topLeft, Point bottomRight) : topLeft_(topLeft), bottomRight_(bottomRight) {
        if (topLeft.x() > bottomRight.x() || topLeft.y() > bottomRight.y()) {
            throw invalid_argument("Левый верхний угол должен быть левее и выше правого нижнего");
        }
    }

    Point topLeft() const { return topLeft_; }
    Point bottomRight() const { return bottomRight_; }

    // Граница считается частью прямоугольника
    bool contains(Point p) const {
        return p.x() >= topLeft_.x() && p.x() <= bottomRight_.x() &&
               p.y() >= topLeft_.y() && p.y() <= bottomRight_.y();
    }

    Rect operator+(Vector v) const { return Rect(topLeft_ + v, bottomRight_ + v); }

    // Пересечение. Если общей области нет, результат пустой (has_value() == false)
    optional<Rect> operator&(Rect other) const {
        double left = max(topLeft_.x(), other.topLeft_.x());
        double top = max(topLeft_.y(), other.topLeft_.y());
        double right = min(bottomRight_.x(), other.bottomRight_.x());
        double bottom = min(bottomRight_.y(), other.bottomRight_.y());
        if (left > right || top > bottom) return nullopt;
        return Rect(Point(left, top), Point(right, bottom));
    }

    // Наименьший прямоугольник, в который помещаются оба
    Rect operator|(Rect other) const {
        return Rect(Point(min(topLeft_.x(), other.topLeft_.x()), min(topLeft_.y(), other.topLeft_.y())),
                    Point(max(bottomRight_.x(), other.bottomRight_.x()),
                          max(bottomRight_.y(), other.bottomRight_.y())));
    }

    bool operator==(Rect other) const {
        return topLeft_ == other.topLeft_ && bottomRight_ == other.bottomRight_;
    }
    bool operator!=(Rect other) const { return !(*this == other); }

    string toString() const { return "[" + topLeft_.toString() + " - " + bottomRight_.toString() + "]"; }
};
