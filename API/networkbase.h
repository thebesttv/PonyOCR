#ifndef NETWORKBASE_H
#define NETWORKBASE_H

#include <QObject>
#include "Configuation/confighandler.h"

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

class NetworkBase : public QObject {
public:
    explicit NetworkBase(QNetworkAccessManager *manager, QObject *parent = nullptr);

protected:
    ConfigHandler m_handler;
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QByteArray m_array;

    virtual void setHeader(QNetworkRequest &request);
    virtual void connectReply(QNetworkReply *reply);

private slots:
    void readyRead();
    virtual void parse();
};

#endif // NETWORKBASE_H
