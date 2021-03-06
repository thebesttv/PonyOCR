#ifndef GENERALAPI_H
#define GENERALAPI_H

#include <QObject>
#include "ocrrequest.h"
#include "baiduapi.h"
#include "tencentapi.h"
#include "ocrspaceapi.h"
#include "mathpixapi.h"

class QNetworkAccessManager;

class GeneralAPI : public QObject
{
    Q_OBJECT
public:
    explicit GeneralAPI(QNetworkAccessManager *normalManager,
                        QNetworkAccessManager *proxiedManager,
                        QObject *parent = nullptr);
    void process(const OCRRequest &req);
    baidu::BaiduAPI &baidu();
    tencent::TencentAPI &tencent();
    ocrspace::OCRSpaceAPI &ocrspace();
    mathpix::MathpixAPI &mathpix();

private:
    baidu::BaiduAPI baiduAPI;
    tencent::TencentAPI tencentAPI;
    ocrspace::OCRSpaceAPI ocrspaceAPI;
    mathpix::MathpixAPI mathpixAPI;

signals:
    void OCRSuccessful(QString s);
    void OCRFailure(OCRPlatform platform, int errCode, QString errDescription);
    void authorizationFailure(OCRPlatform platform, QString errDescription);
};

#endif // GENERALAPI_H
