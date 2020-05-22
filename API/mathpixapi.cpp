#include "mathpixapi.h"
#include "apicommon.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>

namespace mathpix {

MathpixAPI::MathpixAPI(QNetworkAccessManager *normalManager,
                       QNetworkAccessManager *proxiedManager,
                       QObject *parent)
    : APIBase(OCRPlatform::Mathpix, normalManager, proxiedManager, parent)
{

}

void MathpixAPI::setHeader(QNetworkRequest &request)
{
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("app_id", m_handler.APIKey1(Mathpix).toUtf8());
    request.setRawHeader("app_key", m_handler.APIKey2(Mathpix).toUtf8());
}

void MathpixAPI::processFormula()
{
    QNetworkRequest request(V3_TEXT);
    setHeader(request);

    m_base64str = "{ \"src\": \"data:image/jpeg;base64," + m_base64str + "\" }";
    if(m_handler.useProxy(OCRPlatform::Mathpix))
        m_reply = m_proxiedManager->post(request, m_base64str.toUtf8());
    else
        m_reply = m_normalManager->post(request, m_base64str.toUtf8());

    connectReply(m_reply);
}

void MathpixAPI::parse()
{
    m_reply->deleteLater();
    qInfo().noquote() << QString("%1: request finished, start parsing")
                         .arg(ConfigHandler::asPlatformName(m_platform));

    QJsonObject obj = QJsonDocument::fromJson(m_array).object();
    QString res = obj["text"].toString();

    res.replace("\\", "\\\\");

    QRegularExpression rx("(\\\\\\\\)([a-zA-Z])");
    // match 2 backslash followed by a letter
    res.replace(rx, "\\\\2");

    qInfo().noquote() << "parse successful";
    emit OCRSuccessful(res);
}

}
