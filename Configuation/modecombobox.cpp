#include "modecombobox.h"
#include <QLabel>

ModeComboBox::ModeComboBox(OCRMode mode, QWidget *parent)
    : QComboBox(parent), m_mode(mode)
{
    m_label = new QLabel(ConfigHandler::asModeName(mode));
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
