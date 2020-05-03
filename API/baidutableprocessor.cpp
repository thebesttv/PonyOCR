#include "baidutableprocessor.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include "apicommon.h"

namespace baidu {

BaiduTableProcessor::BaiduTableProcessor(QNetworkAccessManager *manager, QObject *parent)
    : NetworkBase(manager, parent)
{

}

void BaiduTableProcessor::process(const QString &base64str,
                                  const QUrl &requestUrl, const QUrl &replyUrl)
{
    m_requestUrl = requestUrl, m_replyUrl = replyUrl;
    m_base64str = base64str;
    tableRequest();
}

void BaiduTableProcessor::tableRequest()
{
    m_array.clear(); m_stage = 1;

    QNetworkRequest request(m_requestUrl);
    setHeader(request);
    // 默认异步
    QUrlQuery query;
    query.addQueryItem("image", QUrl::toPercentEncoding(m_base64str));
    m_reply = m_manager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    connectReply(m_reply);
}

void BaiduTableProcessor::tableReply()
{
    m_array.clear();
    m_stage = 2;

    QNetworkRequest request(m_replyUrl);
    setHeader(request);
    QUrlQuery query;
    query.addQueryItem("request_id", m_requestID);
    query.addQueryItem("result_type", "json");
    m_reply = m_manager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    connectReply(m_reply);
}

void BaiduTableProcessor::parse()
{
    QJsonDocument doc(QJsonDocument::fromJson(m_array));
    QJsonObject obj(doc.object());

    if(obj.contains("error_code")){
        int errCode = obj["error_code"].toInt();
        emit OCRFailure(OCRPlatform::Baidu, errCode,
                        QString("Message: %1\nDetail: %2")
                        .arg(obj["error_msg"].toString())
                        .arg(ErrorCodeMap[errCode]));
        return;
    }

    if(m_stage == 1) {   // parse eequest
        m_requestID = obj["result"].toArray()[0].toObject()["request_id"].toString();
        QTimer::singleShot(1000, this, &BaiduTableProcessor::tableReply);
    } else if(m_stage == 2) {   // parse reply
        obj = obj["result"].toObject();
        int percentage = obj["percent"].toInt();
        qDebug() << "percentage:" << percentage;
        emit OCRSuccessful(QString("processing ... %1%").arg(percentage));
        if(percentage != 100) { // not done, wait for 1s and request again
            QTimer::singleShot(1000, this, &BaiduTableProcessor::tableReply);
        } else {
            m_stage = 0;
            parseTable(obj["result_data"].toString().toUtf8());
        }
    }
}

void BaiduTableProcessor::parseTable(const QByteArray &srcArray)
{
    QJsonDocument doc(QJsonDocument::fromJson(srcArray));
    QJsonObject obj(doc.object());
    QJsonArray forms(obj["forms"].toArray());
    QString res;
    for(auto form : forms) {
        QJsonObject formObj = form.toObject();
        if(!res.isEmpty()) res += '\n';
        res += parseOneThird(formObj["header"].toArray());
        res += parseOneThird(formObj["body"].toArray());
        res += parseOneThird(formObj["footer"].toArray());
    }
    emit OCRSuccessful(res);
}

QString BaiduTableProcessor::parseOneThird(const QJsonArray &array)
{
    QString res;
    QMap<QPair<int, int>, QString> cells;
    int row, col; QString word;
    for(auto cell : array) {
        word = parseCell(cell.toObject(), row, col);
        cells[{row, col}] = word;
        qDebug() << QString("(%1,%2): %3")
                    .arg(row)
                    .arg(col)
                    .arg(word);
    }
    // traverse all cells
    QMapIterator<QPair<int,int>, QString> it(cells);
    row = 1, col = 1;
    while(it.hasNext()) {
        it.next();
        int tr = it.key().first, tc = it.key().second;
        while(row < tr) {
            ++row; col = 0;
            res.push_back('\n');
        }
        while(col < tc) {
            ++col; res.push_back('\t');
        }
        res.push_back(it.value());
    }
    return res;
}

QString BaiduTableProcessor::parseCell(const QJsonObject &cell, int &row, int &col)
{
    row = cell["row"].toArray()[0].toInt();
    col = cell["column"].toArray()[0].toInt();
    return cell["word"].toString();
}

}
