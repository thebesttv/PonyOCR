#include "capturebuttonhandler.h"
#include <QMouseEvent>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

CaptureButtonHandler::CaptureButtonHandler(QWidget *parent)
    : QWidget(parent), m_visible(false)
{
    hide();

    m_buttonWidth = m_buttonHeight = 0;
    m_mbArray.resize(EndOfMode);
    for(int m = 0; m < EndOfMode; ++m){
        ModeButton *btn = new ModeButton(static_cast<OCRMode>(m), parent);
        m_mbArray[m] = btn; btn->hide();
        connect(btn, &ModeButton::enterMode,
                this, &CaptureButtonHandler::setModeAndUpdate);
        connect(btn, &ModeButton::buttonClicked,
                this, &CaptureButtonHandler::setModeAndPush);
        m_buttonWidth = qMax(m_buttonWidth, btn->sizeHint().width());
        m_buttonHeight = qMax(m_buttonHeight, btn->sizeHint().height());
    }

    m_pbArray.resize(EndOfPlatform);
    for(int i = 0; i < EndOfPlatform; ++i){
        PlatformButton *btn = new PlatformButton(static_cast<OCRPlatform>(i), parent);
        m_pbArray[i] = btn; btn->hide();
        connect(btn, &PlatformButton::buttonClicked,
                this, &CaptureButtonHandler::setPlatformAndPush);
        m_buttonWidth = qMax(m_buttonWidth, btn->sizeHint().width());
        m_buttonHeight = qMax(m_buttonHeight, btn->sizeHint().height());
    }

    for(int i = 0; i < EndOfMode; ++i)
        m_mbArray[i]->resize(m_buttonWidth, m_buttonHeight);

    for(int i = 0; i < EndOfPlatform; ++i)
        m_pbArray[i]->resize(m_buttonWidth, m_buttonHeight);
}

void CaptureButtonHandler::setScreenRegions(const QVector<QRect> &v)
{
    m_regions = v;
    qDebug().noquote() << "screens:" << v;
}

void CaptureButtonHandler::hideAll()
{
    if(!m_visible) return;
    for(int i = 0; i < EndOfMode; ++i)
        m_mbArray[i]->hide();
    for(int i = 0; i < EndOfPlatform; ++i)
        m_pbArray[i]->hide();
}

void CaptureButtonHandler::defaultPush()
{
    if(modeAvailable(OCRMode::NormalText))
        emit selectionDone(m_handler.defaultPlatform(OCRMode::NormalText),
                           OCRMode::NormalText);
}

void CaptureButtonHandler::drawPush(QRect selection)
{
    m_visible = true;
    m_platform = EndOfPlatform;
    m_mode = EndOfMode;

    // draw mode button
    int modeCnt = 0;
    for(int m = 0; m < EndOfMode; ++m)
        if(modeAvailable(static_cast<OCRMode>(m)))
            ++modeCnt;
    m_modeRect.setWidth(m_buttonWidth * 2);
    m_modeRect.setHeight(m_buttonHeight * modeCnt);

    int d = m_handler.handlerRadius() * 2;

    // left
    QPoint point(selection.bottomLeft().x() - d,
                 selection.bottomLeft().y() + d);
    m_modeRect.moveBottomRight(point);
    if(fullyContained(m_modeRect)) {
        m_reverse = true;
        m_pbX = m_modeRect.left();
        drawModeButton(point.x() - m_buttonWidth,
                       point.y() - m_buttonHeight);
        return;
    }

    // down
    m_modeRect.moveTopLeft(point);
    if(fullyContained(m_modeRect)) {
        m_reverse = false;
        m_pbX = m_modeRect.left() + m_buttonWidth;
        drawModeButton(point.x(), point.y());
        return;
    }

    // top
    point.setX(selection.topLeft().x() - d);
    point.setY(selection.topLeft().y() - d);
    m_modeRect.moveBottomLeft(point);
    int topPre = fullyContained(m_modeRect) ?
                selection.height() : INT_MAX - 1;

    point.setX(selection.bottomRight().x() + d);
    point.setY(selection.bottomRight().y() + d);
    m_modeRect.moveBottomLeft(point);
    int bottomPre = fullyContained(m_modeRect) ?
                selection.width() : INT_MAX;

    // if selection has the same height & width, choose height
    if(topPre == bottomPre)
        bottomPre = INT_MAX;

    point = selection.bottomLeft();
    m_modeRect.moveBottomLeft(point);
    int insidePre = fullyContained(m_modeRect) ?
                qMax(m_modeRect.height(), m_modeRect.width()) * 2 : INT_MAX;

    int type = 0;
    // 0: too small
    // 1: inside
    // 2: top left
    // 3: bottom right

    if(topPre < bottomPre) {
        if(topPre == INT_MAX - 1) { // only inside possible
            type = insidePre != INT_MAX ? 1 : 0;
        } else {    // inside | top
            type = insidePre < topPre ? 1 : 2;
        }
    } else {    // bottom | inside
        type = insidePre < bottomPre ? 1 : 3;
    }

    if(type != 0) {
        int x, y;
        switch (type) {
        case 1:
            x = selection.bottomLeft().x();
            y = selection.bottomRight().y() - m_buttonHeight;
            break;
        case 2:
            x = selection.topLeft().x() - d;
            y = selection.topLeft().y() - d - m_buttonHeight;
            break;
        case 3:
            x = selection.bottomRight().x() + d;
            y = selection.bottomRight().y() + d - m_buttonHeight;
            break;
        }

        m_pbX = x + m_buttonWidth;
        m_reverse = true;
        drawModeButton(x, y);
        return;
    }

    // the last resort
    point.setX(selection.bottomLeft().x() - d);
    point.setY(selection.bottomLeft().y() + d);
    m_modeRect.moveTopRight(point);
    if(fullyContained(m_modeRect)) {
        m_pbX = m_modeRect.left();
        m_reverse = false;
        drawModeButton(point.x() - m_buttonWidth, point.y());
        return;
    }

    qWarning() << "range too small to display mode buttons";
    hideAll(); return;
}

void CaptureButtonHandler::setPlatformAndPush(OCRPlatform p)
{
    m_platform = p;
    emit selectionDone(m_platform, m_mode);
    hideAll();
}

void CaptureButtonHandler::setModeAndPush(OCRMode m)
{
    m_mode = m;
    m_platform = m_handler.defaultPlatform(m_mode);
    emit selectionDone(m_platform, m_mode);
    hideAll();
}

void CaptureButtonHandler::setModeAndUpdate(OCRMode m)
{
    m_mode = m;
    updatePlatformButton();
}

void CaptureButtonHandler::drawModeButton(int x, int y)
{
    for(int m = 0; m < EndOfMode; ++m) {
        if(!modeAvailable(static_cast<OCRMode>(m)))
            continue;
        m_mbArray[m]->move(x, y);
        m_mbArray[m]->show();
        if(m_reverse) y -= m_buttonHeight;
        else y += m_buttonHeight;
    }
}

bool CaptureButtonHandler::fullyContained(const QRect &rect)
{
    for(const QRect &ori : m_regions)
        if((ori & rect) == rect) {
            return true;
        }
    return false;
}

bool CaptureButtonHandler::modeAvailable(OCRMode mode)
{
    //    for(int p = 0; p < EndOfPlatform; ++p) {
    //        if(m_handler.modeAvailable(static_cast<OCRPlatform>(p),
    //                                   static_cast<OCRMode>(mode))) {
    //            return true;
    //        }
    //    }
    //    return false;
    return m_handler.defaultPlatform(mode) != EndOfPlatform;
}

void CaptureButtonHandler::updatePlatformButton()
{
    if(m_mode == EndOfMode) {
        for(int p = 0; p < EndOfPlatform; ++p)
            if(m_pbArray[p]->isVisible())
                m_pbArray[p]->hide();
        return;
    }

    int x = m_pbX;
    int y = m_mbArray[m_mode]->pos().y();
    QRect screenRect;
    for(auto r : m_regions)
        if(r.contains(m_mbArray[m_mode]->geometry())) {
            screenRect = r;
            break;
        }

    if(screenRect.isNull()) {
        qWarning() << "unable to draw platform buttons: screenRect is null";
        return;
    }

    if(m_reverse) {
        for(int p = 0; p < EndOfPlatform; ++p)
            if(m_handler.modeAvailable(static_cast<OCRPlatform>(p), m_mode))
                y -= m_buttonHeight;

        if(y < screenRect.top())
            y = m_mbArray[m_mode]->pos().y() + screenRect.top() - y;
        else
            y = m_mbArray[m_mode]->pos().y();

        for(int p = 0; p < EndOfPlatform; ++p) {
            if(m_handler.modeAvailable(static_cast<OCRPlatform>(p), m_mode)) {
                m_pbArray[p]->move(x, y);
                m_pbArray[p]->show();
                y -= m_buttonHeight;
            } else if (m_pbArray[p]->isVisible()) {
                m_pbArray[p]->hide();
            }
        }
    } else {
        for(int p = 0; p < EndOfPlatform; ++p)
            if(m_handler.modeAvailable(static_cast<OCRPlatform>(p), m_mode))
                y += m_buttonHeight;

        if(y > screenRect.bottom())
            y = m_mbArray[m_mode]->pos().y() - (y - screenRect.bottom());
        else
            y = m_mbArray[m_mode]->pos().y();

        for(int p = 0; p < EndOfPlatform; ++p) {
            if(m_handler.modeAvailable(static_cast<OCRPlatform>(p), m_mode)) {
                m_pbArray[p]->move(x, y);
                m_pbArray[p]->show();
                y += m_buttonHeight;
            } else if (m_pbArray[p]->isVisible()) {
                m_pbArray[p]->hide();
            }
        }
    }
}
