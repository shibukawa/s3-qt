// http://stackoverflow.com/questions/12194256/qt-how-to-organize-unit-test-with-more-than-one-class

#include <QCoreApplication>
#include <QTimer>
#include "test/testrunner.h"


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    TestRunner runner(argc, argv, &app);
    QTimer::singleShot(10, &runner, SLOT(run()));
    return app.exec();
}
