#include "selectionwidget.h"
#include "Configuation/confighandler.h"
#include <QPainter>
#include <QPainterPath>
#include <QtDebug>

SelectionWidget::SelectionWidget(QWidget *parent) : QWidget(parent)
{
    m_handlerRadius = ConfigHandler().handlerRadius();
    m_handlerWidth = 5 * m_handlerRadius;
    hide();
}

SelectionWidget::SideType SelectionWidget::getMouseSide(const QPoint &pos)
{
    if(m_TopLeftHArea.contains(pos) || m_TopLeftVArea.contains(pos))
        return TopLeftCorner;
    if(m_TopRightHArea.contains(pos) || m_TopRightVArea.contains(pos))
        return TopRightCorner;
    if(m_BottomRightHArea.contains(pos) || m_BottomRightVArea.contains(pos))
        return BottomRightCorner;
    if(m_BottomLeftHArea.contains(pos) || m_BottomLeftVArea.contains(pos))
        return BottomLeftCorner;
    if(m_TopArea.contains(pos))
        return TopSide;
    if(m_RightArea.contains(pos))
        return RightSide;
    if(m_BottomArea.contains(pos))
        return BottomSide;
    if(m_LeftArea.contains(pos))
        return LeftSide;

    return NoSide;
}

QString SelectionWidget::asSideName(SelectionWidget::SideType side)
{
    switch(side) {
    case TopSide:
        return "top side";
    case BottomSide:
        return "bottom side";
    case LeftSide:
        return "left side";
    case RightSide:
        return "right side";
    case TopLeftCorner:
        return "top left corner";
    case TopRightCorner:
        return "top right corner";
    case BottomRightCorner:
        return "bottom right corner";
    case BottomLeftCorner:
        return "bottom left corner";
    default:
        return "no side";
    }
}

void SelectionWidget::setGeometry(const QRect &rect)
{
    // assuming rect is already normalized
    QWidget::setGeometry(rect);
    updateArea(rect);
}

// size of the selection widget is exactly the same as selection
// m_borderRect is slightly larger than selection
void SelectionWidget::updateArea(QRect rect)
{
    int r = m_handlerRadius, w = m_handlerWidth;
    rect.adjust(-r, -r, r, r);
    m_borderRect = rect;

    // make sure handls at corner don't overlap
    int x = m_handlerX = qMin(m_borderRect.width()/2 - r, w);
    int y = m_handlerY = qMin(m_borderRect.height()/2 - r, w);

    // add 2 px offset to both the width and the height of the rectangle
    QSize hSize(x + r + 2, 2 * r + 2);
    QSize vSize(2 * r + 2, y + r + 2);
    // move all areas left & up 1 px
    r += 1;

    m_TopLeftHArea = QRect(rect.topLeft() - QPoint(r, r), hSize);
    m_TopLeftVArea = QRect(rect.topLeft() - QPoint(r, r), vSize);

    m_TopRightHArea = QRect(rect.topRight() - QPoint(x, r), hSize);
    m_TopRightVArea = QRect(rect.topRight() - QPoint(r, r), vSize);

    m_BottomLeftHArea = QRect(rect.bottomLeft() - QPoint(r, r), hSize);
    m_BottomLeftVArea = QRect(rect.bottomLeft() - QPoint(r, y), vSize);

    m_BottomRightHArea = QRect(rect.bottomRight() - QPoint(x, r), hSize);
    m_BottomRightVArea = QRect(rect.bottomRight() - QPoint(r, y), vSize);

//    if(x < w) {
//        m_TopArea = m_BottomArea = QRect();
//    } else {
        m_TopArea = QRect(m_TopLeftHArea.topRight(), m_TopRightHArea.bottomLeft());
        m_BottomArea = QRect(m_BottomLeftHArea.topRight(), m_BottomRightHArea.bottomLeft());
//    }

//    if(y < w) {
//        m_LeftArea = m_RightArea = QRect();
//    } else {
        m_LeftArea = QRect(m_TopLeftVArea.bottomLeft(), m_BottomLeftVArea.topRight());
        m_RightArea = QRect(m_TopRightVArea.bottomLeft(), m_BottomRightVArea.topRight());
//    }
}

void SelectionWidget::paintHandler(QPainter &painter)
{
    QPainterPath path = getPainterPath();
    // draw white inner border
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(m_borderRect);

    // draw grey border
    painter.setPen(Qt::gray);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(m_borderRect);
    painter.drawRect(m_borderRect.adjusted(m_handlerRadius, m_handlerRadius,
                                     -m_handlerRadius, -m_handlerRadius));

    painter.setPen(Qt::gray);
    painter.setBrush(Qt::white);

    painter.save();
    painter.translate(m_borderRect.topLeft());
    painter.drawPath(path);
    painter.restore();

    painter.save();
    painter.translate(m_borderRect.topRight());
    painter.scale(-1, 1);
    painter.drawPath(path);
    painter.restore();

    painter.save();
    painter.translate(m_borderRect.bottomRight());
    painter.scale(-1, -1);
    painter.drawPath(path);
    painter.restore();

    painter.save();
    painter.translate(m_borderRect.bottomLeft());
    painter.scale(1, -1);
    painter.drawPath(path);
    painter.restore();
}

void SelectionWidget::saveGeometry()
{
    m_oldGeomotry = geometry();
}

QRect SelectionWidget::savedGeometry()
{
    return m_oldGeomotry;
}

// get painter path from border rectangle
QPainterPath SelectionWidget::getPainterPath()
{
    QPainterPath path;
    int r = m_handlerRadius, x = m_handlerX, y = m_handlerY;

    path.moveTo(r, r);
    path.lineTo(x, r);
    path.arcTo(x-r, -r, 2*r, 2*r, -90, 180);
    path.lineTo(0, -r);
    path.arcTo(-r, -r, 2*r, 2*r, 90, 90);
    path.lineTo(-r, y);
    path.arcTo(-r, y-r, 2*r, 2*r, 180, 180);
    path.closeSubpath();
    return path;
}
