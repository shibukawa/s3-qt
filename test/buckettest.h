#ifndef BUCKETTEST_H
#define BUCKETTEST_H

#include <QObject>
#include "test/testutil.h"

class BucketTest : public QObject
{
    Q_OBJECT
public:
    explicit BucketTest(QObject *parent = 0);
    virtual ~BucketTest();

signals:

private slots:
    void init();
    void testSendItem();
    void testReceiveItem();
    void testReceiveSyncItem();
    void cleanup();

protected:
    TestUtil* _util;
};

#endif // BUCKETTEST_H
