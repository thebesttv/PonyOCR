#ifndef CAPTUREBUTTON_H
#define CAPTUREBUTTON_H

#include <QWidget>
#include "twobutton.h"
#include "Configuation/confighandler.h"

class CaptureButtonHandler : public QWidget
{
    Q_OBJECT
public:
    explicit CaptureButtonHandler(QWidget *parent = nullptr);

    void setScreenRegions(const QVector<QRect> &v);
    void defaultPush();
    void drawPush(QRect selection);
    void hideAll();

private:
    ConfigHandler m_handler;
    bool m_visible;
    OCRPlatform m_platform;
    OCRMode m_mode;
    QVector<ModeButton*> m_mbArray;
    QVector<PlatformButton*> m_pbArray;
    int m_buttonWidth, m_buttonHeight;
    int m_pbX;
    bool m_reverse;
    QVector<QRect> m_regions;
    QRect m_modeRect, m_platformRect;

    void setPlatformAndPush(OCRPlatform p);
    void setModeAndPush(OCRMode m);
    void setModeAndUpdate(OCRMode m);
    void drawModeButton(int x, int y);
    bool fullyContained(const QRect &rect);

    bool modeAvailable(OCRMode mode);
    void updatePlatformButton();

signals:
    void selectionDone(OCRPlatform platform, OCRMode mode);
};

#endif // CAPTUREBUTTON_H
