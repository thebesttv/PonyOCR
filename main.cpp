#include "API/apicommon.h"
#include "ponyocr.h"
#include "Configuation/confighandler.h"
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTranslator>

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

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("thebesttv");
    QCoreApplication::setApplicationName("PonyOCR");
    // High DPI support
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qInstallMessageHandler(msgHandler);
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/img/icon.ico"));

    QTranslator translator;
    translator.load("pony_" + ConfigHandler().language());
    app.installTranslator(&translator);

    GlobalInitAPIResources();
    PonyOCR ocr;
    ocr.show();

    return app.exec();
}
