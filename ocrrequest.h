#ifndef CAPTUREREQUEST_H
#define CAPTUREREQUEST_H

#include <QPixmap>
#include "Configuation/confighandler.h"

class OCRRequest
{
public:
    OCRRequest() : m_mode(EndOfMode), m_platform(EndOfPlatform) { }
    OCRRequest(OCRMode mode, OCRPlatform platform, const QPixmap &img)
        : m_mode(mode), m_platform(platform), m_img(img) { }

    OCRMode m_mode;
    OCRPlatform m_platform;
    QPixmap m_img;

    void setMode(OCRMode mode) {
        m_mode = mode;
    }
    void setPlatform(OCRPlatform platform){
        m_platform = platform;
    }
    void setImage(const QPixmap &img){
        m_img = img;
    }
};

#endif // CAPTUREREQUEST_H
