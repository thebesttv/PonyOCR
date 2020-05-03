#ifndef BAIDUAPI_H
#define BAIDUAPI_H

#include "apibase.h"
#include "baiduaccesstoken.h"
#include "baidutableprocessor.h"

class QPixmap;

namespace baidu {

class BaiduAPI : public APIBase {
    Q_OBJECT

public:
    BaiduAPI(QNetworkAccessManager *m_manager, QObject *parent = nullptr);

private:
    BaiduAccessToken m_tokengetter;
    BaiduTableProcessor m_tableProcessor;

    QUrl flavoredUrl(const QString &s);

    void processNoamrlText() override;
    void processWebImage() override;
    void processTable() override;
    void processQRCode() override;
    void processNumbers() override;
    void processHandwriting() override;
    void processFormula() override;
    void processBase(QString url);

private slots:
    void parse() override;
};

}

#endif // BAIDUAPI_H
