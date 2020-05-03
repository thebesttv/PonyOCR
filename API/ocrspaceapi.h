#ifndef OCRSPACEAPI_H
#define OCRSPACEAPI_H

#include "apibase.h"

namespace ocrspace {

class OCRSpaceAPI : public APIBase
{
    Q_OBJECT
public:
    explicit OCRSpaceAPI(QNetworkAccessManager *manager, QObject *parent = nullptr);

private:
    void setHeader(QNetworkRequest &request) override;

    void processNoamrlText() override;
    void parse() override;
    QString parseSingleResult(const QJsonObject &source);

signals:

};

}

#endif // OCRSPACEAPI_H
