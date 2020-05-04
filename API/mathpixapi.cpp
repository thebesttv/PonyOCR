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

#include <QFile>

namespace mathpix {

MathpixAPI::MathpixAPI(QNetworkAccessManager *manager, QObject *parent)
    : APIBase(OCRPlatform::Mathpix, manager, parent)
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
    m_reply = m_manager->post(request, m_base64str.toUtf8());

    connectReply(m_reply);
}

void MathpixAPI::parse()
{
    m_reply->deleteLater();

    qDebug().noquote() << "Mathpix: request finished, start parsing";
    QJsonObject obj = QJsonDocument::fromJson(m_array).object();
    QString res = obj["text"].toString();

    res.replace("\\", "\\\\");

    QRegularExpression rx("(\\\\\\\\)([a-zA-Z])");
    // match 2 backslash followed by a letter
    res.replace(rx, "\\\\2");

    qDebug().noquote() << "parse successful";
    emit OCRSuccessful(res);
}

}
