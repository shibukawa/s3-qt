#include "s3test.h"
#include "qs3/qs3.h"
#include <QtTest>

S3Test::S3Test(QObject *parent) :
    QObject(parent)
{
}

void S3Test::testStoringInformations()
{
    QS3::S3 s3("s3.amazonaws.com", "http://10.4.5.54:8080");
    QCOMPARE(s3.hostName(), QString("s3.amazonaws.com"));
    QCOMPARE(s3.proxy(), QString("http://10.4.5.54:8080"));
}

void S3Test::testSignatureV2()
{
    QS3::S3 s3("s3.amazonaws.com", "http://10.4.5.54:8080");
    auto signature = s3.signatureV2(
            "YSBHHAQIRUHAFKH8N_ZD",
            "8TwaVRAB3Ba6Q7tpqb_VTSXuozgQ91DtDCQNwQ==",
            "GET", "Wed, 10 Dec 2014 07:21:09 +0000", "/");
    QVERIFY(signature.startsWith("AWS "));
    QVERIFY(signature.contains("YSBHHAQIRUHAFKH8N_ZD"));
    QVERIFY(signature.endsWith(":3LCCRgqhw7CQL8T8zjCasP8Vbaw="));
}
