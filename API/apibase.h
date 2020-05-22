#ifndef APIBASE_H
#define APIBASE_H

#include "networkbase.h"
#include "ocrrequest.h"

class APIBase : public NetworkBase
{
    Q_OBJECT
public:
    explicit APIBase(OCRPlatform platform,
                     QNetworkAccessManager *normalManager,
                     QNetworkAccessManager *proxiedManager,
                     QObject *parent = nullptr);
    virtual void process(const OCRRequest &request);

protected:
    OCRPlatform m_platform;

    OCRRequest m_req;
    QString m_base64str;

    void getBase64Img();

    void _process(const OCRRequest &request);
    virtual void processNoamrlText() { }
    virtual void processHandwriting() { }
    virtual void processQRCode() { }
    virtual void processWebImage(){ }
    virtual void processTable() { }
    virtual void processNumbers() { }
    virtual void processFormula() { }

signals:
    void OCRSuccessful(QString res);
    void OCRFailure(OCRPlatform platform, int errCode, QString errMsg);
    void authorizationFailure(OCRPlatform platform, QString errMsg);
};

#endif // APIBASE_H
