#ifndef BAIDUTABLEPROCESSOR_H
#define BAIDUTABLEPROCESSOR_H

#include <QObject>
#include <QUrl>
#include "Configuation/confighandler.h"
#include "networkbase.h"

namespace baidu {

class BaiduTableProcessor : public NetworkBase {
    Q_OBJECT
public:
    explicit BaiduTableProcessor(QNetworkAccessManager *normalManager,
                                 QNetworkAccessManager *proxiedManager,
                                 QObject *parent = nullptr);
    void process(const QString &base64str, const QUrl &requestUrl, const QUrl &replyUrl);

private:
    ConfigHandler m_handler;
    QString m_base64str;
    QUrl m_requestUrl, m_replyUrl;
    QString m_requestID;
    int m_stage;

    void tableRequest();
    void tableReply();

private slots:
    void parse() override;
    void parseTable(const QByteArray &srcArray);
    void parseOneThird(const QJsonArray &array,
                       QMap<QPair<int, int>, QString> &cells,
                       int &startRow, int &maxCol);
    QString parseCell(const QJsonObject &cell, int &row, int &col);

signals:
    void OCRSuccessful(QString res);
    void OCRFailure(OCRPlatform platform, int errCode, QString errDescription);
};

}

#endif // BAIDUTABLEPROCESSOR_H
