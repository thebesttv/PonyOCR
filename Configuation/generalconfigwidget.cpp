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

ModeComboBox::ModeComboBox(OCRMode mode, QWidget *parent)
    : QComboBox(parent), m_mode(mode)
{
    m_label = new QLabel(ConfigHandler::asModeName(mode));
    connect(this, SIGNAL(currentIndexChanged(int)),
            this, SLOT(platformChanged(int)));
}

QLabel *ModeComboBox::label()
{
    return m_label;
}

void ModeComboBox::hide()
{
    m_label->hide();
    QComboBox::hide();
}

void ModeComboBox::show()
{
    m_label->show();
    QComboBox::show();
}

void ModeComboBox::platformChanged()
{
    emit setDefaultPlatform(m_mode, static_cast<OCRPlatform>(currentData().toInt()));
}

GeneralConfigWidget::GeneralConfigWidget(QWidget *parent) : QWidget(parent)
{
    // ui color
    uiColor = handler.UIMainColor();
    colorLabel = new QLabel(tr("Notification Box Color:"));
    colorPushButton = new QPushButton;
    colorPushButton->setStyleSheet(QString("background-color: %1")
                                   .arg(uiColor.name()));
    connect(colorPushButton, &QPushButton::clicked,
            this, &GeneralConfigWidget::setColor);
    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(colorPushButton);

    // opacity
    QGridLayout *opacityLayout = new QGridLayout;
    opacityLabel = new QLabel(tr("Capture window opacity:"));
    opacitySpinBox = new QSpinBox;
    opacitySpinBox->setRange(0,100);
    opacitySpinBox->setSuffix("%");
    opacitySlider = new QSlider(Qt::Horizontal);
    opacitySlider->setRange(0,100);
    connect(opacitySlider, &QSlider::valueChanged,
            opacitySpinBox, &QSpinBox::setValue);
    connect(opacitySpinBox, SIGNAL(valueChanged(int)),
            opacitySlider, SLOT(setValue(int)));
    opacitySlider->setValue(handler.backgroundOpacity());
    opacityLayout->addWidget(opacityLabel, 1, 1);
    opacityLayout->addWidget(opacitySpinBox, 1, 2);
    opacityLayout->addWidget(opacitySlider, 2, 1, 1, 2);

    clipboardCheckBox = new QCheckBox(tr("Copy OCR result to clipboard"));
    clipboardCheckBox->setChecked(handler.copyToClipboard());

    // mode default platform combo
    comboLayout = new QFormLayout;
    QLabel *defaultLabel = new QLabel(tr("Default platform for mode:"));
    comboLayout->addRow(defaultLabel);
    modePlatforms.resize(EndOfMode);
    for(int m = 0; m < EndOfMode; ++m) {
        ModeComboBox *combo = new ModeComboBox(static_cast<OCRMode>(m));
        modeList.push_back(combo);
        comboLayout->addRow(combo->label(), combo);
        combo->show();
    }
    setMinimumSize(sizeHint());

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(colorLayout);
    mainLayout->addLayout(opacityLayout);
    mainLayout->addWidget(clipboardCheckBox);
    mainLayout->addLayout(comboLayout);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

void GeneralConfigWidget::saveConfig()
{
    paintEvent(nullptr);    // force update all combos

    handler.setUIMainColor(uiColor);
    handler.setBackgroundOpacity(opacitySpinBox->value());
    handler.setCopyToClipboard(clipboardCheckBox->isChecked());

    for(int m = 0; m < EndOfMode; ++m){
        if(modeList[m]->currentIndex() == -1)
            continue;
        OCRMode mode = static_cast<OCRMode>(m);
        OCRPlatform platform = static_cast<OCRPlatform>(
                    modeList[m]->currentData().toInt());
        handler.setDefaultPlatform(mode, platform);
        qDebug() << QString("save default for mode %1: %2")
                    .arg(ConfigHandler::asModeName(mode))
                    .arg(ConfigHandler::asPlatformName(platform));
    }
}

void GeneralConfigWidget::addAPI(APIConfigWidget *widget)
{
    apiList.append(widget);
}

void GeneralConfigWidget::setColor()
{
    QColor newColor = QColorDialog::getColor();
    if(!newColor.isValid())
        return;
    uiColor = newColor;
    colorPushButton->setStyleSheet(QString("background-color: %1")
                                   .arg(uiColor.name()));
}

void GeneralConfigWidget::updateComboBox()
{
    qDebug() << "update comboBox";
    for(int m = 0; m < EndOfMode; ++m) {
        OCRMode mode = static_cast<OCRMode>(m);
        modeList[m]->hide();
        comboLayout->removeWidget(modeList[m]->label());
        comboLayout->removeWidget(modeList[m]);
        // remove all previous items
        int c = modeList[m]->count();
        for(int i = c-1; i >= 0; --i) {
            modeList[m]->removeItem(i);
        }
        qDebug() << QString("delete %1 items from mode %2, change to %3")
                    .arg(c)
                    .arg(ConfigHandler::asModeName(static_cast<OCRMode>(m)))
                    .arg(modePlatforms[m].size());

        // if has at least one new item
        if (!modePlatforms[m].isEmpty()) {
            for(auto p : modePlatforms[m]) {
                OCRPlatform platform = static_cast<OCRPlatform>(p);
                modeList[m]->addItem(ConfigHandler::asPlatformName(platform), p);
                if(platform == handler.defaultPlatform(mode)) {
                    // if match default from settings
                    modeList[m]->setCurrentIndex(modeList[m]->count()-1);
                }
            }
        }
    }

    for(int m = 0; m < EndOfMode; ++m) if(!modePlatforms[m].isEmpty()) {
        comboLayout->addRow(modeList[m]->label(), modeList[m]);
        modeList[m]->show();
    }
}

void GeneralConfigWidget::paintEvent(QPaintEvent *event)
{
    // ensure that **all** api widgets are configured before this is called
    decltype (modePlatforms) newModePlatforms(EndOfMode);
    for(auto api : apiList) if(api->available()) {
        int p = static_cast<int>(api->platform());
        for(int m = 0; m < EndOfMode; ++m)
            if (api->modeChecked(static_cast<OCRMode>(m))){
                newModePlatforms[m].push_back(p);
            }
    }
    if(newModePlatforms != modePlatforms) {
        modePlatforms = newModePlatforms;
        updateComboBox();
    }
    QWidget::paintEvent(event);
}
