#include "screengrabber.h"
#include <QPixmap>
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
#include <QDir>
#include <QRandomGenerator>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#else
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#endif
#include <QDebug>

QPixmap ScreenGrabber::grabEntireDesktop()
{
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    //https://gitlab.gnome.org/GNOME/gnome-shell/blob/master/data/dbus-interfaces/org.gnome.Shell.Screenshot.xml
    QDBusInterface gnomeInterface("org.gnome.Shell", "/org/gnome/Shell/Screenshot",
                                  "org.gnome.Shell.Screenshot");
    QString path = QDir::temp().absoluteFilePath(QString("FunOCR_%1.png")
                                                 .arg(QRandomGenerator::global()->generate()));
    QDBusReply<bool> reply = gnomeInterface.call("Screenshot", false, false, path);

    QPixmap img;
    if(!reply){
        qDebug().noquote() << "Screenshot failed";
    }else{
        img.load(path);
        qDebug().noquote() << QString("get entire desktop of size (%1, %2)")
                              .arg(img.size().width())
                              .arg(img.size().height());
        QFile imgFile(path);
        imgFile.remove();
    }
    return img;
#else
    QRect geometry;
    for (QScreen *const screen : QGuiApplication::screens()) {
        geometry = geometry.united(screen->geometry());
    }

    QPixmap img(QApplication::primaryScreen()->grabWindow(
                  QApplication::desktop()->winId(),
                  geometry.x(),
                  geometry.y(),
                  geometry.width(),
                  geometry.height())
              );
    auto screenNumber = QApplication::desktop()->screenNumber();
    QScreen *screen = QApplication::screens()[screenNumber];
    img.setDevicePixelRatio(screen->devicePixelRatio());
    qDebug().noquote() << QString("get entire desktop of size (%1, %2)")
                          .arg(img.size().width())
                          .arg(img.size().height());
    return img;
#endif
}
