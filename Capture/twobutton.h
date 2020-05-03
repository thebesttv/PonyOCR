#ifndef TWOBUTTON_H
#define TWOBUTTON_H

#include <QPushButton>
#include <Configuation/confighandler.h>

class PlatformButton : public QPushButton {
    Q_OBJECT
public:
    explicit PlatformButton(OCRPlatform platform, QWidget *parent = nullptr);
private:
    OCRPlatform m_platform;
protected:
    void enterEvent(QEvent *e) override;
signals:
    void buttonClicked(OCRPlatform platform);
};

class ModeButton : public QPushButton {
    Q_OBJECT
public:
    explicit ModeButton(OCRMode m_mode, QWidget *parent = nullptr);
private:
    OCRMode m_mode;
protected:
    void enterEvent(QEvent *e) override;
signals:
    void buttonClicked(OCRMode mode);
    void enterMode(OCRMode mode);
};

#endif
