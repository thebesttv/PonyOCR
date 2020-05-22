#include "apiconfigwidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QFrame>

APIConfigWidget::APIConfigWidget(OCRPlatform platform,
                                 const QString &kay1name, const QString &key2name,
                                 const QString &description, QWidget *parent)
    : QWidget(parent), m_platform(platform)
    , m_checkBoxes(EndOfMode), m_boxCnt(0)
{
    QFormLayout *keyLayout = new QFormLayout;
    m_label1 = new QLabel(kay1name);
    m_lineEdit1 = new QLineEdit(m_handler.APIKey1(m_platform));
    keyLayout->addRow(m_label1, m_lineEdit1);

    m_label2 = new QLabel(key2name);
    m_lineEdit2 = new QLineEdit(m_handler.APIKey2(m_platform));
    if(key2name != "disabled") {
        keyLayout->addRow(m_label2, m_lineEdit2);
    }

    QGroupBox *modeGroup = new QGroupBox(tr("Mode"));
    m_grid = new QGridLayout;
    modeGroup->setLayout(m_grid);

    m_proxyCheckBox = new QCheckBox(tr("Use Proxy"));
    m_proxyCheckBox->setChecked(m_handler.useProxy(m_platform));

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background-color: lightgray;");

    m_descriptionLabel = new QLabel(description);
    m_descriptionLabel->setWordWrap(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(keyLayout);
    mainLayout->addWidget(modeGroup);
    mainLayout->addWidget(m_proxyCheckBox);
    mainLayout->addWidget(separator);
    mainLayout->addWidget(m_descriptionLabel);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

QString APIConfigWidget::key1()
{
    return m_lineEdit1->text();
}

QString APIConfigWidget::key2()
{
    return m_lineEdit2->text();
}

OCRPlatform APIConfigWidget::platform()
{
    return m_platform;
}

void APIConfigWidget::addMode(OCRMode mode)
{
    if(m_checkBoxes[mode])
        return;
    QCheckBox *checkBox = new QCheckBox(ConfigHandler::asModeName(mode));
    checkBox->setChecked(m_handler.modeAvailable(m_platform, mode));
    m_checkBoxes[mode] = checkBox;
    ++m_boxCnt;
    int y = (m_boxCnt - 1)%3 + 1, x = (m_boxCnt - 1)/3 + 1;
    m_grid->addWidget(checkBox, x, y);
}

bool APIConfigWidget::modeChecked(OCRMode mode)
{
    if(!m_checkBoxes[mode])
        return false;
    return m_checkBoxes[mode]->isChecked();
}

void APIConfigWidget::saveConfig()
{
    QString m_key1 = m_lineEdit1->text(),
            m_key2 = m_lineEdit2->text();
    m_handler.setAPIKey1(m_platform, m_key1);
    m_handler.setAPIKey2(m_platform, m_key2);

    for(int m = 0; m < EndOfMode; ++m) if(m_checkBoxes[m]) {
        m_handler.setModeAvailability(m_platform, static_cast<OCRMode>(m),
                                      m_checkBoxes[m]->isChecked());
    }

    m_handler.setUseProxy(m_platform, m_proxyCheckBox->isChecked());
}

bool APIConfigWidget::available()
{
    return !m_lineEdit1->text().isEmpty();
}
