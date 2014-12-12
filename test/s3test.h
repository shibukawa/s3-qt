#ifndef S3TEST_H
#define S3TEST_H

#include <QObject>

class S3Test : public QObject
{
    Q_OBJECT
public:
    explicit S3Test(QObject *parent = 0);

signals:

private slots:
    void testStoringInformations();
    void testSignatureV2();
};

#endif // S3TEST_H
