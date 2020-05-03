#include "networkbase.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

NetworkBase::NetworkBase(QNetworkAccessManager *manager, QObject *parent)
    : QObject(parent), m_manager(manager)
{
}

void NetworkBase::setHeader(QNetworkRequest &request)
{
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
}

void NetworkBase::connectReply(QNetworkReply *reply)
{
    connect(reply, &QNetworkReply::readyRead,
            this, &NetworkBase::readyRead);
    connect(reply, &QNetworkReply::finished,
            this, &NetworkBase::parse);
}

void NetworkBase::readyRead()
{
    m_array.append(m_reply->readAll());
}

void NetworkBase::parse()
{
    m_reply->deleteLater();
}
