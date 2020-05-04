#include "apibase.h"
#include <QByteArray>
#include <QBuffer>
#include <QPixmap>
#include <QDebug>

APIBase::APIBase(OCRPlatform platform, QNetworkAccessManager *manager, QObject *parent)
    : NetworkBase(manager, parent) , m_platform(platform)
{

}

void APIBase::getBase64Img()
{
    QByteArray imgArray;
    QBuffer buffer(&imgArray);
    m_req.m_img.save(&buffer, "png");
    m_base64str = imgArray.toBase64();
}

void APIBase::_process(const OCRRequest &request)
{
    qDebug().noquote() << QString("%1: process request (%2, %3)")
                          .arg(ConfigHandler::asPlatformName(m_platform))
                          .arg(ConfigHandler::asPlatformName(request.m_platform))
                          .arg(ConfigHandler::asModeName(request.m_mode));

    m_array.clear();

    m_req = request;
    getBase64Img();

    switch(m_req.m_mode){
    case OCRMode::NormalText:
        processNoamrlText(); break;
    case OCRMode::WebImage:
        processWebImage(); break;
    case OCRMode::Table:
        processTable(); break;
    case OCRMode::QRCode:
        processQRCode(); break;
    case OCRMode::Number:
        processNumbers(); break;
    case OCRMode::Handwriting:
        processHandwriting(); break;
    case OCRMode::Formula:
        processFormula(); break;
    default:
        break;
    }
}

void APIBase::process(const OCRRequest &request)
{
    // allow for initialization before actual processing
    _process(request);
}
