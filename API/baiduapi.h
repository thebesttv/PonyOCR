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
    explicit BaiduAPI(QNetworkAccessManager *normalManager,
                      QNetworkAccessManager *proxiedManager,
                      QObject *parent = nullptr);

private:
    BaiduAccessToken m_tokenGetter;
    BaiduTableProcessor m_tableProcessor;

    QUrl flavoredUrl(const QString &s, const QString &at);

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
