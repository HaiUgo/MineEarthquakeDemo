#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QtCore/QString>
#include <QDir>
#include <QDebug>

class QProcess;
class Assistant
{
public:
    Assistant();
    ~Assistant();
    void showDocumentation(const QString &file);

private:
    bool startAssistant();

private:
    QProcess *proc;
};

#endif // ASSISTANT_H

