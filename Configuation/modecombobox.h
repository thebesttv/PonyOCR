#ifndef MODECOMBOBOX_H
#define MODECOMBOBOX_H

#include "confighandler.h"
#include <QComboBox>

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
};

#endif // MODECOMBOBOX_H
