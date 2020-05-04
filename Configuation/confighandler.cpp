#include "confighandler.h"

ConfigHandler::ConfigHandler(QObject *parent)
    : QObject(parent), settings(this)
{

}

bool ConfigHandler::copyToClipboard()
{
    return settings.value("copytoclipboard", false).toBool();
}

void ConfigHandler::setCopyToClipboard(bool checked)
{
    settings.setValue("copytoclipboard", checked);
}

int ConfigHandler::backgroundOpacity()
{
    return settings.value("backgroundopacity", 75).toInt();
}

void ConfigHandler::setBackgroundOpacity(int opacity)
{
    settings.setValue("backgroundopacity", opacity);
}

QColor ConfigHandler::UIMainColor()
{
    QColor color = settings.value("uimaincolor").value<QColor>();
    if(!color.isValid())
        color = QColor(116, 0, 150);
    return color;
}

void ConfigHandler::setUIMainColor(QColor &color)
{
    if(color.isValid())
        settings.setValue("uimaincolor", color);
}

inline qreal getColorLuma(const QColor &c) {
    return 0.30 * c.redF() + 0.59 * c.greenF() + 0.11 * c.blueF();
}

bool ConfigHandler::colorIsDark(const QColor &c)
{
    bool isWhite = false;
    if (getColorLuma(c) <= 0.60) {
        isWhite = true;
    }
    return isWhite;
}

int ConfigHandler::handlerRadius()
{
    return 4;
}

OCRPlatform ConfigHandler::defaultPlatform(OCRMode mode)
{
    return static_cast<OCRPlatform>(
                settings.value(fromMode(mode, "default"),
                               static_cast<int>(EndOfPlatform)).toInt());
}

void ConfigHandler::setDefaultPlatform(OCRMode mode, OCRPlatform platform)
{
    settings.setValue(fromMode(mode, "default"), static_cast<int>(platform));
}

QNetworkProxy::ProxyType ConfigHandler::proxyType()
{
    using PT = QNetworkProxy::ProxyType;
    return static_cast<PT>(settings.value("proxy/type",
                                          PT::NoProxy).toInt());
}

void ConfigHandler::setProxyType(QNetworkProxy::ProxyType proxy)
{
    settings.setValue("proxy/type", static_cast<int>(proxy));
}

QString ConfigHandler::proxyHost(ConfigHandler::ProxyType proxy)
{
    return settings.value("proxy/" + fromProxyType(proxy, "host")).toString();
}

void ConfigHandler::setProxyHost(ConfigHandler::ProxyType proxy, const QString &hostname)
{
    settings.setValue("proxy/" + fromProxyType(proxy, "host"), hostname);
}

int ConfigHandler::proxyPort(ConfigHandler::ProxyType proxy)
{
    return settings.value("proxy/" + fromProxyType(proxy, "port")).toInt();
}

void ConfigHandler::setProxyPort(ConfigHandler::ProxyType proxy, int port)
{
    settings.setValue("proxy/" + fromProxyType(proxy, "port"), port);
}

bool ConfigHandler::useProxy(OCRPlatform platform)
{
    return settings.value(fromPlatform(platform, "useproxy"), false).toBool();
}

void ConfigHandler::setUseProxy(OCRPlatform platform, bool checked)
{
    settings.setValue(fromPlatform(platform, "useproxy"), checked);
}

bool ConfigHandler::platformAvailable(OCRPlatform platform)
{
//    return settings.value(fromPlatform(platform, "ava"), false).toBool();
    return !APIKey1(platform).isEmpty();
}

//void ConfigHandler::setPlatformAvailability(OCRPlatform platform, bool checked)
//{
//    settings.setValue(fromPlatform(platform, "ava"), checked);
//}

bool ConfigHandler::modeAvailable(OCRPlatform platform, OCRMode mode)
{
    return settings.value(fromPlatform(platform, fromMode(mode, "ava")), false).toBool();
}

void ConfigHandler::setModeAvailability(OCRPlatform platform, OCRMode mode, bool checked)
{
    settings.setValue(fromPlatform(platform, fromMode(mode, "ava")), checked);
}

QString ConfigHandler::APIKey1(OCRPlatform platform)
{
    return settings.value(fromPlatform(platform, "key1")).toString();
}

void ConfigHandler::setAPIKey1(OCRPlatform platform, const QString &key)
{
    settings.setValue(fromPlatform(platform, "key1"), key);
}

QString ConfigHandler::APIKey2(OCRPlatform platform)
{
    return settings.value(fromPlatform(platform, "key2")).toString();
}

void ConfigHandler::setAPIKey2(OCRPlatform platform, const QString &key)
{
    settings.setValue(fromPlatform(platform, "key2"), key);
}

QString ConfigHandler::fromPlatform(OCRPlatform platform, const QString &value)
{
    QString res;
    switch(platform){
    case Baidu:
        res = "baidu"; break;
    case Tencent:
        res = "tencent"; break;
    case OCR_Space:
        res = "ocrspace"; break;
    case Google:
        res = "google"; break;
    case Mathpix:
        res = "mathpix"; break;
    case Azure:
        res = "azure"; break;
    default:
        res = "badplatform"; break;
    }

    if(!value.isEmpty())
        res += "/" + value;
    return res;
}

QString ConfigHandler::fromMode(OCRMode mode, const QString &value)
{
    QString res;
    switch(mode){
    case NormalText:
        res = "normaltext"; break;
    case Table:
        res = "table"; break;
    case Formula:
        res = "formula"; break;
    case Handwriting:
        res = "handwriting"; break;
    case WebImage:
        res = "webimage"; break;
    case QRCode:
        res = "qrcode"; break;
    case Number:
        res = "number"; break;
    default:
        res = "badmode"; break;
    }

    if(!value.isEmpty())
        res += "/" + value;
    return res;
}

QString ConfigHandler::fromProxyType(ConfigHandler::ProxyType proxy, const QString &value)
{
    QString res;
    switch (proxy) {
    case ProxyType::Socks5Proxy:
        res = "socks5"; break;
    case ProxyType::HttpProxy:
        res = "http"; break;
    default:
        return "";
    }

    if(!value.isEmpty())
        res += "/" + value;
    return res;
}

QString ConfigHandler::asPlatformName(OCRPlatform platform)
{
    switch(platform){
    case Baidu:
        return tr("Baidu");
    case Tencent:
        return tr("Tencent");
    case OCR_Space:
        return tr("OCR.Space");
    case Mathpix:
        return tr("Mathpix");
    case Google:
        return tr("Google");
    case Azure:
        return tr("Azure");
    default:
        return tr("bad platform name");
    }
}

QString ConfigHandler::asModeName(OCRMode mode)
{
    switch(mode){
    case NormalText:
        return tr("Normal Text");
    case Table:
        return tr("Table");
    case Formula:
        return tr("Formula");
    case Handwriting:
        return tr("Handwriting");
    case WebImage:
        return tr("Web Image");
    case QRCode:
        return tr("QR Code");
    case Number:
        return tr("Number");
    default:
        return tr("bad mode name");
    }
}

void ConfigHandler::setDefault()
{
    settings.clear();
}
