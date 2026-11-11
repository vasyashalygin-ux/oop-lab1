#include <cstdlib>
#include "../common/testing.h"
#include "editor.h"

bool has(string text, string part) { return text.find(part) != string::npos; }

// ---------- Тесты ----------

void testVector() {
    Vector a(1, 2), b(3, -1);
    check(a + b == Vector(4, 1), "Vector + Vector");
    check(a - b == Vector(-2, 3), "Vector - Vector");
    check(a * 2 == Vector(2, 4), "Vector * число");
    check(Vector(4, 2) / 2 == Vector(2, 1), "Vector / число");
    checkThrows([&] { a / 0; }, "деление Vector на 0");
    check(a != b, "Vector !=");
    check(!(a != Vector(1, 2)), "равные Vector не различаются через !=");
    check(Vector(3, 4).length() == 5, "длина вектора");
    check(a.dot(b) == 1, "скалярное произведение");
    check(a == Vector(1, 2), "операции не меняют исходный вектор");
}

void testPoint() {
    Point p(1, 1), q(4, 5);
    check(p + Vector(2, 3) == Point(3, 4), "Point + Vector");
    check(q - p == Vector(3, 4), "Point - Point");
    check(p == Point(1, 1), "Point ==");
    check(p != q, "Point !=");
}

void testRect() {
    checkThrows([] { Rect(Point(4, 4), Point(0, 0)); }, "углы в неправильном порядке");
    Rect r(Point(0, 0), Point(4, 3));
    check(r.contains(Point(2, 1)), "точка внутри Rect");
    check(r.contains(Point(4, 3)), "точка на границе Rect");
    check(!r.contains(Point(5, 1)), "точка снаружи Rect");

    optional<Rect> common = r & Rect(Point(2, 1), Point(6, 6));
    check(common.has_value() && *common == Rect(Point(2, 1), Point(4, 3)), "Rect & Rect: общая область");
    check(!(r & Rect(Point(10, 10), Point(12, 12))).has_value(), "Rect & Rect: нет общей области");
    check((r & Rect(Point(4, 0), Point(8, 3))).has_value(), "Rect & Rect: общая сторона считается пересечением");
    check((r | Rect(Point(2, 1), Point(6, 6))) == Rect(Point(0, 0), Point(6, 6)), "Rect | Rect");
    check(r + Vector(1, 1) == Rect(Point(1, 1), Point(5, 4)), "Rect + Vector");
    check(r == Rect(Point(0, 0), Point(4, 3)), "Rect ==");
    check(r != Rect(Point(1, 1), Point(5, 4)), "Rect !=");
}

void testCircle() {
    checkThrows([] { Circle(Point(0, 0), 0); }, "радиус 0");
    checkThrows([] { Circle(Point(0, 0), -1); }, "отрицательный радиус");
    Circle c(Point(0, 0), 2);
    check(c.contains(Point(1, 1)), "точка внутри круга");
    check(c.contains(Point(2, 0)), "точка на границе круга");
    check(!c.contains(Point(2, 2)), "точка вне круга");
    check(c.boundingBox() == Rect(Point(-2, -2), Point(2, 2)), "ограничивающий прямоугольник круга");
    c.move(Vector(5, 1));
    check(c.boundingBox() == Rect(Point(3, -1), Point(7, 3)), "перемещение круга");
    check(has(c.toString(), "Circle"), "текст круга");
}

void testRectangle() {
    Rectangle r(Rect(Point(0, 0), Point(3, 2)));
    check(r.contains(Point(1, 1)), "точка внутри прямоугольника");
    check(r.contains(Point(3, 0)), "точка на границе прямоугольника");
    check(!r.contains(Point(4, 0)), "точка вне прямоугольника");
    check(r.boundingBox() == Rect(Point(0, 0), Point(3, 2)), "ограничивающий прямоугольник прямоугольника");
    r.move(Vector(1, 1));
    check(r.boundingBox() == Rect(Point(1, 1), Point(4, 3)), "перемещение прямоугольника");
    check(has(r.toString(), "Rectangle"), "текст прямоугольника");
}

void testSegment() {
    checkThrows([] { Segment(Point(1, 1), Point(1, 1)); }, "отрезок нулевой длины");
    Segment s(Point(0, 0), Point(10, 0));
    check(s.contains(Point(5, 0)), "точка на отрезке");
    check(s.contains(Point(5, 0.5)), "точка на расстоянии ровно в погрешность");
    check(!s.contains(Point(5, 1)), "точка дальше погрешности");
    check(!s.contains(Point(11, 0)), "точка на продолжении отрезка");
    check(s.boundingBox() == Rect(Point(0, 0), Point(10, 0)), "ограничивающий прямоугольник отрезка");
    s.move(Vector(0, 2));
    check(s.contains(Point(5, 2)), "перемещение отрезка");
    check(has(s.toString(), "Segment"), "текст отрезка");
}

void testGroup() {
    checkThrows([] { Group g(nullptr); }, "группа без объектов");

    ShapePtr circle = make_shared<Circle>(Point(0, 0), 1);
    ShapePtr rect = make_shared<Rectangle>(Rect(Point(5, 5), Point(7, 6)));
    auto group = make_shared<Group>(circle);
    group->add(rect);

    check(circle->isInGroup() && rect->isInGroup(), "объекты отмечены как лежащие в группе");
    check(group->boundingBox() == Rect(Point(-1, -1), Point(7, 6)), "ограничивающий прямоугольник группы");
    check(group->contains(Point(6, 5.5)), "точка принадлежит одному из объектов группы");
    check(!group->contains(Point(3, 3)), "точка не принадлежит ни одному объекту группы");
    check(has(group->toString(), "Circle") && has(group->toString(), "Rectangle"), "текст группы содержит объекты");

    group->move(Vector(1, 0));
    check(circle->boundingBox() == Rect(Point(0, -1), Point(2, 1)), "перемещение группы двигает объекты");

    checkThrows([&] { group->add(circle); }, "объект уже в этой группе");
    checkThrows([&] { make_shared<Group>(circle); }, "объект нельзя положить в две группы");
    checkThrows([&] { group->add(nullptr); }, "пустой объект в группу");
    checkThrows([&] { group->add(group); }, "группа внутри самой себя");

    auto other = make_shared<Group>(make_shared<Segment>(Point(20, 0), Point(30, 0)));
    group->add(other);
    check(group->boundingBox().bottomRight() == Point(30, 6), "вложенная группа учитывается в границах");
    checkThrows([&] { other->add(group); }, "группа внутри вложенной группы (непрямой цикл)");

    ShapePtr lone = make_shared<Circle>(Point(0, 0), 1);
    {
        Group temp(lone);
        check(lone->isInGroup(), "объект в группе");
    }
    check(!lone->isInGroup(), "после удаления группы объект свободен");
}

void testEditor() {
    Editor editor;
    ShapePtr rect = make_shared<Rectangle>(Rect(Point(0, 0), Point(10, 10)));
    ShapePtr circle = make_shared<Circle>(Point(5, 5), 2);
    ShapePtr segment = make_shared<Segment>(Point(20, 20), Point(30, 20));

    check(editor.count() == 0, "новый редактор пуст");
    check(editor.topAt(Point(1, 1)) == nullptr, "пустой редактор: topAt");
    check(editor.inArea(Rect(Point(-100, -100), Point(100, 100))).empty(), "пустой редактор: inArea");

    editor.add(rect);
    editor.add(circle);
    editor.add(segment);
    check(editor.count() == 3, "добавление объектов");
    checkThrows([&] { editor.add(rect); }, "повторное добавление");
    checkThrows([&] { editor.add(nullptr); }, "добавление пустого объекта");

    check(editor.topAt(Point(5, 5)) == circle, "сверху объект, добавленный позже");
    check(editor.topAt(Point(1, 1)) == rect, "под точкой только нижний объект");
    check(editor.topAt(Point(50, 50)) == nullptr, "под точкой ничего нет");

    vector<ShapePtr> found = editor.inArea(Rect(Point(4, 4), Point(6, 6)));
    check(found.size() == 2 && found[0] == rect && found[1] == circle, "inArea находит пересекающиеся объекты");
    check(editor.inArea(Rect(Point(40, 40), Point(50, 50))).empty(), "inArea: в области никого нет");

    editor.move(segment, Vector(0, 5));
    check(segment->contains(Point(25, 25)), "перемещение объекта");
    ShapePtr stranger = make_shared<Circle>(Point(0, 0), 1);
    checkThrows([&] { editor.move(stranger, Vector(1, 1)); }, "перемещение объекта, которого нет в редакторе");

    int moved = editor.moveInArea(Rect(Point(4, 4), Point(6, 6)), Vector(100, 0));
    check(moved == 2, "moveInArea возвращает число подвинутых объектов");
    check(circle->contains(Point(105, 5)), "moveInArea подвинул объект из области");
    check(segment->contains(Point(25, 25)), "moveInArea не тронул объект вне области");

    ShapePtr inner = make_shared<Circle>(Point(0, 50), 1);
    auto group = make_shared<Group>(inner);
    checkThrows([&] { editor.add(inner); }, "объект из группы нельзя добавить отдельно");
    editor.add(group);
    check(editor.topAt(Point(0, 50)) == group, "группа ведёт себя как обычный объект");

    editor.remove(circle);
    check(editor.count() == 3, "удаление объекта");
    checkThrows([&] { editor.remove(circle); }, "повторное удаление");

    Editor small;
    small.add(make_shared<Rectangle>(Rect(Point(0, 0), Point(2, 0))));
    small.add(make_shared<Circle>(Point(2, 0), 0.5));
    check(small.render(3, 1) == "RRO\n", "рисунок учитывает порядок наложения");
}

void runTests() {
    testVector();
    testPoint();
    testRect();
    testCircle();
    testRectangle();
    testSegment();
    testGroup();
    testEditor();
    printResult();
}

// ---------- Демонстрация ----------

void demo() {
    Editor editor;

    ShapePtr bigRect = make_shared<Rectangle>(Rect(Point(2, 5), Point(12, 9)));
    ShapePtr bigCircle = make_shared<Circle>(Point(13, 4), 4);
    auto group = make_shared<Group>(make_shared<Rectangle>(Rect(Point(19, 1), Point(25, 3))));
    group->add(make_shared<Circle>(Point(24, 4), 2));
    ShapePtr line = make_shared<Segment>(Point(2, 11), Point(30, 11));

    // Разные типы лежат в одном редакторе. Круг добавлен позже прямоугольника, поэтому лежит выше
    editor.add(bigRect);
    editor.add(bigCircle);
    editor.add(group);
    editor.add(line);

    cout << "Рисунок (O - круг, R - прямоугольник, * - отрезок, G - группа):" << endl;
    cout << editor.render(32, 13) << endl;

    Point p(11, 6);
    cout << "Верхний объект в точке " << p.toString() << ": " << editor.topAt(p)->toString() << endl;
    cout << "(точка есть и в прямоугольнике, но круг лежит выше)" << endl << endl;

    Rect area(Point(18, 0), Point(26, 4));
    cout << "Объекты в области " << area.toString() << ":" << endl;
    for (ShapePtr s : editor.inArea(area)) cout << "  " << s->toString() << endl;

    int moved = editor.moveInArea(area, Vector(0, 5));
    cout << endl << "Подвинули объекты из области на <0, 5>: " << moved << " шт." << endl;
    cout << editor.render(32, 13) << endl;

    try {
        group->add(group);
    } catch (exception& e) {
        cout << "Ошибка (так и задумано): " << e.what() << endl;
    }
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");  // чтобы русский текст нормально выводился в консоли Windows
#endif
    cout << "=== Тесты ===" << endl;
    runTests();
    cout << endl << "=== Демонстрация ===" << endl;
    demo();
    return failed == 0 ? 0 : 1;
}
