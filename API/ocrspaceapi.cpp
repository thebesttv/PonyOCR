#include "ocrspaceapi.h"
#include "apicommon.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

extern QMap<int, QString> ocrspace::ErrorCodeMap;

namespace ocrspace {

OCRSpaceAPI::OCRSpaceAPI(QNetworkAccessManager *normalManager,
                         QNetworkAccessManager *proxiedManager,
                         QObject *parent)
    : APIBase(OCRPlatform::OCR_Space, normalManager, proxiedManager, parent)
{

}

void OCRSpaceAPI::setHeader(QNetworkRequest &request)
{
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setRawHeader("apikey", m_handler.APIKey1(OCRPlatform::OCR_Space).toUtf8());
}

void OCRSpaceAPI::processBase(bool isTable, int engine)
{
    if(engine != 1 && engine !=2) return;
    QNetworkRequest request(GENERAL);
    setHeader(request);

    QUrlQuery query;
    m_base64str = "data:image/png;base64," + m_base64str; // add file type
    query.addQueryItem("base64Image",  QUrl::toPercentEncoding(m_base64str));
    query.addQueryItem("language", "chs");
    query.addQueryItem("isTable", isTable ? "true" : "false");
    query.addQueryItem("scale", "true");
    query.addQueryItem("OCREngine", engine == 1 ? "1" : "2");
    if(m_handler.useProxy(OCRPlatform::OCR_Space))
        m_reply = m_proxiedManager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    else
        m_reply = m_normalManager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());

    connectReply(m_reply);
}

void OCRSpaceAPI::processNoamrlText()
{
    processBase(false, 1);
}

void OCRSpaceAPI::processTable()
{
    processBase(true, 1);
}

void OCRSpaceAPI::parse()
{
    m_reply->deleteLater();
    qInfo().noquote() << QString("%1: request finished, start parsing")
                         .arg(ConfigHandler::asPlatformName(m_platform));

    QJsonObject obj(QJsonDocument::fromJson(m_array).object());

    int exitCode = obj["OCRExitCode"].toInt();
    if(exitCode != 1) {
        emit OCRFailure(OCRPlatform::OCR_Space, exitCode, ErrorCodeMap[exitCode]);
        if(exitCode > 2) {
            qCritical().noquote() << "parse failed completely";
            return;
        } else
            qWarning().noquote() << "only parsed partially";
    }

    QJsonArray array = obj["ParsedResults"].toArray();
    QString res;
    for(auto p : array) {
        if(!res.isEmpty()) res += '\n';
        res += parseSingleResult(p.toObject());
    }

    qInfo().noquote() << "parse successful";
    emit OCRSuccessful(res);
}

QString OCRSpaceAPI::parseSingleResult(const QJsonObject &source)
{
    QJsonObject obj = source;
    int exitCode = obj["FileParseExitCode"].toInt();
    if(exitCode != 1) {
        QString errMsg = tr("%1\nDetail:%2")
                         .arg(obj["ErrorMessage"].toString())
                         .arg(obj["ErrorDetails"].toString());
        qCritical().noquote() << QString("parse failed");
        emit OCRFailure(OCRPlatform::OCR_Space, exitCode, errMsg);
        return "";
    }

    return obj["ParsedText"].toString();
}

}
