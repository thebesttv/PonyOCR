#ifndef TENCENTAPI_H
#define TENCENTAPI_H

#include "apibase.h"

namespace tencent {

class TencentAPI : public APIBase
{
    Q_OBJECT
public:
    TencentAPI(QNetworkAccessManager *manager, QObject *parent = nullptr);
    void process(const OCRRequest &req) override;

private:
    QString m_AppID;
    QString m_AppKey;

    QString getSigniture(const QString &s);

    void processNoamrlText() override;
    void processHandwriting() override;
    void processBase(QString url);

private slots:
    void parse() override;
};

}

#endif // TENCENTAPI_H
