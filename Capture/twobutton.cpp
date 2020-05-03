#include "twobutton.h"

PlatformButton::PlatformButton(OCRPlatform platform, QWidget *parent)
    : QPushButton(ConfigHandler::asPlatformName(platform), parent)
    , m_platform(platform)
{
    connect(this, &QPushButton::clicked, [this](){
        emit buttonClicked(this->m_platform);
    });
}

void PlatformButton::enterEvent(QEvent *e)
{
    setCursor(Qt::ArrowCursor);
    QPushButton::enterEvent(e);
}

ModeButton::ModeButton(OCRMode mode, QWidget *parent)
    : QPushButton(ConfigHandler::asModeName(mode), parent)
    , m_mode(mode)
{
    setMouseTracking(true);
    connect(this, &QPushButton::clicked, [this](){
        emit buttonClicked(this->m_mode);
    });
}

void ModeButton::enterEvent(QEvent *e)
{
    emit enterMode(m_mode);
    setCursor(Qt::ArrowCursor);
    QPushButton::enterEvent(e);
}
