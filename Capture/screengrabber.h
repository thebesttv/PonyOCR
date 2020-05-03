#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QObject>

class ScreenGrabber : public QObject
{
    Q_OBJECT
public:
    ScreenGrabber() { }
    ~ScreenGrabber() { }
    static QPixmap grabEntireDesktop();
private:
};
#endif // SCREENSHOT_H
