#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include <QObject>
#include <QSettings>
#include <QColor>

enum OCRPlatform {
    Baidu,
    Tencent,
    OCR_Space,
    Mathpix,
    Google,
    Azure,
    EndOfPlatform
};

enum OCRMode {
    NormalText,
    Table,
    Formula,
    Handwriting,
    WebImage,
    QRCode,
    Number,
    EndOfMode
};

class ConfigHandler : public QObject
{
    Q_OBJECT
public:
    explicit ConfigHandler(QObject *parent = nullptr);

    bool copyToClipboard();
    void setCopyToClipboard(bool checked);

    int backgroundOpacity();
    void setBackgroundOpacity(int opacity);

    QColor UIMainColor();
    void setUIMainColor(QColor &color);

    bool colorIsDark(const QColor &c);

    int handlerRadius();

    OCRPlatform defaultPlatform(OCRMode mode);
    void setDefaultPlatform(OCRMode mode, OCRPlatform platform);

    // platform is considered available if key1 is not empty
    bool platformAvailable(OCRPlatform platform);
//    void setPlatformAvailability(OCRPlatform platform, bool checked);

    bool modeAvailable(OCRPlatform platform, OCRMode mode);
    void setModeAvailability(OCRPlatform platform, OCRMode mode, bool checked);

    QString APIKey1(OCRPlatform platform);
    void setAPIKey1(OCRPlatform platform, const QString &key);

    QString APIKey2(OCRPlatform platform);
    void setAPIKey2(OCRPlatform platform, const QString &key);

    void setDefault();

    // for settings
    static QString fromPlatform(OCRPlatform platform, const QString &value = "");
    static QString fromMode(OCRMode mode, const QString &value = "");

    // for display
    static QString asPlatformName(OCRPlatform platform);
    static QString asModeName(OCRMode mode);

private:
    QSettings settings;

signals:

};

#endif // CONFIGHANDLER_H
