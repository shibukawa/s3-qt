#ifndef TESTUTILTEST_H
#define TESTUTILTEST_H

#include <QObject>

class TestUtilTest : public QObject
{
    Q_OBJECT
public:
    explicit TestUtilTest(QObject *parent = 0);

signals:

private slots:
    void initBucketTest();
    void getBucketContentsTest();
};

#endif // TESTUTILTEST_H
