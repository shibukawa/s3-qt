#include "testutiltest.h"
#include "test/testutil.h"
#include "test/externalprocess.h"
#include <QtTest>
#include <QDebug>

const QString s3cmdPath = "/opt/local/Library/Frameworks/Python.framework/Versions/Current/bin/s3cmd";
const QString riakcsHelperPath = "/Users/shibukawa.yoshiki/gobin/riakcs-helper";
const QString testUserAccessKey = "OTJDXRPHUPUG2KLDZNPD";
const QString testUserSecretKey = "iRZUq4d_Rk9wMjBS1lTeYz0BPdvifW8wogXK3w==";
const QString testBucketName = "bucket-for-unittest";

TestUtilTest::TestUtilTest(QObject *parent) :
    QObject(parent)
{
}

void TestUtilTest::initBucketTest()
{
    TestUtil util(riakcsHelperPath);
    util.initTestBucket(testBucketName);

    ExternalProcess process(false, true);
    process.run(s3cmdPath, QStringList() << "ls" << QString("s3://%1").arg(testBucketName));
    process.waitForFinished();
    QCOMPARE(process.stdoutResult(), QString(""));
}

void TestUtilTest::getBucketContentsTest()
{
    TestUtil util(riakcsHelperPath);
    util.initTestBucket(testBucketName);
    QVERIFY(util.getBucketContents().contains("empty"));
}
