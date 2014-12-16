#include "buckettest.h"
#include "qs3/qs3.h"
#include <QEventLoop>
#include <QDebug>
#include <QtTest>
#include <QScopedPointer>

const QString riakcsHelperPath = "/Users/shibukawa.yoshiki/gobin/riakcs-helper";
const QString testBucketName = "bucket-for-unittest";
const QString testUserAccessKey = "OTJDXRPHUPUG2KLDZNPD";
const QString testUserSecretKey = "iRZUq4d_Rk9wMjBS1lTeYz0BPdvifW8wogXK3w==";
const QString s3Host = "s3.amazonaws.com";
const QString s3Proxy = "http://10.4.5.54:8080";

BucketTest::BucketTest(QObject *parent) :
    QObject(parent)
{
}

BucketTest::~BucketTest()
{
}

void BucketTest::init()
{
    this->_util = new TestUtil(riakcsHelperPath);
}

void BucketTest::testSendItem()
{
    this->_util->initTestBucket(testBucketName);

    QS3::S3 s3(s3Host, s3Proxy);
    QScopedPointer<QS3::Bucket> bucket(s3.bucket(testBucketName, testUserAccessKey, testUserSecretKey));

    QByteArray data("test data");
    bucket->upload("test_01.txt", data);

    // wait finish
    QEventLoop loop;
    connect(bucket.data(), SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    auto content = this->_util->getBucketContents();
    QVERIFY(content.contains("test_01.txt"));
}

void BucketTest::testReceiveItem()
{
    this->_util->initTestBucket(testBucketName);

    QS3::S3 s3(s3Host, s3Proxy);
    QScopedPointer<QS3::Bucket> bucket(s3.bucket(testBucketName, testUserAccessKey, testUserSecretKey));

    QByteArray data("test data");
    bucket->upload("test_01.txt", data);

    // wait finish
    QEventLoop loop;
    connect(bucket.data(), SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    // receive uploaded data
    QString receivedKey;
    QByteArray receivedContent;
    connect(bucket.data(), &QS3::Bucket::downloaded, [&] (const QString& key, const QByteArray& content) {
        receivedKey = key;
        receivedContent = content;
    });
    bucket->download("test_01.txt");
    connect(bucket.data(), SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QCOMPARE(receivedKey, QString("test_01.txt"));
    QCOMPARE(receivedContent, QByteArray("test data"));
}

void BucketTest::testReceiveSyncItem()
{
    this->_util->initTestBucket(testBucketName);
    QS3::S3 s3(s3Host, s3Proxy);
    QScopedPointer<QS3::Bucket> bucket(s3.bucket(testBucketName, testUserAccessKey, testUserSecretKey));

    QByteArray data("test data");
    bucket->upload("test_02.txt", data);

    // wait finish
    QEventLoop loop;
    connect(bucket.data(), SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    // receive uploaded data
    QByteArray receivedContent;
    int httpStatus = bucket->downloadSync("test_02.txt", receivedContent);
    QCOMPARE(httpStatus, 200);
    QCOMPARE(receivedContent, QByteArray("test data"));
}

void BucketTest::cleanup()
{
    delete this->_util;
    this->_util = nullptr;
}
