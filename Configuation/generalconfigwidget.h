#ifndef GENERALCONFIGWIDGET_H
#define GENERALCONFIGWIDGET_H

#include <QWidget>
#include <QComboBox>
#include "confighandler.h"

class QLabel;

class ModeComboBox : public QComboBox {
    Q_OBJECT

public:
    ModeComboBox(OCRMode mode, QWidget *parent = nullptr);
    QLabel *label();
    void hide();
    void show();

private:
    QLabel *m_label;
    OCRMode m_mode;

private slots:
    void platformChanged();

signals:
    void setDefaultPlatform(OCRMode mode, OCRPlatform platform);
};

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

    void setColor();
    void updateComboBox();
    void setDefaultPlatform(OCRMode, OCRPlatform);

protected:
    void paintEvent(QPaintEvent *) override;
signals:

};

#endif // GENERALCONFIGWIDGET_H
