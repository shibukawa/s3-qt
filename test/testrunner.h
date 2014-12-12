#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <QObject>

class TestRunner : public QObject
{
    Q_OBJECT
    int _argc;
    char** _argv;
public:
    explicit TestRunner(int argc, char** argv, QObject* parent=0);
    ~TestRunner();

public slots:
    void run();
};

#endif // TESTRUNNER_H
