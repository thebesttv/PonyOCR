#ifndef APICONFIGWIDGET_H
#define APICONFIGWIDGET_H

#include <QWidget>
#include <QSettings>
#include <QTabWidget>
#include "confighandler.h"

class QLabel;
class QLineEdit;
class QCheckBox;
class QGridLayout;

class APIConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit APIConfigWidget(OCRPlatform platform,
                             const QString &kay1name, const QString &key2name,
                             const QString &description, QWidget *parent = nullptr);
    QString key1();
    QString key2();
    OCRPlatform platform();
    void addMode(OCRMode mode);
    bool available();
    bool modeChecked(OCRMode mode);
    void saveConfig();

private:
    OCRPlatform m_platform;
    ConfigHandler m_handler;

    QLineEdit *m_lineEdit1,
              *m_lineEdit2;
    QLabel *m_label1,
           *m_label2;

    QVector<QCheckBox*> m_checkBoxes;
    int m_boxCnt;
    QGridLayout *m_grid;

    QCheckBox *m_proxyCheckBox;

    QLabel *m_descriptionLabel;
};


#endif // APICONFIGWIDGET_H
