#pragma once
// Иммутабельные типы-значения геометрии: Vector2D, Point2D, Rect.
// Иммутабельность: у классов нет методов, меняющих объект; все операции
// возвращают НОВЫЙ объект, а исходный остаётся прежним.
#include <optional>
#include <string>

// Погрешность сравнения вещественных чисел.
const double EPS = 1e-9;
bool almostEqual(double a, double b);

class Vector2D {
public:
    Vector2D(double x = 0, double y = 0);

    double x() const;
    double y() const;
    double length() const;
    double dot(const Vector2D& other) const;  // скалярное произведение

    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(double k) const;
    Vector2D operator/(double k) const;  // k == 0 -> исключение

    bool operator==(const Vector2D& other) const;
    bool operator!=(const Vector2D& other) const;

    std::string toString() const;

private:
    double x_;
    double y_;
};

// k * v — чтобы умножение на число было коммутативным.
Vector2D operator*(double k, const Vector2D& v);

class Point2D {
public:
    Point2D(double x = 0, double y = 0);

    double x() const;
    double y() const;
    double distanceTo(const Point2D& other) const;

    Point2D operator+(const Vector2D& v) const;   // точка + вектор = точка
    Point2D operator-(const Vector2D& v) const;   // точка - вектор = точка
    Vector2D operator-(const Point2D& p) const;   // точка - точка = вектор

    bool operator==(const Point2D& other) const;
    bool operator!=(const Point2D& other) const;

    std::string toString() const;

private:
    double x_;
    double y_;
};

// Прямоугольник как область (значение, а не графический объект).
// Стороны параллельны осям. Хранится как min-угол и max-угол.
// Допускается вырожденная область (ширина или высота 0) — например,
// ограничивающий прямоугольник горизонтального отрезка.
class Rect {
public:
    // Любые два противоположных угла — порядок не важен, нормализуем в конструкторе.
    Rect(const Point2D& a, const Point2D& b);

    Point2D min() const;
    Point2D max() const;
    double width() const;
    double height() const;

    bool contains(const Point2D& p) const;  // включая границу

    Rect operator+(const Vector2D& v) const;  // сдвинутая копия
    // Пересечение. Пустое std::optional => общей области нет.
    std::optional<Rect> operator&(const Rect& other) const;
    // Наименьший прямоугольник, содержащий оба (нужен для группы).
    Rect operator|(const Rect& other) const;

    bool operator==(const Rect& other) const;
    bool operator!=(const Rect& other) const;

    std::string toString() const;

private:
    Point2D min_;
    Point2D max_;
};
