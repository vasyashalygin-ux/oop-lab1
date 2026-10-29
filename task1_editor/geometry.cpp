#include "geometry.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

bool almostEqual(double a, double b) {
    return std::fabs(a - b) < EPS;
}

namespace {
// Вспомогательная функция форматирования: 3.0 -> "3", 2.5 -> "2.5".
std::string num(double v) {
    std::ostringstream out;
    out << v;
    return out.str();
}
}  // namespace

// ---------------- Vector2D ----------------

Vector2D::Vector2D(double x, double y) : x_(x), y_(y) {}

double Vector2D::x() const { return x_; }
double Vector2D::y() const { return y_; }

double Vector2D::length() const { return std::sqrt(x_ * x_ + y_ * y_); }

double Vector2D::dot(const Vector2D& other) const {
    return x_ * other.x_ + y_ * other.y_;
}

Vector2D Vector2D::operator+(const Vector2D& other) const {
    return Vector2D(x_ + other.x_, y_ + other.y_);
}

Vector2D Vector2D::operator-(const Vector2D& other) const {
    return Vector2D(x_ - other.x_, y_ - other.y_);
}

Vector2D Vector2D::operator*(double k) const {
    return Vector2D(x_ * k, y_ * k);
}

Vector2D Vector2D::operator/(double k) const {
    if (almostEqual(k, 0)) {
        throw std::invalid_argument("Деление вектора на ноль");
    }
    return Vector2D(x_ / k, y_ / k);
}

bool Vector2D::operator==(const Vector2D& other) const {
    return almostEqual(x_, other.x_) && almostEqual(y_, other.y_);
}

bool Vector2D::operator!=(const Vector2D& other) const {
    return !(*this == other);
}

std::string Vector2D::toString() const {
    return "<" + num(x_) + ", " + num(y_) + ">";
}

Vector2D operator*(double k, const Vector2D& v) {
    return v * k;
}

// ---------------- Point2D ----------------

Point2D::Point2D(double x, double y) : x_(x), y_(y) {}

double Point2D::x() const { return x_; }
double Point2D::y() const { return y_; }

double Point2D::distanceTo(const Point2D& other) const {
    return (*this - other).length();
}

Point2D Point2D::operator+(const Vector2D& v) const {
    return Point2D(x_ + v.x(), y_ + v.y());
}

Point2D Point2D::operator-(const Vector2D& v) const {
    return Point2D(x_ - v.x(), y_ - v.y());
}

Vector2D Point2D::operator-(const Point2D& p) const {
    return Vector2D(x_ - p.x_, y_ - p.y_);
}

bool Point2D::operator==(const Point2D& other) const {
    return almostEqual(x_, other.x_) && almostEqual(y_, other.y_);
}

bool Point2D::operator!=(const Point2D& other) const {
    return !(*this == other);
}

std::string Point2D::toString() const {
    return "(" + num(x_) + ", " + num(y_) + ")";
}

// ---------------- Rect ----------------

Rect::Rect(const Point2D& a, const Point2D& b)
    : min_(std::min(a.x(), b.x()), std::min(a.y(), b.y())),
      max_(std::max(a.x(), b.x()), std::max(a.y(), b.y())) {}

Point2D Rect::min() const { return min_; }
Point2D Rect::max() const { return max_; }
double Rect::width() const { return max_.x() - min_.x(); }
double Rect::height() const { return max_.y() - min_.y(); }

bool Rect::contains(const Point2D& p) const {
    return p.x() >= min_.x() - EPS && p.x() <= max_.x() + EPS &&
           p.y() >= min_.y() - EPS && p.y() <= max_.y() + EPS;
}

Rect Rect::operator+(const Vector2D& v) const {
    return Rect(min_ + v, max_ + v);
}

std::optional<Rect> Rect::operator&(const Rect& other) const {
    double left = std::max(min_.x(), other.min_.x());
    double right = std::min(max_.x(), other.max_.x());
    double bottom = std::max(min_.y(), other.min_.y());
    double top = std::min(max_.y(), other.max_.y());
    if (left > right + EPS || bottom > top + EPS) {
        return std::nullopt;  // общей области нет
    }
    return Rect(Point2D(left, bottom), Point2D(right, top));
}

Rect Rect::operator|(const Rect& other) const {
    return Rect(Point2D(std::min(min_.x(), other.min_.x()), std::min(min_.y(), other.min_.y())),
                Point2D(std::max(max_.x(), other.max_.x()), std::max(max_.y(), other.max_.y())));
}

bool Rect::operator==(const Rect& other) const {
    return min_ == other.min_ && max_ == other.max_;
}

bool Rect::operator!=(const Rect& other) const {
    return !(*this == other);
}

std::string Rect::toString() const {
    return "[" + min_.toString() + " - " + max_.toString() + "]";
}
