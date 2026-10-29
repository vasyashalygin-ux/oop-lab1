#include "editor.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

void Editor::add(const std::shared_ptr<Shape>& shape) {
    if (!shape) {
        throw std::invalid_argument("Нельзя добавить пустой объект");
    }
    if (shape->isInGroup()) {
        throw std::invalid_argument("Объект принадлежит группе — добавляйте группу целиком");
    }
    for (const auto& existing : shapes_) {
        // includes покрывает и повторное добавление того же объекта (this == other),
        // и группу, в которой лежит объект, уже находящийся в редакторе.
        if (shape->includes(existing.get())) {
            throw std::invalid_argument("Объект уже есть в редакторе");
        }
    }
    shapes_.push_back(shape);
}

bool Editor::remove(const std::shared_ptr<Shape>& shape) {
    auto it = std::find(shapes_.begin(), shapes_.end(), shape);
    if (it == shapes_.end()) {
        return false;
    }
    shapes_.erase(it);
    return true;
}

bool Editor::move(const std::shared_ptr<Shape>& shape, const Vector2D& v) {
    auto it = std::find(shapes_.begin(), shapes_.end(), shape);
    if (it == shapes_.end()) {
        return false;
    }
    (*it)->move(v);
    return true;
}

std::shared_ptr<Shape> Editor::topAt(const Point2D& p) const {
    // Идём с конца: последний добавленный объект — самый верхний.
    for (auto it = shapes_.rbegin(); it != shapes_.rend(); ++it) {
        if ((*it)->contains(p)) {
            return *it;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Shape>> Editor::inArea(const Rect& area) const {
    std::vector<std::shared_ptr<Shape>> result;
    for (const auto& shape : shapes_) {
        if (shape->boundingBox() & area) {  // optional -> true, если пересечение есть
            result.push_back(shape);
        }
    }
    return result;
}

size_t Editor::moveInArea(const Rect& area, const Vector2D& v) {
    // Используем ту же логику поиска, что и inArea, — по требованию задания.
    std::vector<std::shared_ptr<Shape>> found = inArea(area);
    for (const auto& shape : found) {
        shape->move(v);
    }
    return found.size();
}

size_t Editor::size() const { return shapes_.size(); }

std::string Editor::render(const Rect& viewport) const {
    std::string result;
    int x0 = static_cast<int>(std::floor(viewport.min().x()));
    int x1 = static_cast<int>(std::ceil(viewport.max().x()));
    int y0 = static_cast<int>(std::floor(viewport.min().y()));
    int y1 = static_cast<int>(std::ceil(viewport.max().y()));
    for (int y = y0; y <= y1; ++y) {  // ось y направлена вниз, как строки в консоли
        std::string line;
        for (int x = x0; x <= x1; ++x) {
            Point2D p(x, y);
            std::shared_ptr<Shape> top = topAt(p);
            line += top ? top->symbolAt(p) : ' ';
        }
        while (!line.empty() && line.back() == ' ') {
            line.pop_back();
        }
        result += line + "\n";
    }
    return result;
}

std::string Editor::toString() const {
    std::string result;
    for (size_t i = 0; i < shapes_.size(); ++i) {
        result += std::to_string(i) + ": " + shapes_[i]->toString() + "\n";
    }
    return result;
}
