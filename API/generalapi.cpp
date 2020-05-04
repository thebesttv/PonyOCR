#include "generalapi.h"

#include <QtDebug>

GeneralAPI::GeneralAPI(QNetworkAccessManager *normalManager,
                       QNetworkAccessManager *proxiedManager,
                       QObject *parent)
    : QObject(parent)
    , baiduAPI(normalManager, proxiedManager, this)
    , tencentAPI(normalManager, proxiedManager, this)
    , ocrspaceAPI(normalManager, proxiedManager, this)
    , mathpixAPI(normalManager, proxiedManager, this)
{
    connect(&baiduAPI, &baidu::BaiduAPI::OCRSuccessful,
            this, &GeneralAPI::OCRSuccessful);

    connect(&tencentAPI, &tencent::TencentAPI::OCRSuccessful,
            this, &GeneralAPI::OCRSuccessful);

    connect(&ocrspaceAPI, &ocrspace::OCRSpaceAPI::OCRSuccessful,
            this, &GeneralAPI::OCRSuccessful);

    connect(&mathpixAPI, &mathpix::MathpixAPI::OCRSuccessful,
            this, &GeneralAPI::OCRSuccessful);
}

void GeneralAPI::process(const OCRRequest &req)
{
    qDebug() << QString("GeneralAPI: process request (%1, %2)")
                .arg(ConfigHandler::asPlatformName(req.m_platform))
                .arg(ConfigHandler::asModeName(req.m_mode));
    switch(req.m_platform){
    case OCRPlatform::Baidu:
        baiduAPI.process(req); break;
    case OCRPlatform::Tencent:
        tencentAPI.process(req); break;
    case OCRPlatform::OCR_Space:
        ocrspaceAPI.process(req); break;
    case OCRPlatform::Mathpix:
        mathpixAPI.process(req); break;
    default:
        break;
    }
}

baidu::BaiduAPI &GeneralAPI::baidu()
{
    return baiduAPI;
}

tencent::TencentAPI &GeneralAPI::tencent()
{
    return tencentAPI;
}

ocrspace::OCRSpaceAPI &GeneralAPI::ocrspace()
{
    return ocrspaceAPI;
}

mathpix::MathpixAPI &GeneralAPI::mathpix()
{
    return mathpixAPI;
}
