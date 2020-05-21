#include "API/apicommon.h"
#include "ponyocr.h"
#include "ponyocr_adaptor.h"
#include "Configuation/confighandler.h"
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTranslator>
#include <QtDBus>
#include <iostream>

void msgHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
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
    QString txt = QString("%1 %2: %3")
                  .arg(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"))
                  .arg(typeStr)
                  .arg(msg);

    // log file at application directory
    QDir dir(qApp->applicationDirPath());
    QFile outFile(dir.absoluteFilePath("log.txt"));
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
    outFile.close();
}

void criticalError(const QString &msg) {
    qCritical().noquote() << msg;                   // print to log
    std::cerr << msg.toStdString() << std::endl;    // print to screen
    exit(1);
}

const QString SERVICE_NAME("org.thebesttv.PonyOCR");

int main(int argc, char *argv[])
{
    qInstallMessageHandler(msgHandler);

    if(argc == 1) { // start main application
        QDBusConnection dbus = QDBusConnection::sessionBus();
        if (!dbus.isConnected()) {
            criticalError("Unable to connect to DBus");
        }

        QCoreApplication::setOrganizationName("thebesttv");
        QCoreApplication::setApplicationName("PonyOCR");
        // High DPI support
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

        QApplication app(argc, argv);
        app.setWindowIcon(QIcon(":/img/icon.ico"));

        QTranslator translator;
        translator.load("pony_" + ConfigHandler().language());
        app.installTranslator(&translator);

        GlobalInitAPIResources();
        PonyOCR pony;
        pony.show();

        // wait for window to render completely
        QApplication::processEvents();

        new PonyOCRAdaptor(&pony);
        if (!dbus.registerObject("/twilight", &pony)) {
            criticalError("Unable to register adaptor");
        }
        if (!dbus.registerService(SERVICE_NAME)) {
            criticalError("Unable to register service, is PonyOCR alreaty running?");
        }

        return app.exec();
    }

    QCoreApplication app(argc, argv);

    if(argc > 2 || QString(argv[1]) != "request") {
        criticalError("Usage: ponyocr [request]");
    }

    auto iface = new QDBusInterface(SERVICE_NAME,
                                    "/twilight",
                                    "org.thebesttv.PonyOCR");

    if (!iface->isValid()) {
        criticalError(qPrintable(QDBusConnection::sessionBus().lastError().message()));
    }

    iface->call("requestOCR");

    QTimer::singleShot(0, qApp, SLOT(quit()));
    return app.exec();
}
