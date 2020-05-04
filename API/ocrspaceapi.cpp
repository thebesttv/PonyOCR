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

OCRSpaceAPI::OCRSpaceAPI(QNetworkAccessManager *manager, QObject *parent)
    : APIBase(OCRPlatform::OCR_Space, manager, parent)
{

}

void OCRSpaceAPI::setHeader(QNetworkRequest &request)
{
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    request.setRawHeader("apikey", m_handler.APIKey1(OCRPlatform::OCR_Space).toUtf8());
}

void OCRSpaceAPI::processNoamrlText()
{
    QNetworkRequest request(GENERAL);
    setHeader(request);

    QUrlQuery query;
    m_base64str = "data:image/png;base64," + m_base64str;
    query.addQueryItem("base64Image",  QUrl::toPercentEncoding(m_base64str));
    query.addQueryItem("language", "chs");
    query.addQueryItem("scale", "true");
    query.addQueryItem("OCREngine", "1");
    m_reply = m_manager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());

    connectReply(m_reply);
}

void OCRSpaceAPI::parse()
{
    m_reply->deleteLater();

    qDebug().noquote() << "OCR.Space: request finished, start parsing";
    QJsonObject obj(QJsonDocument::fromJson(m_array).object());

    int exitCode = obj["OCRExitCode"].toInt();
    if(exitCode != 1) {
        emit OCRFailure(OCRPlatform::OCR_Space, exitCode, ErrorCodeMap[exitCode]);
        qWarning().noquote() << QString("parse failed with exit code: %1")
                                .arg(exitCode);
        if(exitCode != 2) return;
    }

    QJsonArray array = obj["ParsedResults"].toArray();
    QString res;
    for(auto p : array) {
        if(!res.isEmpty()) res += '\n';
        res += parseSingleResult(p.toObject());
    }

    qDebug().noquote() << "parse successful";
    emit OCRSuccessful(res);
}

QString OCRSpaceAPI::parseSingleResult(const QJsonObject &source)
{
    QJsonObject obj = source;
    int exitCode = obj["FileParseExitCode"].toInt();
    if(exitCode != 1) {
        QString errMsg =  QString("Message: %1\nDetail:%2")
                          .arg(obj["ErrorMessage"].toString())
                          .arg(obj["ErrorDetails"].toString());
        qWarning().noquote() << QString("parse failed with exit code: %1\n%2")
                                .arg(exitCode).arg(errMsg);
        emit OCRFailure(OCRPlatform::OCR_Space, exitCode, errMsg);
        return "";
    }

    return obj["ParsedText"].toString();
}

}
