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

BaiduAPI::BaiduAPI(QNetworkAccessManager *manager, QObject *parent)
    : APIBase(OCRPlatform::Baidu, manager, parent)
    , m_tokengetter(manager, this)
    , m_tableProcessor(manager, this)
{
    connect(&m_tokengetter, &BaiduAccessToken::authorizationFailure,
            this, &APIBase::authorizationFailure);

    connect(&m_tableProcessor, &BaiduTableProcessor::OCRSuccessful,
            this, &APIBase::OCRSuccessful);
    connect(&m_tableProcessor, &BaiduTableProcessor::OCRFailure,
            this, &APIBase::OCRFailure);
}

QUrl BaiduAPI::flavoredUrl(const QString &s)
{
    QUrl url(s); QUrlQuery query;
    query.addQueryItem("access_token", m_tokengetter.getAccessToken());
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
    m_tableProcessor.process(m_base64str,
                             flavoredUrl(TABLE_REQUEST),
                             flavoredUrl(TABLE_RESULT));
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
    QNetworkRequest request(flavoredUrl(url));
    setHeader(request);

    QUrlQuery query;
    query.addQueryItem("image", QUrl::toPercentEncoding(m_base64str));
    m_reply = m_manager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());

    connectReply(m_reply);
}

void BaiduAPI::parse()
{
    qDebug() << "BaiduAPI: start parsing";
    m_reply->deleteLater();

    QJsonObject obj = QJsonDocument::fromJson(m_array).object();
    if(obj.contains("error_code")){
        int errCode = obj["error_code"].toInt();
        emit OCRFailure(OCRPlatform::Baidu, errCode,
                        QString("Message: %1\nDetail: %2")
                        .arg(obj["error_msg"].toString())
                        .arg(ErrorCodeMap[errCode]));
        return;
    }

    QString result;
    if (m_req.m_mode == OCRMode::NormalText
    || m_req.m_mode == OCRMode::Handwriting) {
        if(m_req.m_mode == OCRMode::NormalText) qDebug() << "Normal Text";
        else qDebug() << "Handwriting";
        QJsonArray array = obj["words_result"].toArray();
        for(auto v : array){
            QJsonObject obj(v.toObject());
            result += obj["words"].toString() + '\n';
        }
    } else if (m_req.m_mode == OCRMode::QRCode) {
        qDebug() << "QR Code";
        QJsonArray array = obj["codes_result"].toArray();
        for(auto v : array){
            QJsonArray texts = v.toObject()["text"].toArray();
            for(auto x : texts) result += x.toString() + '\n';
        }
    }

    emit OCRSuccessful(result);
}

}
