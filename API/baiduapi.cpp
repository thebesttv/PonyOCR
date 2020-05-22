#include "baiduapi.h"
#include "apicommon.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

extern QMap<int, QString> baidu::ErrorCodeMap;

namespace baidu {

BaiduAPI::BaiduAPI(QNetworkAccessManager *normalManager,
                   QNetworkAccessManager *proxiedManager,
                   QObject *parent)
    : APIBase(OCRPlatform::Baidu, normalManager, proxiedManager, parent)
    , m_tokenGetter(normalManager, proxiedManager, this)
    , m_tableProcessor(normalManager, proxiedManager, this)
{
    connect(&m_tokenGetter, &BaiduAccessToken::authorizationFailure,
            this, &BaiduAPI::authorizationFailure);

    connect(&m_tableProcessor, &BaiduTableProcessor::OCRSuccessful,
            this, &BaiduAPI::OCRSuccessful);
    connect(&m_tableProcessor, &BaiduTableProcessor::OCRFailure,
            this, &BaiduAPI::OCRFailure);
}

QUrl BaiduAPI::flavoredUrl(const QString &s, const QString &at)
{
    QUrl url(s); QUrlQuery query;
    query.addQueryItem("access_token", at);
    url.setQuery(query);
    return url;
}

void BaiduAPI::processNoamrlText()
{
    processBase(ACCURATE_BASIC);
}

void BaiduAPI::processWebImage()
{
    processBase(WEB_IMAGE);
}

void BaiduAPI::processTable()
{
    QString token = m_tokenGetter.getAccessToken();
    if(token.isEmpty())
        return;
    m_tableProcessor.process(m_base64str,
                             flavoredUrl(TABLE_REQUEST, token),
                             flavoredUrl(TABLE_RESULT, token));
}

void BaiduAPI::processQRCode()
{
    processBase(QRCODE);
}

void BaiduAPI::processNumbers()
{
    processBase(NUMBERS);
}

void BaiduAPI::processHandwriting()
{
    processBase(HANDWRITING);
}

void BaiduAPI::processFormula()
{
    processBase(FORMULA);
}

void BaiduAPI::processBase(QString url)
{
    QString token = m_tokenGetter.getAccessToken();
    if(token.isEmpty())
        return;

    QNetworkRequest request(flavoredUrl(url, token));
    setHeader(request);

    QUrlQuery query;
    query.addQueryItem("image", QUrl::toPercentEncoding(m_base64str));
    if(m_handler.useProxy(OCRPlatform::Baidu))
        m_reply = m_proxiedManager->post(request,
                                         query.toString(QUrl::FullyEncoded).toUtf8());
    else
        m_reply = m_normalManager->post(request,
                                        query.toString(QUrl::FullyEncoded).toUtf8());

    connectReply(m_reply);
}

void BaiduAPI::parse()
{
    m_reply->deleteLater();
    qInfo().noquote() << QString("%1: request finished, start parsing")
                         .arg(ConfigHandler::asPlatformName(m_platform));

    QJsonObject obj = QJsonDocument::fromJson(m_array).object();
    if(obj.contains("error_code")){
        int errCode = obj["error_code"].toInt();
        QString errMsg =  tr("%1\nDetail: %2")
                          .arg(obj["error_msg"].toString())
                          .arg(ErrorCodeMap[errCode]);
        qCritical().noquote() << QString("parse failed");
        emit OCRFailure(OCRPlatform::Baidu, errCode, errMsg);
        return;
    }

    QString result;
    if (m_req.m_mode == OCRMode::QRCode) {
        QJsonArray array = obj["codes_result"].toArray();
        for(auto v : array){
            QJsonArray texts = v.toObject()["text"].toArray();
            for(auto x : texts) result += x.toString() + '\n';
        }
    } else {
        QJsonArray array = obj["words_result"].toArray();
        for(auto v : array){
            QJsonObject obj(v.toObject());
            result += obj["words"].toString() + '\n';
        }
    }

    qInfo().noquote() << "parse successful";
    emit OCRSuccessful(result);
}

}
