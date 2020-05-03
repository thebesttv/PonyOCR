#include "configdialog.h"
#include "apiconfigwidget.h"
#include "generalconfigwidget.h"
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QKeyEvent>

ConfigDialog::ConfigDialog(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Configuation"));

    m_tab = new QTabWidget;

    // 1. init generalWidget
    // 2. configure all api widgets
    // 3. add api widget to general widget
    m_generalWidget = new GeneralConfigWidget;
    m_tab->addTab(m_generalWidget, tr("General"));
    createBaiduWidget();
    createTencentWidget();
    createOCRSpaceWidget();
    createMathpixWidget();

    createButtonBox();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_tab);
    mainLayout->addWidget(m_buttonBox);

    setLayout(mainLayout);
}

void ConfigDialog::createButtonBox()
{
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Save
                                   | QDialogButtonBox::Cancel);
    connect(m_buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked,
            this, &ConfigDialog::saveConfig);
    connect(m_buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &ConfigDialog::hide);
}

void ConfigDialog::createBaiduWidget()
{
    m_baiduWidget = new APIConfigWidget(OCRPlatform::Baidu, "API Key", "Secret Key",
                                      tr("API is considered unavailable"
                                         "if the first key is left empty"));
    m_baiduWidget->addMode(OCRMode::NormalText);
    m_baiduWidget->addMode(OCRMode::Handwriting);
    m_baiduWidget->addMode(OCRMode::Formula);
    m_baiduWidget->addMode(OCRMode::WebImage);
    m_baiduWidget->addMode(OCRMode::Table);
    m_baiduWidget->addMode(OCRMode::Number);
    m_baiduWidget->addMode(OCRMode::QRCode);

    // remember to add to tab & general widget
    m_tab->addTab(m_baiduWidget, ConfigHandler::asPlatformName(OCRPlatform::Baidu));
    m_generalWidget->addAPI(m_baiduWidget);
}

void ConfigDialog::createTencentWidget()
{
    m_tencentWidget = new APIConfigWidget(OCRPlatform::Tencent, "APPID", "APPKey",
                                      tr("API is considered unavailable"
                                         "if the first key is left empty"));
    m_tencentWidget->addMode(OCRMode::NormalText);
    m_tencentWidget->addMode(OCRMode::Handwriting);

    m_tab->addTab(m_tencentWidget, ConfigHandler::asPlatformName(OCRPlatform::Tencent));
    m_generalWidget->addAPI(m_tencentWidget);
}

void ConfigDialog::createOCRSpaceWidget()
{
    m_ocrspaceWidget = new APIConfigWidget(OCRPlatform::OCR_Space, "API Key", "disabled",
                                           tr("API is considered unavailable"
                                              "if the first key is left empty"));
    m_ocrspaceWidget->addMode(OCRMode::NormalText);

    m_tab->addTab(m_ocrspaceWidget, ConfigHandler::asPlatformName(OCRPlatform::OCR_Space));
    m_generalWidget->addAPI(m_ocrspaceWidget);
}

void ConfigDialog::createMathpixWidget()
{
    m_mathpixWidget = new APIConfigWidget(OCRPlatform::Mathpix, "APP ID", "APP Key",
                                           tr("API is considered unavailable"
                                              "if the first key is left empty"));
    m_mathpixWidget->addMode(OCRMode::Formula);

    m_tab->addTab(m_mathpixWidget, ConfigHandler::asPlatformName(OCRPlatform::Mathpix));
    m_generalWidget->addAPI(m_mathpixWidget);
}

void ConfigDialog::saveConfig()
{
    m_baiduWidget->saveConfig();
    m_tencentWidget->saveConfig();
    m_ocrspaceWidget->saveConfig();
    m_mathpixWidget->saveConfig();

    m_generalWidget->saveConfig();
    hide();
}

void ConfigDialog::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Enter: case Qt::Key_Return:
        m_buttonBox->button(QDialogButtonBox::Save)->click();
        break;
    case Qt::Key_Escape:
        m_buttonBox->button(QDialogButtonBox::Cancel)->click();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}
