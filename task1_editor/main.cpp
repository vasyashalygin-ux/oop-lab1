// Лабораторная работа 1, задание 1, вариант 4: двумерный графический редактор.
#include <iostream>
#include <memory>

#include "../common/testing.h"
#include "editor.h"
#include "tests.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace {

void printList(const std::vector<std::shared_ptr<Shape>>& shapes) {
    for (const auto& s : shapes) {
        std::cout << "   - " << s->toString() << "\n";
    }
}

void demo() {
    Editor editor;
    Rect viewport(Point2D(0, 0), Point2D(32, 12));

    auto bigRect = std::make_shared<Rectangle>(Rect(Point2D(2, 5), Point2D(12, 9)));
    auto bigCircle = std::make_shared<Circle>(Point2D(13, 4), 4);
    auto smallRect = std::make_shared<Rectangle>(Rect(Point2D(19, 1), Point2D(25, 3)));
    auto smallCircle = std::make_shared<Circle>(Point2D(24, 4), 2);
    auto line = std::make_shared<Segment>(Point2D(2, 11), Point2D(30, 11));

    // Разные типы — одна коллекция, работа через общий тип Shape.
    editor.add(bigRect);
    editor.add(bigCircle);   // выше bigRect
    editor.add(smallRect);
    editor.add(smallCircle); // выше smallRect
    editor.add(line);

    std::cout << "Объекты (снизу вверх):\n" << editor.toString();
    std::cout << "\nРисунок (O - круг, R - прямоугольник, * - отрезок):\n" << editor.render(viewport);

    Point2D p(11, 6);
    std::cout << "\nВерхний объект в точке " << p.toString() << ": " << editor.topAt(p)->toString()
              << "\n(точка принадлежит и прямоугольнику, но круг добавлен позже)\n";

    Point2D empty(30, 1);
    std::cout << "Объект в точке " << empty.toString() << ": "
              << (editor.topAt(empty) ? editor.topAt(empty)->toString() : "нет") << "\n";

    Rect area(Point2D(18, 0), Point2D(26, 4));
    std::cout << "\nОбъекты, пересекающие область " << area.toString() << ":\n";
    printList(editor.inArea(area));

    std::cout << "\nПеремещаем все объекты этой области на <0, 5>. Перемещено: "
              << editor.moveInArea(area, Vector2D(0, 5)) << "\n" << editor.render(viewport);

    // Группа из двух объектов, которые уже не нужны в редакторе по отдельности.
    editor.remove(smallRect);
    editor.remove(smallCircle);
    auto group = std::make_shared<Group>(std::vector<std::shared_ptr<Shape>>{smallRect, smallCircle});
    editor.add(group);
    std::cout << "\nСоздана группа: " << group->toString() << "\n";
    std::cout << "Ограничивающий прямоугольник группы: " << group->boundingBox().toString() << "\n";

    editor.move(group, Vector2D(-4, -4));
    std::cout << "Группа перемещена на <-4, -4>:\n" << editor.render(viewport);

    // Ошибочные действия обрабатываются явно.
    try {
        group->add(group);
    } catch (const std::exception& e) {
        std::cout << "\nОшибка (ожидаемо): " << e.what() << "\n";
    }
    try {
        editor.add(smallCircle);
    } catch (const std::exception& e) {
        std::cout << "Ошибка (ожидаемо): " << e.what() << "\n";
    }
}

}  // namespace

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // чтобы русский текст корректно отображался в консоли Windows
#endif
    std::cout << "=== Автотесты ===\n";
    runTask1Tests();
    bool ok = printTestSummary();

    std::cout << "\n=== Демонстрация ===\n";
    demo();
    return ok ? 0 : 1;
}
