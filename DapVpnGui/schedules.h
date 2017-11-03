#ifndef SCHEDULES_H
#define SCHEDULES_H

#include <QGraphicsView>
#include <QGraphicsScene>

#include "schedule.h"

class Schedules
{

private:
    Schedule inp = Schedule();
    Schedule out = Schedule();

    void draw_backgraund(QGraphicsScene *scene, int width, int height);

public:
    void addInp(int elem);
    void addOut(int elem);

    void draw_chart(QGraphicsScene *scene, int width, int height);
    Schedules();
};



#endif // SCHEDULES_H
