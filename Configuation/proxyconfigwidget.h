#ifndef PROXYCONFIGWIDGET_H
#define PROXYCONFIGWIDGET_H

#include <QWidget>
#include "confighandler.h"

class QLineEdit;
class QRadioButton;
class QSpinBox;
class QNetworkAccessManager;

class ProxyConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProxyConfigWidget(QNetworkAccessManager *proxiedManager,
                               QWidget *parent = nullptr);
    void saveConfig();

private:
    ConfigHandler m_handler;
    QNetworkAccessManager *m_proxiedManager;
    QRadioButton *m_noProxyButton;
    QRadioButton *m_socksButton;
    QRadioButton *m_httpButton;

    QLineEdit *m_socksHost;
    QSpinBox *m_socksPort;

    QLineEdit *m_httpHost;
    QSpinBox *m_httpPort;
};

#endif // PROXYCONFIGWIDGET_H
