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
    ConfigHandler handler;
    QList<APIConfigWidget*> apiList;
    QList<ModeComboBox*> modeList;
    QVector<QList<int>> modePlatforms;
    QFormLayout *comboLayout;

    QColor uiColor;
    QLabel *colorLabel;
    QPushButton *colorPushButton;

    QLabel *opacityLabel;
    QSpinBox *opacitySpinBox;
    QSlider *opacitySlider;

    QCheckBox *clipboardCheckBox;

    QComboBox *languageCombo;

    void setColor();
    void updateComboBox();
    void setDefaultPlatform(OCRMode, OCRPlatform);

protected:
    void paintEvent(QPaintEvent *) override;
};

#endif // GENERALCONFIGWIDGET_H
