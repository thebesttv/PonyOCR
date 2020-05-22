#ifndef GENERALCONFIGWIDGET_H
#define GENERALCONFIGWIDGET_H

#include <QWidget>
#include "confighandler.h"
#include "modecombobox.h"

class QPushButton;
class QSlider;
class QSpinBox;
class APIConfigWidget;
class QCheckBox;
class QFormLayout;

class GeneralConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralConfigWidget(QWidget *parent = nullptr);
    void saveConfig();
    void addAPI(APIConfigWidget *widget);

private:
    ConfigHandler m_handler;

    QColor m_uiColor;
    QLabel *m_colorLabel;
    QPushButton *m_colorPushButton;

    QLabel *m_opacityLabel;
    QSpinBox *m_opacitySpinBox;
    QSlider *m_opacitySlider;

    QCheckBox *m_clipboardCheckBox;

    QComboBox *m_languageCombo;

    QList<APIConfigWidget*> m_apiList;
    QList<ModeComboBox*> m_modeList;
    QVector<QList<int>> m_modePlatforms;
    QFormLayout *m_comboLayout;

    void setColor();
    void updateComboBox();
    void setDefaultPlatform(OCRMode, OCRPlatform);

protected:
    void paintEvent(QPaintEvent *) override;
};

#endif // GENERALCONFIGWIDGET_H
