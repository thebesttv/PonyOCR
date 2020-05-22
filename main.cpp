#include "API/apicommon.h"
#include "Configuation/confighandler.h"
#include "ponyocr.h"
#include <QApplication>
#include <QTranslator>
#include <QtDebug>
#include <QDir>
#include <QFile>
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
#include "ponyocr_adaptor.h"
#include <QtDBus>
#include <syslog.h>
#else
#include <QDateTime>
#include <QTextStream>
#endif

void msgHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    // for GNOME, use syslog
    int errLevel;
    switch(type) {
//    qDebug() is used for writing custom debug output.
//    qInfo() is used for informational messages.
//    qWarning() is used to report warnings and *recoverable* errors in your application.
//    qCritical() is used for writing *critical* error messages and reporting system errors.
//    qFatal() is used for writing fatal error messages *shortly before exiting*.
    case QtDebugMsg:
        errLevel = LOG_DEBUG; break;
    case QtInfoMsg:
        errLevel = LOG_INFO; break;
    case QtWarningMsg:
        errLevel = LOG_WARNING; break;
    case QtCriticalMsg:
        errLevel = LOG_ERR; break;
    case QtFatalMsg:
        errLevel = LOG_CRIT; break;
    }

    syslog(errLevel, "%s", msg.toStdString().c_str());

#else
    // for windows, save log to application folder
    QString typeStr;
    switch (type) {
    case QtDebugMsg:
        typeStr = "Debug"; break;
    case QtInfoMsg:
        typeStr = "Info"; break;
    case QtWarningMsg:
        typeStr = "Warning"; break;
    case QtCriticalMsg:
        typeStr = "Critical"; break;
    case QtFatalMsg:
        typeStr = "Fatal"; break;
    }

    QString txt = QString("%1 %2: %3\r\n")  // windows way of line return
                  .arg(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"))
                  .arg(typeStr)
                  .arg(msg);

    // log file at application directory
    QDir dir(qApp->applicationDirPath());
    QFile outFile(dir.absoluteFilePath("PonyLog.txt"));
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt;
    outFile.close();
#endif
}

void fatalQuit(const char *msg) {
    qFatal("%s", msg);
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    closelog(); // close log before exit
#endif
    exit(1);
}

const QString SERVICE_NAME("org.thebesttv.PonyOCR");

int main(int argc, char *argv[])
{
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    // for GNOME, use (r)syslog, with facility LOCAL2
    openlog("PonyOCR", LOG_PERROR, LOG_LOCAL2);
#endif
    qInstallMessageHandler(msgHandler);

    if(argc == 1) { // start main application
        QCoreApplication::setOrganizationName("thebesttv");
        QCoreApplication::setApplicationName("PonyOCR");
        // High DPI support
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

        QApplication app(argc, argv);
        app.setWindowIcon(QIcon(":/img/icon.ico"));

        QString language = ConfigHandler().language();
        qDebug().noquote() << "language:" << language;
        QTranslator translator;
        translator.load("pony_" + language, qApp->applicationDirPath());
        app.installTranslator(&translator);

        GlobalInitAPIResources();
        PonyOCR pony;
        pony.show();
        // wait for window to render completely
        QApplication::processEvents();

#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
        QDBusConnection dbus = QDBusConnection::sessionBus();
        if (!dbus.isConnected()) {
            fatalQuit("Unable to connect to DBus");
        }

        new PonyOCRAdaptor(&pony);
        if (!dbus.registerObject("/twilight", &pony)) {
            fatalQuit("Unable to register adaptor");
        }
        if (!dbus.registerService(SERVICE_NAME)) {
            fatalQuit("Unable to register service, is PonyOCR alreaty running?");
        }

        QObject::connect(qApp, &QApplication::aboutToQuit, [](){
            closelog();
        });
#endif
        return app.exec();
    }

#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    // CLI processing only for Linux
    QCoreApplication app(argc, argv);

    if(argc > 2 || QString(argv[1]) != "request") {
        fatalQuit("Usage: ponyocr [request]");
    }

    auto iface = new QDBusInterface(SERVICE_NAME,
                                    "/twilight",
                                    "org.thebesttv.PonyOCR");

    if (!iface->isValid()) {
        fatalQuit(qPrintable(QDBusConnection::sessionBus().lastError().message()));
    }

    iface->call("requestOCR");

    QObject::connect(qApp, &QApplication::aboutToQuit, [](){
        closelog();
    });
    QTimer::singleShot(0, qApp, SLOT(quit()));
    return app.exec();
#endif
}
