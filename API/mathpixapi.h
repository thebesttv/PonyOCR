#ifndef MATHPIXAPI_H
#define MATHPIXAPI_H

#include "apibase.h"

namespace mathpix {

class MathpixAPI : public APIBase
{
    Q_OBJECT
public:
    explicit MathpixAPI(QNetworkAccessManager *normalManager,
                        QNetworkAccessManager *proxiedManager,
                        QObject *parent = nullptr);

private:
    void setHeader(QNetworkRequest &request) override;

    void processFormula() override;

private slots:
    void parse() override;
};

}

#endif // MATHPIXAPI_H
