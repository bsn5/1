#include <stdlib.h>
#include <list>
#include <iterator>
#include <QDebug>
#include <QGraphicsPathItem>

#include "schedule.h"

Schedule::Schedule() {
    elems.push_front(Pair(time(NULL),0));
}

void Schedule::addElem(int newQuantity)
{
    int newTime = time(NULL);
    int velocity;

    if (newTime == s_time) {
        velocity = diff = diff + newQuantity - s_quantity;
        // убираем ненужный элемент.
        elems.pop_front();
    } else {
        diff     = newQuantity - s_quantity;
        velocity = diff/(newTime - s_time);
        s_time   = newTime;
    }
    elems.push_front(Pair(newTime, velocity));
    s_quantity = newQuantity;

    if (elems.size() > 40) {
        elems.pop_back();
    }
}

int Schedule::maxValue()
{
    if (size() == 0) {
        return 0;
    }

    int maxVal = elems.begin()->velocity;
    std::list<Pair>::iterator ptr;

    for (ptr = elems.begin(); ptr != elems.end(); ptr++) {
        if (ptr->velocity > maxVal) maxVal = ptr->velocity;
    }
    return maxVal;
}

int x_shift(int x, int width, int number_of_elem)
{
    if (number_of_elem == 0) {
        number_of_elem = 1;
    }

    int res = x * width / number_of_elem;
    return res;
}

int y_shift(int y, int height, int maxValue)
{
    if (maxValue == 0) {
        maxValue = 1;
    }
    int res = height - y * height / maxValue;
    return res;
}


void Schedule::showChart(
    QGraphicsScene *scene, QPen pen, QColor color, int width, int height, int maxVal)
{
    int size_of_chart = size();
    std::list<Pair>::iterator ptr;
    QPainterPath path = QPainterPath();

    // выставляем на начальные позиции
    path.moveTo(
        width,
        y_shift(elems.begin()->velocity, height, maxVal)
    );

    int time_pos = size_of_chart - 1;
    // отрисовываем путь
    for (ptr = elems.begin(); ptr != elems.end(); ptr++) {
        int y = 0;
        if (ptr->velocity > y) y = ptr->velocity;

        path.lineTo(
            x_shift(time_pos, width, size_of_chart),
            y_shift(y, height, maxVal));
        time_pos--;
    }

    // замкнуть график по низу
    path.lineTo(0,     height);
    path.lineTo(width, height);
    path.closeSubpath();

    // отрисовка графика
    scene->addPath(path, pen, color);
}

