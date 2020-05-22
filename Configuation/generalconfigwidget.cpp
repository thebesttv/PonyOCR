#include "generalconfigwidget.h"
#include "configdialog.h"
#include "apiconfigwidget.h"
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QtDebug>

GeneralConfigWidget::GeneralConfigWidget(QWidget *parent) : QWidget(parent)
{
    // ui color
    m_uiColor = m_handler.UIMainColor();
    m_colorLabel = new QLabel(tr("Notification Box Color:"));
    m_colorPushButton = new QPushButton;
    m_colorPushButton->setStyleSheet(QString("background-color: %1")
                                     .arg(m_uiColor.name()));
    connect(m_colorPushButton, &QPushButton::clicked,
            this, &GeneralConfigWidget::setColor);
    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addWidget(m_colorLabel);
    colorLayout->addWidget(m_colorPushButton);

    // opacity
    QGridLayout *opacityLayout = new QGridLayout;
    m_opacityLabel = new QLabel(tr("Capture window opacity:"));
    m_opacitySpinBox = new QSpinBox;
    m_opacitySpinBox->setRange(0,100);
    m_opacitySpinBox->setSuffix("%");
    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setRange(0,100);
    connect(m_opacitySlider, &QSlider::valueChanged,
            m_opacitySpinBox, &QSpinBox::setValue);
    connect(m_opacitySpinBox, SIGNAL(valueChanged(int)),
            m_opacitySlider, SLOT(setValue(int)));
    m_opacitySlider->setValue(m_handler.backgroundOpacity());
    opacityLayout->addWidget(m_opacityLabel, 1, 1);
    opacityLayout->addWidget(m_opacitySpinBox, 1, 2);
    opacityLayout->addWidget(m_opacitySlider, 2, 1, 1, 2);

    m_clipboardCheckBox = new QCheckBox(tr("Copy OCR result to clipboard"));
    m_clipboardCheckBox->setChecked(m_handler.copyToClipboard());

    m_languageCombo = new QComboBox;
    m_languageCombo->addItem("English", "en");
    m_languageCombo->addItem("简体中文", "zh");
    m_languageCombo->setCurrentIndex(m_handler.language() == QString("en") ? 0 : 1);
    QLabel *languageLabel = new QLabel(tr("Language (need to restart program):"));
    QFormLayout *languageLayout = new QFormLayout;
    languageLayout->addRow(languageLabel, m_languageCombo);

    // mode default platform combo
    m_comboLayout = new QFormLayout;
    QLabel *defaultLabel = new QLabel(tr("Default platform for mode:"));
    m_comboLayout->addRow(defaultLabel);
    m_modePlatforms.resize(EndOfMode);
    for(int m = 0; m < EndOfMode; ++m) {
        ModeComboBox *combo = new ModeComboBox(static_cast<OCRMode>(m));
        m_modeList.push_back(combo);
        m_comboLayout->addRow(combo->label(), combo);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(colorLayout);
    mainLayout->addLayout(opacityLayout);
    mainLayout->addWidget(m_clipboardCheckBox);
    mainLayout->addLayout(languageLayout);
    mainLayout->addLayout(m_comboLayout);
    mainLayout->addStretch();
    setLayout(mainLayout);
    setMinimumSize(sizeHint());
}

void GeneralConfigWidget::saveConfig()
{
    paintEvent(nullptr);    // force update all combos

    m_handler.setUIMainColor(m_uiColor);
    m_handler.setBackgroundOpacity(m_opacitySpinBox->value());
    m_handler.setCopyToClipboard(m_clipboardCheckBox->isChecked());

    for(int m = 0; m < EndOfMode; ++m){
        if(m_modeList[m]->currentIndex() == -1)
            continue;
        OCRMode mode = static_cast<OCRMode>(m);
        OCRPlatform platform = static_cast<OCRPlatform>(
                    m_modeList[m]->currentData().toInt());
        m_handler.setDefaultPlatform(mode, platform);
    }

    m_handler.setLanguage(m_languageCombo->currentData().toString());
}

void GeneralConfigWidget::addAPI(APIConfigWidget *widget)
{
    m_apiList.append(widget);
}

void GeneralConfigWidget::setColor()
{
    QColor newColor = QColorDialog::getColor();
    if(!newColor.isValid())
        return;
    m_uiColor = newColor;
    m_colorPushButton->setStyleSheet(QString("background-color: %1")
                                     .arg(m_uiColor.name()));
}

void GeneralConfigWidget::updateComboBox()
{
//    qDebug().noquote() << "update comboBox";
    for(int m = 0; m < EndOfMode; ++m) {
        OCRMode mode = static_cast<OCRMode>(m);
        m_modeList[m]->hide();
        m_comboLayout->removeWidget(m_modeList[m]->label());
        m_comboLayout->removeWidget(m_modeList[m]);
        // remove all previous items
        int c = m_modeList[m]->count();
        for(int i = c-1; i >= 0; --i) {
            m_modeList[m]->removeItem(i);
        }
//        qDebug().noquote() << QString("delete %1 items from mode %2, change to %3")
//                              .arg(c)
//                              .arg(ConfigHandler::asModeName(static_cast<OCRMode>(m)))
//                              .arg(modePlatforms[m].size());

        // if has at least one new item
        if (!m_modePlatforms[m].isEmpty()) {
            for(auto p : m_modePlatforms[m]) {
                OCRPlatform platform = static_cast<OCRPlatform>(p);
                m_modeList[m]->addItem(ConfigHandler::asPlatformName(platform), p);
                if(platform == m_handler.defaultPlatform(mode)) {
                    // if match default from settings
                    m_modeList[m]->setCurrentIndex(m_modeList[m]->count()-1);
                }
            }
        }
    }

    for(int m = 0; m < EndOfMode; ++m) if(!m_modePlatforms[m].isEmpty()) {
        m_comboLayout->addRow(m_modeList[m]->label(), m_modeList[m]);
        m_modeList[m]->show();
    }
}

void GeneralConfigWidget::paintEvent(QPaintEvent *event)
{
    // ensure that **all** api widgets are configured before this is called
    decltype (m_modePlatforms) newModePlatforms(EndOfMode);
    for(auto api : m_apiList) if(api->available()) {
        int p = static_cast<int>(api->platform());
        for(int m = 0; m < EndOfMode; ++m)
            if (api->modeChecked(static_cast<OCRMode>(m))){
                newModePlatforms[m].push_back(p);
            }
    }
    if(newModePlatforms != m_modePlatforms) {
        m_modePlatforms = newModePlatforms;
        updateComboBox();
    }
    QWidget::paintEvent(event);
}
