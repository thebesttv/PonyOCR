#include "proxyconfigwidget.h"
#include <QButtonGroup>
#include <QRadioButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QNetworkProxy>
#include <QNetworkAccessManager>

ProxyConfigWidget::ProxyConfigWidget(QNetworkAccessManager *proxiedManager,
                                     QWidget *parent)
    : QWidget(parent), m_proxiedManager(proxiedManager)
{
    m_noProxyButton = new QRadioButton("No Proxy");
    m_socksButton = new QRadioButton("Socks5 Proxy");
    m_httpButton = new QRadioButton("HTTP Proxy");

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(m_noProxyButton);
    group->addButton(m_socksButton);
    group->addButton(m_httpButton);

    switch (m_handler.proxyType()) {
    case QNetworkProxy::NoProxy:
        m_noProxyButton->setChecked(true); break;
    case QNetworkProxy::Socks5Proxy:
        m_socksButton->setChecked(true); break;
    case QNetworkProxy::HttpProxy:
        m_httpButton->setChecked(true); break;
    default:
        break;
    }

    m_socksHost = new QLineEdit;
    m_socksHost->setText(m_handler.proxyHost(QNetworkProxy::Socks5Proxy));
    m_socksPort = new QSpinBox;
    m_socksPort->setRange(0, 65535);
    m_socksPort->setValue(m_handler.proxyPort(QNetworkProxy::Socks5Proxy));
    QHBoxLayout *socksLayout = new QHBoxLayout;
    socksLayout->addWidget(m_socksHost);
    socksLayout->addWidget(m_socksPort);

    m_httpHost = new QLineEdit;
    m_httpHost->setText(m_handler.proxyHost(QNetworkProxy::HttpProxy));
    m_httpPort = new QSpinBox;
    m_httpPort->setRange(0, 65535);
    m_httpPort->setValue(m_handler.proxyPort(QNetworkProxy::HttpProxy));
    QHBoxLayout *httpLayout = new QHBoxLayout;
    httpLayout->addWidget(m_httpHost);
    httpLayout->addWidget(m_httpPort);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_noProxyButton);
    formLayout->addRow(m_socksButton, socksLayout);
    formLayout->addRow(m_httpButton, httpLayout);
    setLayout(formLayout);
}

void ProxyConfigWidget::saveConfig()
{
    if(m_noProxyButton->isChecked()) {
        m_handler.setProxyType(QNetworkProxy::NoProxy);
    } else if(m_socksButton->isChecked()) {
        m_handler.setProxyType(QNetworkProxy::Socks5Proxy);
    } else if(m_httpButton->isChecked()) {
        m_handler.setProxyType(QNetworkProxy::HttpProxy);
    }

    m_handler.setProxyHost(QNetworkProxy::Socks5Proxy, m_socksHost->text());
    m_handler.setProxyPort(QNetworkProxy::Socks5Proxy, m_socksPort->value());

    m_handler.setProxyHost(QNetworkProxy::HttpProxy, m_httpHost->text());
    m_handler.setProxyPort(QNetworkProxy::HttpProxy, m_httpPort->value());

    QNetworkProxy proxy;
    proxy.setType(m_handler.proxyType());
    if(proxy.type() != QNetworkProxy::NoProxy) {
        proxy.setHostName(m_handler.proxyHost(proxy.type()));
        proxy.setPort(m_handler.proxyPort(proxy.type()));
    }
    qDebug() << "here" << m_proxiedManager;
    m_proxiedManager->setProxy(proxy);
    qDebug() << "done";
}
