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

BaiduTableProcessor::BaiduTableProcessor(QNetworkAccessManager *normalManager,
                                         QNetworkAccessManager *proxiedManager,
                                         QObject *parent)
    : NetworkBase(normalManager, proxiedManager, parent)
{

}

void BaiduTableProcessor::process(const QString &base64str,
                                  const QUrl &requestUrl, const QUrl &replyUrl)
{
    qDebug().noquote() << "Baidu: start table processing";
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
    if(m_handler.useProxy(OCRPlatform::Baidu))
        m_reply = m_proxiedManager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    else
        m_reply = m_normalManager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
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
    if(m_handler.useProxy(OCRPlatform::Baidu))
        m_reply = m_proxiedManager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    else
        m_reply = m_normalManager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    connectReply(m_reply);
}

void BaiduTableProcessor::parse()
{
    QJsonDocument doc(QJsonDocument::fromJson(m_array));
    QJsonObject obj(doc.object());

    if(obj.contains("error_code")){
        int errCode = obj["error_code"].toInt();
        QString errMsg = QString("Message: %1\nDetail: %2")
                         .arg(obj["error_msg"].toString())
                         .arg(ErrorCodeMap[errCode]);
        qDebug().noquote() << QString("table parse failed, error code: %1\n%2")
                              .arg(errCode).arg(errMsg);
        emit OCRFailure(OCRPlatform::Baidu, errCode, errMsg);
        return;
    }

    if(m_stage == 1) {   // parse eequest
        m_requestID = obj["result"].toArray()[0].toObject()["request_id"].toString();
        qDebug() << "get result id";
        QTimer::singleShot(2000, this, &BaiduTableProcessor::tableReply);
    } else if(m_stage == 2) {   // parse reply
        obj = obj["result"].toObject();
        int percentage = obj["percent"].toInt();
        qDebug() << "remote table processing percentage:" << percentage;
        emit OCRSuccessful(QString("processing ... %1%").arg(percentage));
        if(percentage != 100) {
            // not done, wait for some time and request again
            QTimer::singleShot(800, this, &BaiduTableProcessor::tableReply);
        } else {
            m_stage = 0;
            parseTable(obj["result_data"].toString().toUtf8());
        }
    }
}

void BaiduTableProcessor::parseTable(const QByteArray &srcArray)
{
    QJsonObject obj(QJsonDocument::fromJson(srcArray).object());
    QJsonArray forms(obj["forms"].toArray());
    QString res;
    for(auto form : forms) {
        QJsonObject formObj = form.toObject();
        if(!res.isEmpty()) res += '\n';
        QMap<QPair<int, int>, QString> cells;
        int startRow = 0, maxCol = 0;
        parseOneThird(formObj["header"].toArray(), cells, startRow, maxCol);
        parseOneThird(formObj["body"].toArray(),   cells, startRow, maxCol);
        parseOneThird(formObj["footer"].toArray(), cells, startRow, maxCol);
        if(cells.isEmpty()) continue;

        // output as markdown table format
        res += '|';
        for(int c = 1; c <= maxCol; ++c)
            res += QString(" %1 |").arg(cells[{1, c}]);
        res += "\n|";
        for(int c = 1; c <= maxCol; ++c)
            res += QString('-').repeated(qMax(cells[{1, c}].size(), 1)) + "|";
        res += "\n";

        for(int r = 2; r <= startRow; ++r) {
            res += '|';
            for(int c = 1; c <= maxCol; ++c)
                res += QString(" %1 |").arg(cells[{r, c}]);
            res += '\n';
        }
    }
    emit OCRSuccessful(res);
}

void BaiduTableProcessor::parseOneThird(const QJsonArray &array,
                                        QMap<QPair<int, int>, QString> &cells,
                                        int &startRow, int &maxCol)
{
    int row, col; QString word;
    int maxRow = startRow;
    for(auto cell : array) {
        word = parseCell(cell.toObject(), row, col);
        if(word.isEmpty()) continue;
        row += startRow;
        maxRow = qMax(maxRow, row);
        maxCol = qMax(maxCol, col);
        cells[{row, col}] = word;
//        qDebug().noquote() << QString("table cell(%1,%2): %3")
//                              .arg(row).arg(col).arg(word);
    }
    startRow = maxRow;
}

QString BaiduTableProcessor::parseCell(const QJsonObject &cell, int &row, int &col)
{
    row = cell["row"].toArray()[0].toInt();
    col = cell["column"].toArray()[0].toInt();
    return cell["word"].toString();
}

}
