#ifndef BAIDUACCESSTOKEN_H
#define BAIDUACCESSTOKEN_H

#include "networkbase.h"

namespace baidu {

class BaiduAccessToken : public NetworkBase {
    Q_OBJECT

public:
    explicit BaiduAccessToken(QNetworkAccessManager *m_manager, QObject *parent = nullptr);
    QString getAccessToken();

private:
    QString m_apiKey, m_secretKey;
    QString m_oldak, m_oldsk;
    QString m_accessToken;
    int m_expiredTime;

private slots:
    void readyRead();
    void parse() override;

signals:
    void authorizationFailure(OCRPlatform platform, QString errDescription);
};

}

#endif // BAIDUACCESSTOKEN_H