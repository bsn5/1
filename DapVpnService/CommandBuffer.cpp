#include <QString>
#include <QList>
#include "CommandBuffer.h"

void CommandBuffer::listFilter()
{

    // "connect"
    // "logout"
    QList<QString> newCommandList;

    int ind = -1;

    for (int i = 0; i < listOfCommand.size(); ++i) {
        QStringList cmdSub = listOfCommand.at(i).split(' ');
        if (cmdSub.length() == 0) continue;
        if (cmdSub[0] == "connect" || cmdSub[0] == "logout") {
            ind = i;
            continue;
        }
        newCommandList.push_front(listOfCommand.at(i));
    }

    if (ind != -1) {
        newCommandList.push_front(listOfCommand.at(ind));
    }

    listOfCommand = newCommandList;
}


QString CommandBuffer::readCommand()
{
    QString command = listOfCommand.back();
    listOfCommand.removeLast();
    return command;
}
