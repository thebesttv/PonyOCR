#include "tencentapi.h"
#include "apicommon.h"
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QUuid>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

extern QMap<int, QString> tencent::ErrorCodeMap;

namespace tencent {

TencentAPI::TencentAPI(QNetworkAccessManager *manager, QObject *parent)
    : APIBase(OCRPlatform::Tencent, manager, parent)
{

}

void TencentAPI::process(const OCRRequest &req)
{
    m_AppID = m_handler.APIKey1(OCRPlatform::Tencent);
    m_AppKey = m_handler.APIKey2(OCRPlatform::Tencent);
    _process(req);
}

QString TencentAPI::getSigniture(const QString &ori)
{
    QString s = ori + "&app_key=" + m_AppKey;
    return QCryptographicHash::hash(s.toUtf8(),QCryptographicHash::Md5).toHex().toUpper();
}

void TencentAPI::processNoamrlText()
{
    processBase(GENERAL);
}

void TencentAPI::processHandwriting()
{
    processBase(HANDWRITING);
}

void TencentAPI::processBase(QString url)
{
    qDebug() << QString("TencentAPI: processBase(%1)").arg(url);
    QNetworkRequest request(url);
    setHeader(request);

    int now = time(nullptr);
    // create 32 byte random string from random uuid
    QString randomStr(QUuid::createUuid().toString());
    randomStr.remove(QRegularExpression("{|}|-"));
    qDebug() << "TencentAPI: generate random string as nonce_str: " << randomStr;

    QUrlQuery query;
    query.addQueryItem("app_id", m_AppID);
    query.addQueryItem("image", QUrl::toPercentEncoding(m_base64str));
    query.addQueryItem("nonce_str", randomStr);
    query.addQueryItem("time_stamp", QString::number(now));
    query.addQueryItem("sign", getSigniture(query.toString(QUrl::FullyEncoded)));

    m_reply = m_manager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    connectReply(m_reply);
}

void TencentAPI::parse()
{
    qDebug() << "TencentAPI: start parsing";
    QJsonObject obj = QJsonDocument::fromJson(m_array).object();
    int errCode = obj["ret"].toInt();
    if(errCode != 0) {  // error
        emit OCRFailure(OCRPlatform::Tencent, errCode,
                        QString("Message: %1\nDetail: %2")
                        .arg(obj["msg"].toString())
                        .arg(ErrorCodeMap[errCode]));
        return;
    }

    QString result;
    QJsonArray v = obj["data"].toObject()["item_list"].toArray();
    for(auto x : v){
        result += x.toObject()["itemstring"].toString() + "\n";
    }

    emit OCRSuccessful(result);
}

}
