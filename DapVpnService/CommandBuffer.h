#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H


class CommandBuffer
{
    QList<QString> listOfCommand;
public:
    void listFilter();
    void addCommand(QString cmd) {listOfCommand.push_front(cmd);}
    bool isEmpty(){return listOfCommand.size() == 0;}
    int size(){return listOfCommand.size();}
    QString readCommand();
    CommandBuffer(){}
};

#endif // COMMANDBUFFER_H
