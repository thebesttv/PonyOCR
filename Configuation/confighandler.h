#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QNetworkProxy>

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

    bool previewVisible();
    void setPreviewVisible(bool checked);

    int handlerRadius();

    OCRPlatform defaultPlatform(OCRMode mode);
    void setDefaultPlatform(OCRMode mode, OCRPlatform platform);

    using ProxyType = QNetworkProxy::ProxyType;
    ProxyType proxyType();
    void setProxyType(ProxyType proxyType);

    QString proxyHost(ProxyType proxy);
    void setProxyHost(ProxyType proxy, const QString &hostname);

    int proxyPort(ProxyType proxy);
    void setProxyPort(ProxyType proxy, int port);

    bool useProxy(OCRPlatform platform);
    void setUseProxy(OCRPlatform platform, bool checked);

    // platform is considered available if key1 is not empty
    bool platformAvailable(OCRPlatform platform);

    bool modeAvailable(OCRPlatform platform, OCRMode mode);
    void setModeAvailability(OCRPlatform platform, OCRMode mode, bool checked);

    QString APIKey1(OCRPlatform platform);
    void setAPIKey1(OCRPlatform platform, const QString &key);

    QString APIKey2(OCRPlatform platform);
    void setAPIKey2(OCRPlatform platform, const QString &key);

    QString language() const;
    void setLanguage(const QString &lang);

    void setDefault();

    // for settings
    static QString fromPlatform(OCRPlatform platform, const QString &value = "");
    static QString fromMode(OCRMode mode, const QString &value = "");
    static QString fromProxyType(ProxyType proxy, const QString &value = "");

    // for display
    static QString asPlatformName(OCRPlatform platform);
    static QString asModeName(OCRMode mode);

private:
    QSettings settings;

signals:

};

#endif // CONFIGHANDLER_H
