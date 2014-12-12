#include "qs3/qs3.h"
#include "qs3/qbucket.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWriteLocker>
#include <QNetworkProxy>
#include <QUrl>
#include <QDebug>

namespace QS3 {

Bucket::Bucket(const QString& bucketName, const S3* s3, const QString& accessKey, const QString& secretKey, QObject *parent) :
    QObject(parent), _bucketName(bucketName), _s3(s3), _accessKey(accessKey), _secretKey(secretKey)
{
    this->_manager = new QNetworkAccessManager();

    // https://gist.github.com/gokmen/2558692
    this->connect(this->_manager, &QNetworkAccessManager::sslErrors, [=](QNetworkReply* qnr, QList<QSslError> errors) {
        qDebug() << "sslErrorHandler: " << qnr->url().toString();
        // show list of all ssl errors
        foreach (QSslError err, errors) {
            qDebug() << "ssl error: " << err.errorString() << err;
        }
        //qDebug() << qnr->sslConfiguration().peerCertificate().toText();
        qnr->ignoreSslErrors(errors);
    });

    auto proxyUrl = s3->proxy();
    if (!proxyUrl.isEmpty()) {
        QUrl url(proxyUrl);
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, url.host(), url.port());
        this->_manager->setProxy(proxy);
    }

    connect(this->_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(_receiveFinished(QNetworkReply*)));
}

Bucket::~Bucket()
{
    delete this->_manager;
}

void Bucket::upload(const QString &name, const QByteArray &data)
{
    QWriteLocker locker(&this->_lock);

    auto task = new NetworkTask(name);
    task->total = data.size(); // temp size

    auto host = this->_s3->hostName();
    QUrl url(QString("http://%1.%2/%3").arg(this->_bucketName, host, name));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(data.size()));
    request.setRawHeader("Host", QString("%1.%2").arg(this->_bucketName, host).toLocal8Bit());
    this->_s3->signV2(request, this->_accessKey, this->_secretKey, "PUT", QString("/%1/%2").arg(this->_bucketName, name));

    QNetworkReply* reply = this->_manager->put(request, data);

    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_receiveProgress(qint64,qint64)));
    connect(this->_manager, &QNetworkAccessManager::sslErrors, [=](QNetworkReply* reply, QList<QSslError> errors) {
        qDebug() << "ssl errors";
        foreach(auto error, errors) {
            qDebug() << error.errorString();
        }
        reply->ignoreSslErrors(errors);
    });

    this->_tasks[reply] = task;
    this->_sendProgress();
}

void Bucket::download(const QString &name)
{
    QWriteLocker locker(&this->_lock);

    auto task = new NetworkTask(name, false);
    auto host = this->_s3->hostName();
    QUrl url(QString("http://%1.%2/%3").arg(this->_bucketName, host, name));
    QNetworkRequest request(url);
    request.setRawHeader("Host", QString("%1.%2").arg(this->_bucketName, host).toLocal8Bit());
    this->_s3->signV2(request, this->_accessKey, this->_secretKey, "GET", QString("/%1/%2").arg(this->_bucketName, name));

    QNetworkReply* reply = this->_manager->get(request);

    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_receiveProgress(qint64,qint64)));
    connect(this->_manager, &QNetworkAccessManager::sslErrors, [=](QNetworkReply* reply, QList<QSslError> errors) {
        qDebug() << "ssl errors";
        foreach(auto error, errors) {
            qDebug() << error.errorString();
        }
        reply->ignoreSslErrors(errors);
    });

    this->_tasks[reply] = task;
    this->_sendProgress();
}

void Bucket::_receiveFinished(QNetworkReply *reply)
{
    QWriteLocker locker(&this->_lock);
    auto task = this->_tasks[reply];
    task->sent = task->total;
    task->finished = true;

    reply->deleteLater();

    bool isAllFinished = this->_sendProgress();

    if (task->send) {
        emit uploaded(task->key);
    } else {
        QByteArray receivedData = reply->readAll();
        emit downloaded(task->key, receivedData);
    }
    if (isAllFinished) {
        emit finished();
        qDeleteAll(this->_tasks.values());
        this->_tasks.clear();
    }
}

void Bucket::_receiveProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QWriteLocker locker(&this->_lock);
    auto reply = qobject_cast<QNetworkReply*>(this->sender());
    auto task = this->_tasks[reply];
    if (task) {
        task->sent = bytesSent;
        task->total = bytesTotal;
    }
    this->_sendProgress();
}

bool Bucket::_sendProgress()
{
    qint64 sent = 0;
    qint64 total = 0;
    bool finished = true;
    for (const NetworkTask* task : this->_tasks.values()) {
        sent += task->sent;
        total += task->total;
        finished &= task->finished;
    }
    emit progress(sent, total);
    return finished;
}

}
