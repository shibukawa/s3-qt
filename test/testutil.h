#ifndef TESTUTIL_H
#define TESTUTIL_H

#include <QString>
#include <QDir>

class ExternalProcess;

class TestUtil : QObject
{
    Q_OBJECT
public:
    TestUtil(const QString& riakcsHelperPath="");
    ~TestUtil();

    QDir makeTestDir(bool skipRemove=false);
    void initTestBucket(const QString& bucketName);
    QString getBucketContents();
    void makeFile(const QString filename, const QString content);
    QString readFile(const QString& filename);
    
private:
    QString _riakcsHelperPath;
    QString _testDir;
    QString _testBucket;
    bool _skipRemove;
};

#endif // TESTUTIL_H
