#include "shapes.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

// ---------------- Shape ----------------

bool Shape::includes(const Shape* other) const {
    return this == other;
}

bool Shape::isInGroup() const {
    return group_ != nullptr;
}

// ---------------- Circle ----------------

Circle::Circle(const Point2D& center, double radius) : center_(center), radius_(radius) {
    if (radius <= 0) {
        throw std::invalid_argument("Радиус окружности должен быть положительным");
    }
}

Point2D Circle::center() const { return center_; }
double Circle::radius() const { return radius_; }

Rect Circle::boundingBox() const {
    Vector2D r(radius_, radius_);
    return Rect(center_ - r, center_ + r);
}

void Circle::move(const Vector2D& v) {
    center_ = center_ + v;  // старое значение заменяется новым, сам Point2D не меняется
}

bool Circle::contains(const Point2D& p) const {
    return center_.distanceTo(p) <= radius_ + EPS;  // внутри или на границе
}

std::string Circle::toString() const {
    std::ostringstream out;
    out << "Circle(center=" << center_.toString() << ", r=" << radius_ << ")";
    return out.str();
}

char Circle::symbolAt(const Point2D&) const { return 'O'; }

// ---------------- Rectangle ----------------

Rectangle::Rectangle(const Rect& area) : area_(area) {}

Rect Rectangle::boundingBox() const { return area_; }

void Rectangle::move(const Vector2D& v) { area_ = area_ + v; }

bool Rectangle::contains(const Point2D& p) const { return area_.contains(p); }

std::string Rectangle::toString() const { return "Rectangle" + area_.toString(); }

char Rectangle::symbolAt(const Point2D&) const { return 'R'; }

// ---------------- Segment ----------------

Segment::Segment(const Point2D& a, const Point2D& b, double tolerance)
    : a_(a), b_(b), tolerance_(tolerance) {
    if (a == b) {
        throw std::invalid_argument("Отрезок нулевой длины");
    }
    if (tolerance < 0) {
        throw std::invalid_argument("Погрешность не может быть отрицательной");
    }
}

double Segment::distanceTo(const Point2D& p) const {
    // Проецируем точку на прямую ab и "зажимаем" проекцию внутри отрезка.
    Vector2D ab = b_ - a_;
    Vector2D ap = p - a_;
    double t = ap.dot(ab) / ab.dot(ab);
    t = std::max(0.0, std::min(1.0, t));
    Point2D closest = a_ + ab * t;
    return p.distanceTo(closest);
}

Rect Segment::boundingBox() const { return Rect(a_, b_); }

void Segment::move(const Vector2D& v) {
    a_ = a_ + v;
    b_ = b_ + v;
}

bool Segment::contains(const Point2D& p) const {
    return distanceTo(p) <= tolerance_ + EPS;
}

std::string Segment::toString() const {
    return "Segment(" + a_.toString() + " -> " + b_.toString() + ")";
}

char Segment::symbolAt(const Point2D&) const { return '*'; }

// ---------------- Group ----------------

Group::Group(const std::vector<std::shared_ptr<Shape>>& items) {
    if (items.empty()) {
        throw std::invalid_argument("Группа не может быть пустой");
    }
    try {
        for (const auto& item : items) {
            add(item);
        }
    } catch (...) {
        // Деструктор не вызовется, поэтому сами "отпускаем" уже добавленные объекты.
        for (const auto& item : items_) {
            item->group_ = nullptr;
        }
        throw;
    }
}

Group::~Group() {
    // Группа уничтожена — объекты снова свободны и могут попасть в другую группу.
    for (const auto& item : items_) {
        item->group_ = nullptr;
    }
}

void Group::add(const std::shared_ptr<Shape>& item) {
    if (!item) {
        throw std::invalid_argument("Нельзя добавить пустой объект");
    }
    if (item->isInGroup()) {
        throw std::invalid_argument("Объект уже принадлежит группе");
    }
    if (item->includes(this)) {
        throw std::invalid_argument("Группа не может содержать саму себя");
    }
    item->group_ = this;
    items_.push_back(item);
}

size_t Group::size() const { return items_.size(); }

Rect Group::boundingBox() const {
    Rect result = items_.front()->boundingBox();
    for (const auto& item : items_) {
        result = result | item->boundingBox();
    }
    return result;
}

void Group::move(const Vector2D& v) {
    for (const auto& item : items_) {
        item->move(v);
    }
}

bool Group::contains(const Point2D& p) const {
    for (const auto& item : items_) {
        if (item->contains(p)) {
            return true;
        }
    }
    return false;
}

std::string Group::toString() const {
    std::string result = "Group{";
    for (size_t i = 0; i < items_.size(); ++i) {
        if (i > 0) result += "; ";
        result += items_[i]->toString();
    }
    return result + "}";
}

char Group::symbolAt(const Point2D& p) const {
    // Внутри группы тоже есть наложение: идём с конца (сверху вниз).
    for (auto it = items_.rbegin(); it != items_.rend(); ++it) {
        if ((*it)->contains(p)) {
            return (*it)->symbolAt(p);
        }
    }
    return ' ';
}

bool Group::includes(const Shape* other) const {
    if (this == other) {
        return true;
    }
    for (const auto& item : items_) {
        if (item->includes(other)) {
            return true;
        }
    }
    return false;
}
