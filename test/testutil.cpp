#include "testutil.h"
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <stdexcept>
#include <QEventLoop>
#include "externalprocess.h"
#include <QTimer>
#include <time.h>

TestUtil::TestUtil(const QString &riakcsHelperPath)
    : _riakcsHelperPath(riakcsHelperPath)
{
}

TestUtil::~TestUtil()
{
    if (!this->_testDir.isEmpty() && !this->_skipRemove) {
        QDir tempdir(this->_testDir);
        tempdir.removeRecursively();
    }
}

QDir TestUtil::makeTestDir(bool skipRemove)
{
    this->_skipRemove = this->_skipRemove || skipRemove;
    if (!this->_testDir.isNull()) {
        return QDir(this->_testDir);
    }
    QDir temp = QDir::temp();
    QString dirname = QString::number(QDateTime::currentMSecsSinceEpoch());
    temp.mkdir(dirname);
    this->_testDir = temp.filePath(dirname);
    if (skipRemove) {
        qDebug() << "test dir:" << this->_testDir;
    }
    return QDir(this->_testDir);
}

void TestUtil::initTestBucket(const QString &bucketName)
{
    if (!this->_testBucket.isEmpty()) {
        return;
    }
    this->_testBucket = bucketName;
    ExternalProcess process(false, true);
    QStringList args;
    args << "clean-bucket" << bucketName;
    process.run(this->_riakcsHelperPath, args);
    process.waitForFinished();
}

QString TestUtil::getBucketContents()
{
    ExternalProcess process(false, true);
    QStringList args;
    args << "list" << this->_testBucket;
    process.run(this->_riakcsHelperPath, args);
    process.waitForFinished();
    return process.stdoutResult();
}

void TestUtil::makeFile(const QString filename, const QString content)
{
    if (this->_testDir.isEmpty()) {
        throw new std::logic_error("temp dir is not initialized");
    }
    QDir tempdir(this->_testDir);
    QString filepath = tempdir.filePath(filename);
    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw new std::logic_error("file creation error");
    }
    QTextStream stream(&file);
    stream << content << endl;
    file.close();
}

QString TestUtil::readFile(const QString &filename)
{
    if (this->_testDir.isEmpty()) {
        throw new std::logic_error("temp dir is not initialized");
    }
    QDir tempdir(this->_testDir);
    QString filepath = tempdir.filePath(filename);
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw new std::logic_error("file creation error");
    }
    QTextStream stream(&file);
    QString result = stream.readAll();
    file.close();
    return result;
}
