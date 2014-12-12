#ifndef QBUCKET_H
#define QBUCKET_H

#include <QMap>
#include <QObject>
#include <QRunnable>
#include <QReadWriteLock>

class QNetworkAccessManager;
class QNetworkReply;

namespace QS3 {

class S3;

struct NetworkTask {
    NetworkTask(const QString& filename, bool sendFlag=true) : total(0), sent(0), finished(false), key(filename), send(sendFlag) {}
    qint64 total;
    qint64 sent;
    bool finished;
    QString key;
    bool send;
};

class Bucket : public QObject
{
    Q_OBJECT
public:
    explicit Bucket(const QString& bucketName, const S3* s3, const QString& accessKey, const QString& secretKey, QObject *parent = 0);
    ~Bucket();
    void upload(const QString& name, const QByteArray& data);
    void download(const QString& name);

signals:
    void finished();
    void uploaded(const QString& key);
    void progressTask(const QString& key, qint64 sent, qint64 total);
    void progress(qint64 sent, qint64 total);
    void downloaded(const QString& key, const QByteArray& data);

private slots:
    void _receiveFinished(QNetworkReply* reply);
    void _receiveProgress(qint64 bytesSent, qint64 bytesTotal);

protected:
    QReadWriteLock _lock;

    QString _bucketName;
    const S3* _s3;
    QString _accessKey;
    QString _secretKey;
    QNetworkAccessManager* _manager;
    QMap<QNetworkReply*, NetworkTask*> _tasks;

    bool _sendProgress();
};

}

#endif // QBUCKET_H
