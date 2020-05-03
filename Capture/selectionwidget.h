#ifndef SELECTIONWIDGET_H
#define SELECTIONWIDGET_H

#include <QWidget>

class SelectionWidget : public QWidget
{
    Q_OBJECT
public:
    enum SideType {
        TopSide,
        BottomSide,
        LeftSide,
        RightSide,
        TopLeftCorner,
        TopRightCorner,
        BottomLeftCorner,
        BottomRightCorner,
        NoSide
    };

    explicit SelectionWidget(QWidget *parent = nullptr);

    SideType getMouseSide(const QPoint &pos);
    static QString asSideName(SideType side);
    void setGeometry(const QRect &rect);
    void paintHandler(QPainter &painter);
    void saveGeometry();
    QRect savedGeometry();

private:
    int m_handlerRadius, m_handlerWidth, m_handlerX, m_handlerY;
    QRect m_oldGeomotry, m_borderRect;
    QRect m_TopArea, m_BottomArea, m_LeftArea, m_RightArea,
          m_TopLeftVArea, m_TopRightVArea, m_BottomLeftVArea, m_BottomRightVArea,
          m_TopLeftHArea, m_TopRightHArea, m_BottomLeftHArea, m_BottomRightHArea;

    void updateArea(QRect rect);
    QPainterPath getPainterPath();
};

#endif // SELECTIONWIDGET_H
