#include "baiduaccesstoken.h"
#include "apicommon.h"
#include "Configuation/confighandler.h"
#include <QUrlQuery>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>

namespace baidu {

BaiduAccessToken::BaiduAccessToken(QNetworkAccessManager *normalManager,
                                   QNetworkAccessManager *proxiedManager,
                                   QObject *parent)
    : NetworkBase(normalManager, proxiedManager, parent), m_expiredTime(-1)
{

}

QString BaiduAccessToken::getAccessToken()
{
    m_apiKey = m_handler.APIKey1(OCRPlatform::Baidu);
    m_secretKey = m_handler.APIKey2(OCRPlatform::Baidu);
    // if key changed, force refresh
    if(m_apiKey != m_oldak || m_secretKey != m_oldsk) {
        m_oldak = m_apiKey, m_oldsk = m_secretKey;
        m_expiredTime = -1;
    }

    // key not changed, and not expired
    if(!m_accessToken.isEmpty() && time(nullptr) + 24 * 60 * 60 < m_expiredTime){
        return m_accessToken;
    }

    QUrl url(ACCESS_TOKEN);
    QUrlQuery query;
    query.addQueryItem("grant_type", "client_credentials");
    query.addQueryItem("client_id", m_apiKey);
    query.addQueryItem("client_secret", m_secretKey);
    url.setQuery(query);

    m_array.clear();
    if(m_handler.useProxy(OCRPlatform::Baidu))
        m_reply = m_proxiedManager->get(QNetworkRequest(url));
    else
        m_reply = m_normalManager->get(QNetworkRequest(url));
    QEventLoop loop;
    connect(m_reply, &QNetworkReply::readyRead,
            this, &BaiduAccessToken::readyRead);
    connect(m_reply, &QNetworkReply::finished,
            &loop, &QEventLoop::quit);
    loop.exec();    // block until request finished

    parse();
    return m_accessToken;
}

void BaiduAccessToken::parse()
{
    m_reply->deleteLater();

    QJsonObject obj = QJsonDocument::fromJson(m_array).object();
    if(obj.contains("error")){
        emit authorizationFailure(OCRPlatform::Baidu,
                                  obj["error_description"].toString());
        m_accessToken = "";
    } else {
        m_accessToken = obj["access_token"].toString();
    }
}

}
