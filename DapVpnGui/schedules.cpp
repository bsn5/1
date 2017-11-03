#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDebug>

#include "schedules.h"


Schedules::Schedules(){}

void Schedules::addInp(int elem){inp.addElem(elem);}
void Schedules::addOut(int elem){out.addElem(elem);}

void Schedules::draw_backgraund(QGraphicsScene *scene, int width, int height)
{
    QPainterPath path;

    int num_of_lines = 14;

    for (int i = 1; i < num_of_lines; i++){
        path.moveTo(i*width/num_of_lines, 0);
        path.lineTo(i*width/num_of_lines, height);
    }

    num_of_lines = 18;
    for (int i = 1; i < num_of_lines; i++){
        path.moveTo(0, i*height/num_of_lines);
        path.lineTo(width, i*height/num_of_lines);
    }

    scene->addPath(path, QPen(QColor(3, 190, 98, 127), 0.2));
}


int maxInt (int a, int b) {return a > b ? a : b;}


void Schedules::draw_chart(QGraphicsScene *scene, int width, int height)
{
    scene->clear();
    draw_backgraund(scene, width, height);

    if (out.size() < 2) return;

    int maxValue = maxInt(inp.maxValue(), out.maxValue());

    inp.showChart(
        scene,
        QPen(QColor(0, 134, 100), 1),
        QColor(0, 183, 43, 13),
         width, height, maxValue);

    out.showChart(
        scene,
        QPen(QColor(3, 190, 98), 1),
        QColor(3, 167, 198, 20),
        width, height, maxValue);
    // TODO нарисовать рамку

}
