#ifndef SCHEDULE_H
#define SCHEDULE_H


#include <QGraphicsView>
#include <QGraphicsScene>
#include <list>
#ifdef Q_OS_WIN
#include <ctime>
#endif
#include "pair.h"

// Класс для хранения и отрисовки меток скорости на графике.
class Schedule
{
    std::list<Pair> elems;
    int s_time     = time(NULL);
    int s_quantity = 0;
    int diff       = 0;

public:
    Schedule();
    void addElem(int elem);
    int size() {return elems.size();}

    int maxValue();
    void showChart(QGraphicsScene *scene, QPen pen, QColor color, int width, int height, int maxVal);
};

#endif // SCHEDULE_H
