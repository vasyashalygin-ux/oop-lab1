#include "tests.h"

#include <memory>

#include "../common/testing.h"
#include "editor.h"

namespace {

void testVector() {
    Vector2D a(1, 2), b(3, -1);
    check(a + b == Vector2D(4, 1), "Vector + Vector");
    check(a - b == Vector2D(-2, 3), "Vector - Vector");
    check(a * 2 == Vector2D(2, 4), "Vector * scalar");
    check(2 * a == Vector2D(2, 4), "scalar * Vector");
    check(Vector2D(4, 2) / 2 == Vector2D(2, 1), "Vector / scalar");
    checkThrows([&] { a / 0; }, "Vector / 0 -> исключение");
    check(a != b, "Vector !=");
    check(!(a != Vector2D(1, 2)), "Vector != для равных");
    check(almostEqual(Vector2D(3, 4).length(), 5), "длина вектора");
    check(almostEqual(a.dot(b), 1), "скалярное произведение");
    Vector2D c = a + b;
    check(a == Vector2D(1, 2), "иммутабельность: a не изменился после a + b");
    (void)c;
}

void testPoint() {
    Point2D p(1, 1), q(4, 5);
    check(p + Vector2D(2, 3) == Point2D(3, 4), "Point + Vector -> Point");
    check(q - p == Vector2D(3, 4), "Point - Point -> Vector");
    check(q - Vector2D(3, 4) == p, "Point - Vector -> Point");
    check(almostEqual(p.distanceTo(q), 5), "расстояние между точками");
    check(p != q, "Point !=");
    check(Point2D(0.1 + 0.2, 0) == Point2D(0.3, 0), "сравнение с погрешностью");
}

void testRect() {
    Rect r(Point2D(4, 3), Point2D(0, 0));  // углы в "неправильном" порядке
    check(r.min() == Point2D(0, 0) && r.max() == Point2D(4, 3), "Rect нормализует углы");
    check(almostEqual(r.width(), 4) && almostEqual(r.height(), 3), "ширина и высота");
    check(r.contains(Point2D(2, 1)), "точка внутри Rect");
    check(r.contains(Point2D(4, 3)), "точка на границе Rect");
    check(!r.contains(Point2D(5, 1)), "точка вне Rect");

    Rect other(Point2D(2, 1), Point2D(6, 6));
    auto inter = r & other;
    check(inter.has_value(), "пересечение есть");
    check(inter && *inter == Rect(Point2D(2, 1), Point2D(4, 3)), "Rect & Rect — правильная область");
    check(!(r & Rect(Point2D(10, 10), Point2D(12, 12))), "нет пересечения -> пустой optional");
    auto touch = r & Rect(Point2D(4, 0), Point2D(8, 3));
    check(touch && almostEqual(touch->width(), 0), "касание по стороне = вырожденное пересечение");

    check((r | other) == Rect(Point2D(0, 0), Point2D(6, 6)), "Rect | Rect — объединяющий прямоугольник");
    Rect moved = r + Vector2D(1, 1);
    check(moved == Rect(Point2D(1, 1), Point2D(5, 4)), "Rect + Vector");
    check(r == Rect(Point2D(0, 0), Point2D(4, 3)), "иммутабельность Rect");
    check(r != moved, "Rect !=");
}

void testCircle() {
    checkThrows([] { Circle(Point2D(0, 0), 0); }, "радиус 0 -> исключение");
    checkThrows([] { Circle(Point2D(0, 0), -1); }, "отрицательный радиус -> исключение");
    Circle c(Point2D(0, 0), 2);
    check(c.contains(Point2D(0, 0)), "центр принадлежит окружности");
    check(c.contains(Point2D(2, 0)), "точка на границе окружности");
    check(!c.contains(Point2D(2, 2)), "точка вне окружности (угол bbox)");
    check(c.boundingBox() == Rect(Point2D(-2, -2), Point2D(2, 2)), "bbox окружности");
    c.move(Vector2D(5, 1));
    check(c.center() == Point2D(5, 1), "перемещение окружности");
    check(c.toString().find("Circle") != std::string::npos, "toString окружности");
}

void testRectangle() {
    Rectangle r(Rect(Point2D(0, 0), Point2D(3, 2)));
    check(r.contains(Point2D(1, 1)), "точка внутри прямоугольника");
    check(r.contains(Point2D(3, 0)), "точка на границе прямоугольника");
    check(!r.contains(Point2D(4, 0)), "точка вне прямоугольника");
    r.move(Vector2D(1, 1));
    check(r.boundingBox() == Rect(Point2D(1, 1), Point2D(4, 3)), "перемещение прямоугольника");
    check(r.toString().find("Rectangle") != std::string::npos, "toString прямоугольника");
}

void testSegment() {
    checkThrows([] { Segment(Point2D(1, 1), Point2D(1, 1)); }, "отрезок нулевой длины -> исключение");
    checkThrows([] { Segment(Point2D(0, 0), Point2D(1, 1), -0.1); }, "отрицательная погрешность -> исключение");
    Segment s(Point2D(0, 0), Point2D(10, 0), 0.5);
    check(s.contains(Point2D(5, 0)), "точка на отрезке");
    check(s.contains(Point2D(5, 0.5)), "точка на расстоянии = погрешности");
    check(!s.contains(Point2D(5, 1)), "точка дальше погрешности");
    check(!s.contains(Point2D(11, 0)), "точка на продолжении отрезка — не принадлежит");
    check(almostEqual(s.distanceTo(Point2D(13, 4)), 5), "расстояние до конца отрезка");
    check(s.boundingBox() == Rect(Point2D(0, 0), Point2D(10, 0)), "bbox отрезка (вырожденный)");
    s.move(Vector2D(0, 2));
    check(s.contains(Point2D(5, 2)), "перемещение отрезка");
}

void testGroup() {
    auto c = std::make_shared<Circle>(Point2D(0, 0), 1);
    auto r = std::make_shared<Rectangle>(Rect(Point2D(5, 5), Point2D(7, 6)));
    checkThrows([] { Group g({}); }, "пустая группа -> исключение");

    auto g = std::make_shared<Group>(std::vector<std::shared_ptr<Shape>>{c, r});
    check(g->size() == 2, "размер группы");
    check(c->isInGroup() && r->isInGroup(), "объекты отмечены как члены группы");
    check(g->boundingBox() == Rect(Point2D(-1, -1), Point2D(7, 6)), "bbox группы по всем объектам");
    check(g->contains(Point2D(6, 5.5)), "точка принадлежит группе через вложенный объект");
    check(!g->contains(Point2D(3, 3)), "точка не принадлежит ни одному объекту группы");
    check(g->toString().find("Circle") != std::string::npos &&
              g->toString().find("Rectangle") != std::string::npos,
          "toString группы включает вложенные объекты");

    g->move(Vector2D(1, 0));
    check(c->center() == Point2D(1, 0), "перемещение группы двигает вложенные объекты");

    checkThrows([&] { g->add(c); }, "объект уже в этой группе -> исключение");
    checkThrows([&] { Group other({c}); }, "объект не может быть в двух группах");
    checkThrows([&] { g->add(g); }, "группа не может содержать саму себя");
    checkThrows([&] { g->add(nullptr); }, "nullptr в группу -> исключение");

    auto s = std::make_shared<Segment>(Point2D(20, 0), Point2D(30, 0));
    auto outer = std::make_shared<Group>(std::vector<std::shared_ptr<Shape>>{g, s});
    check(outer->boundingBox() == Rect(Point2D(0, -1), Point2D(30, 6)), "bbox вложенной группы");
    checkThrows([&] { g->add(outer); }, "косвенный цикл (g внутри outer) -> исключение");
    check(outer->includes(c.get()), "includes находит объект на глубине 2");

    // Повторная попытка с ошибкой в конструкторе не должна "застревать" объект в группе.
    auto free1 = std::make_shared<Circle>(Point2D(0, 0), 1);
    checkThrows([&] { Group bad({free1, free1}); }, "дубликат в конструкторе группы -> исключение");
    check(!free1->isInGroup(), "после ошибки конструктора объект снова свободен");

    {
        auto temp = std::make_shared<Circle>(Point2D(0, 0), 1);
        { Group local({temp}); }
        check(!temp->isInGroup(), "после уничтожения группы объект свободен");
    }

    // Наложение внутри группы: символ верхнего вложенного объекта.
    auto under = std::make_shared<Rectangle>(Rect(Point2D(0, 0), Point2D(4, 4)));
    auto over = std::make_shared<Circle>(Point2D(2, 2), 1);
    Group layered({under, over});
    check(layered.symbolAt(Point2D(2, 2)) == 'O', "в группе виден верхний объект");
    check(layered.symbolAt(Point2D(0, 0)) == 'R', "в группе виден нижний там, где нет верхнего");
}

void testEditor() {
    Editor editor;
    auto rect = std::make_shared<Rectangle>(Rect(Point2D(0, 0), Point2D(10, 10)));
    auto circle = std::make_shared<Circle>(Point2D(5, 5), 2);
    auto seg = std::make_shared<Segment>(Point2D(20, 20), Point2D(30, 20));

    check(editor.topAt(Point2D(0, 0)) == nullptr, "пустой редактор: topAt -> nullptr");
    check(editor.inArea(Rect(Point2D(0, 0), Point2D(100, 100))).empty(), "пустой редактор: inArea пуст");

    editor.add(rect);
    editor.add(circle);
    editor.add(seg);
    check(editor.size() == 3, "добавление объектов");
    checkThrows([&] { editor.add(rect); }, "повторное добавление -> исключение");
    checkThrows([&] { editor.add(nullptr); }, "nullptr в редактор -> исключение");

    check(editor.topAt(Point2D(5, 5)) == circle, "верхний объект — добавленный позже");
    check(editor.topAt(Point2D(1, 1)) == rect, "под точкой только нижний объект");
    check(editor.topAt(Point2D(50, 50)) == nullptr, "под точкой ничего нет");

    auto found = editor.inArea(Rect(Point2D(4, 4), Point2D(6, 6)));
    check(found.size() == 2 && found[0] == rect && found[1] == circle, "inArea находит пересекающиеся объекты");
    check(editor.inArea(Rect(Point2D(40, 40), Point2D(50, 50))).empty(), "inArea без результатов");

    check(editor.move(seg, Vector2D(0, 5)), "move существующего объекта");
    check(seg->contains(Point2D(25, 25)), "объект действительно сдвинулся");

    size_t moved = editor.moveInArea(Rect(Point2D(4, 4), Point2D(6, 6)), Vector2D(100, 0));
    check(moved == 2, "moveInArea вернул число перемещённых");
    check(circle->center() == Point2D(105, 5), "moveInArea сдвинул окружность");
    check(seg->contains(Point2D(25, 25)), "moveInArea не тронул объект вне области");

    // Объект из группы нельзя добавить отдельно; группу с объектом редактора — тоже.
    auto inside = std::make_shared<Circle>(Point2D(0, 0), 1);
    auto grp = std::make_shared<Group>(std::vector<std::shared_ptr<Shape>>{inside});
    checkThrows([&] { editor.add(inside); }, "объект из группы нельзя добавить в редактор отдельно");
    editor.add(grp);
    check(editor.topAt(Point2D(0, 0)) == grp, "topAt возвращает группу");
    auto free2 = std::make_shared<Circle>(Point2D(0, 0), 1);
    editor.add(free2);
    checkThrows([&] { editor.add(std::make_shared<Group>(std::vector<std::shared_ptr<Shape>>{free2})); },
                "группа с объектом, уже лежащим в редакторе -> исключение");

    check(editor.remove(circle), "удаление объекта");
    check(!editor.remove(circle), "повторное удаление -> false");
    check(!editor.move(circle, Vector2D(1, 1)), "move удалённого объекта -> false");
    check(editor.size() == 4, "размер после удаления");

    // Отрисовка учитывает наложение.
    Editor small;
    small.add(std::make_shared<Rectangle>(Rect(Point2D(0, 0), Point2D(2, 0))));
    small.add(std::make_shared<Circle>(Point2D(2, 0), 0.5));
    check(small.render(Rect(Point2D(0, 0), Point2D(3, 0))) == "RRO\n", "render учитывает порядок наложения");
}

}  // namespace

void runTask1Tests() {
    testVector();
    testPoint();
    testRect();
    testCircle();
    testRectangle();
    testSegment();
    testGroup();
    testEditor();
}
