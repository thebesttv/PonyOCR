#ifndef OCRSPACEAPI_H
#define OCRSPACEAPI_H

#include "apibase.h"

namespace ocrspace {

class OCRSpaceAPI : public APIBase
{
    Q_OBJECT
public:
    explicit OCRSpaceAPI(QNetworkAccessManager *normalManager,
                         QNetworkAccessManager *proxiedManager,
                         QObject *parent = nullptr);

private:
    void setHeader(QNetworkRequest &request) override;

    void processBase(bool isTable, int engine);
    void processNoamrlText() override;
    void processTable() override;
    void parse() override;
    QString parseSingleResult(const QJsonObject &source);

signals:

};

}

#endif // OCRSPACEAPI_H
