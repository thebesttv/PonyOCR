#include "capturewidget.h"
#include "screengrabber.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QtDebug>
#include <QShortcut>

CaptureWidget::CaptureWidget(QWidget *parent)
    : QWidget(parent), m_selectionWidget(this)
    , m_mouseSide(SelectionWidget::NoSide)
    , m_buttonHandler(this)
    , m_state(InitialReadyToSelect)
    , m_showSizeTooSmallWarning(false)
{
#ifdef Q_OS_WIN
    QPoint topLeft(0,0);
    setWindowFlags(Qt::WindowStaysOnTopHint
                 | Qt::FramelessWindowHint
                 | Qt::Popup);

    // find the top left screen
    for (QScreen *const screen : QGuiApplication::screens()) {
        QPoint topLeftScreen = screen->geometry().topLeft();
        if (topLeft.x() > topLeftScreen.x() ||
                topLeft.y() > topLeftScreen.y()) {
            topLeft = topLeftScreen;
        }
    }
    move(topLeft);
#else
    setWindowFlags(Qt::BypassWindowManagerHint
                   | Qt::WindowStaysOnTopHint
                   | Qt::FramelessWindowHint
                   | Qt::Tool);
#endif

    QVector<QRect> areas;
    for(QScreen * screen : QGuiApplication::screens()) {
        QRect r = screen->geometry();
#ifdef Q_OS_WIN
        r.moveTo(r.topLeft() - topLeft);
#endif
        areas.append(r);
    }
    m_buttonHandler.setScreenRegions(areas);

    m_minimalWidth = 20;
    m_img = ScreenGrabber::grabEntireDesktop();
    resize(m_img.size());

    setMouseTracking(true);

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(close()));
    connect(&m_buttonHandler, &CaptureButtonHandler::selectionDone,
            this, &CaptureWidget::pushRequest);
}

CaptureWidget::~CaptureWidget()
{
}

void CaptureWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    m_buttonHandler.hideAll();  // hide selection buttons
    if (m_state == InitialReadyToSelect
     || m_state == ReadyToSelect) {
        m_showSizeTooSmallWarning = false;
        m_state = Selecting;
        m_startPos = event->pos();
        m_selectionWidget.setGeometry(QRect(m_startPos, m_startPos));
    } else if (m_state == SelectionDone
            || m_state == ReadyToResize) {
        m_state = Resizing;
        m_buttonHandler.hideAll();
        m_startPos = event->pos();
        m_selectionWidget.saveGeometry();
    } else if (m_state == ReadyToGrab) {
        m_state = Grabbing;
        m_buttonHandler.hideAll();
        m_startPos = event->pos();
        m_selectionWidget.saveGeometry();
        setCursor(Qt::ClosedHandCursor);
    }
}

void CaptureWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_state == InitialReadyToSelect) {
        setCursor(Qt::CrossCursor);
    } else if(m_state == Selecting) {
        m_selectionWidget.setGeometry(QRect(m_startPos, event->pos()).normalized());
        update();
    } else if(m_state == Grabbing) {
        QRect rect = m_selectionWidget.savedGeometry();
        rect.moveTo(rect.topLeft() + (event->pos() - m_startPos));
        m_selectionWidget.setGeometry(rect);
        update();
    } else if(m_state == Resizing) {
        QPoint delta = event->pos() - m_startPos;
        QRect rect = m_selectionWidget.savedGeometry();
        using sw = SelectionWidget;
        if(m_mouseSide == sw::TopSide
        || m_mouseSide == sw::TopLeftCorner
        || m_mouseSide == sw::TopRightCorner) {
            rect.setTop(qMin(rect.top() + delta.y(),
                             rect.bottom() - m_minimalWidth));
        }

        if(m_mouseSide == sw::BottomSide
        || m_mouseSide == sw::BottomLeftCorner
        || m_mouseSide == sw::BottomRightCorner) {
            rect.setBottom(qMax(rect.bottom() + delta.y(),
                                rect.top() + m_minimalWidth));
        }

        if(m_mouseSide == sw::LeftSide
        || m_mouseSide == sw::TopLeftCorner
        || m_mouseSide == sw::BottomLeftCorner) {
            rect.setLeft(qMin(rect.left() + delta.x(),
                              rect.right() - m_minimalWidth));
        }

        if(m_mouseSide == sw::RightSide
        || m_mouseSide == sw::TopRightCorner
        || m_mouseSide == sw::BottomRightCorner) {
            rect.setRight(qMax(rect.right() + delta.x(),
                               rect.left() + m_minimalWidth));
        }

        if(rect.width() >= m_minimalWidth && rect.height() >= m_minimalWidth)
            m_selectionWidget.setGeometry(rect.normalized());
        update();

        // no need when minimal width is set
        // If mouse side of the selection rectangle changes during resizing
        // (i.e. from bottom right to bottom left), then see this change as
        // a new resize.
//        if(m_mouseSide == sw::TopLeftCorner
//        || m_mouseSide == sw::TopRightCorner
//        || m_mouseSide == sw::BottomLeftCorner
//        || m_mouseSide == sw::BottomRightCorner
//        || m_mouseSide == sw::NoSide) {
//            auto oldSide = m_mouseSide;
//            updateSide(event->pos());
//            if(oldSide != m_mouseSide){
//                m_startPos = event->pos();
//                m_selectionWidget.saveGeometry();
//            }
//        }
    } else {    // SelectionDone, ReadyTo...
        updateSide(event->pos());
        if (m_mouseSide != SelectionWidget::NoSide) {
            m_state = ReadyToResize;
        } else if (m_selectionWidget.geometry().contains(event->pos())) {
            m_state = ReadyToGrab;
            setCursor(Qt::OpenHandCursor);
        } else {
            m_state = ReadyToSelect;
            setCursor(Qt::CrossCursor);
        }
    }
}

void CaptureWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_state == Selecting) {
        // size too small
        if(m_selectionWidget.geometry().width() < m_minimalWidth
        || m_selectionWidget.geometry().height() < m_minimalWidth) {
            m_state = InitialReadyToSelect;
            m_showSizeTooSmallWarning = true;
            update();
            return;
        }

        m_state = SelectionDone;
        int x1 = m_startPos.x(), y1 = m_startPos.y(),
            x2 = event->pos().x(), y2 = event->pos().y();
        if((x2 >= x1 && y2 >= y1) || (x2 <= x1 && y2 <= y1)) {
            m_buttonHandler.defaultPush();
        } else {
            m_buttonHandler.drawPush(m_selectionWidget.geometry());
        }

        updateSide(event->pos());
        update();
    } else if (m_state == Grabbing) {
        m_state = ReadyToGrab;
        m_buttonHandler.drawPush(m_selectionWidget.geometry());
        setCursor(Qt::OpenHandCursor);
    } else if (m_state == Resizing) {
        m_state = ReadyToResize;
        m_buttonHandler.drawPush(m_selectionWidget.geometry());
        updateSide(event->pos());
    }
}

void CaptureWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_img);
    painter.setRenderHint(QPainter::Antialiasing);

    QRegion coverRegion(rect());
    if(m_state != InitialReadyToSelect) {
        coverRegion = coverRegion.subtracted(
                    m_selectionWidget.geometry().adjusted(0, 0, -1, -1));
    }
    painter.setBrush(QColor(0, 0, 0, 191));
    painter.setClipRegion(coverRegion); // exclude selected region
    painter.drawRect(rect());

    if(m_state != InitialReadyToSelect) {
        m_selectionWidget.paintHandler(painter);
    } else {
        if(m_showSizeTooSmallWarning) {
            renderSizeTooSmallWarning(painter);
        } else {
            renderHelpMenu(painter);
        }
    }
}

void CaptureWidget::updateSide(const QPoint &pos)
{
    m_mouseSide = m_selectionWidget.getMouseSide(pos);
    updateCursor();
}

void CaptureWidget::updateCursor()
{
    using sw = SelectionWidget::SideType;
    switch(m_mouseSide) {
    case sw::TopLeftCorner: case sw::BottomRightCorner:
        setCursor(Qt::SizeFDiagCursor); break;
    case sw::TopRightCorner: case sw::BottomLeftCorner:
        setCursor(Qt::SizeBDiagCursor); break;
    case sw::TopSide: case sw::BottomSide:
        setCursor(Qt::SizeVerCursor); break;
    case sw::LeftSide: case sw::RightSide:
        setCursor(Qt::SizeHorCursor); break;
    default:
        break;
    }
}

void CaptureWidget::renderHelpMenu(QPainter &painter)
{
    QString helpText = tr("Swipe along the main diagonal for quick OCR"
                          "\nAlong the antidiagonal for detailed options"
                          "\nNote that picture should not be too small");
    renderMenu(painter, helpText);
}

void CaptureWidget::renderSizeTooSmallWarning(QPainter &painter)
{
    QString warning = tr("Picture size too small!");
    renderMenu(painter, warning);
}

void CaptureWidget::renderMenu(QPainter &painter, const QString &text)
{
    painter.save();
    QScreen *screen = qApp->primaryScreen();
    QRect rect(screen->geometry());
    // translate global position to widgit coordinates
    rect.moveTo(mapFromGlobal(rect.topLeft()));

    QRectF bRect = painter.boundingRect(rect, Qt::AlignCenter, text);

    const int margin = QApplication::fontMetrics().height() / 2;
    bRect.setWidth(bRect.width() + margin);
    bRect.setHeight(bRect.height() + margin);
    bRect.setX(bRect.x() - margin);
    bRect.setY(bRect.y() - margin);

    QColor rectColor(m_handler.UIMainColor());
    QColor textColor(m_handler.colorIsDark(rectColor)
                     ? Qt::white : Qt::black);

    painter.setBrush(QBrush(rectColor, Qt::SolidPattern));
    painter.setPen(textColor);

    painter.drawRect(bRect);
    painter.drawText(rect, Qt::AlignCenter, text);

    painter.restore();
}

void CaptureWidget::pushRequest(OCRPlatform platform, OCRMode mode)
{
    OCRRequest req(mode, platform, m_img.copy(m_selectionWidget.geometry()));
    qDebug().noquote() << QString("push request (%1, %2)")
                          .arg(ConfigHandler::asPlatformName(platform))
                          .arg(ConfigHandler::asModeName(mode));
    emit captureTaken(req);
    close();
}

/*
#include "capturewidget.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QKeyEvent>

#include <QDebug>

CaptureWidget::CaptureWidget(QWidget *parent)
    : QWidget(parent), handler(this)
{
    setWindowFlags(Qt::BypassWindowManagerHint
                   | Qt::WindowStaysOnTopHint
                   | Qt::FramelessWindowHint
                   | Qt::Tool);

    captureButton = new CaptureButton(this);
    connect(captureButton, &CaptureButton::selectionDone,
            this, &CaptureWidget::pushRequest);
}

CaptureWidget::~CaptureWidget()
{
}

void CaptureWidget::requestCapture()
{
    setCursor(Qt::CrossCursor);
    m_showHelpMenu = true;
    m_showSizeTooSmallWarning = false;
    m_captureInProgress = m_captureDone = false;
    captureOption = -1;
    m_dragStartPoint = endPoint = QPoint();

    img = grabber.grabEntireDesktop();
    resize(img.size());

#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    showFullScreen();
#else
    show();
#endif
}

void CaptureWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        hide();
        captureButton->hideAll();
    }
}

void CaptureWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    // background
    painter.drawPixmap(0, 0, img);
    int opacity = handler.backgroundOpacity() * 255.0 / 100.0;
    QColor overlayColor(0, 0, 0, opacity);
    painter.setBrush(overlayColor);
    painter.drawRect(-1, -1, rect().width() + 1, rect().height() + 1);

    // front panel
    if(m_showHelpMenu){
        renderHelpMenu(&painter);
    }else if(m_showSizeTooSmallWarning){
        renderSizeTooSmallWarning(&painter);
    }else if(!m_dragStartPoint.isNull() || !endPoint.isNull()){
        renderCaptureRectangle(&painter);
    }
}

void CaptureWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "CaptureWidget: mouse press";
    if(event->button() == Qt::LeftButton && !m_captureDone){
        m_showHelpMenu = m_showSizeTooSmallWarning = false;
        m_captureInProgress = true;
        m_dragStartPoint = event->pos();
    }
}

void CaptureWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_captureInProgress){
        endPoint = event->pos();
    }
    update();
}

void CaptureWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "CaptureWidget: mouse release";
    if(event->button() == Qt::LeftButton){
        endPoint = event->pos();
        m_captureInProgress = false;
        m_captureDone = true;

        int dx = qAbs(m_dragStartPoint.x() - endPoint.x()),
            dy = qAbs(m_dragStartPoint.y() - endPoint.y());

        if(dx < 20 || dy < 20){
            m_showSizeTooSmallWarning = true;
        }else{
            getOCROption();
            qDebug() << "OCROption: " << captureOption;
            req.setImage(img.copy(selectionRect));
            if(captureOption == 1){
                captureButton->defaultPush();
            }else{
                captureButton->drawPush(event->pos());
            }
            setCursor(Qt::ArrowCursor);
        }
        update();
    }
}

void CaptureWidget::renderHelpMenu(QPainter *painter)
{
    QString helpText = tr("Swipe along the main diagonal for quick OCR"
                          "\nAlong the antidiagonal for detailed options"
                          "\nNote that picture should not be too small");
    renderMenu(painter, helpText);
}

void CaptureWidget::renderSizeTooSmallWarning(QPainter *painter)
{
    QString warning = tr("Picture size too small!");
    renderMenu(painter, warning);
}

void CaptureWidget::renderMenu(QPainter *painter, const QString &text)
{
    painter->save();
    QScreen *screen = qApp->primaryScreen();
    QRect rect(screen->geometry());
    // translate global position to widgit coordinates
    rect.moveTo(mapFromGlobal(rect.topLeft()));

    QRectF bRect = painter->boundingRect(rect, Qt::AlignCenter, text);

    const int margin = QApplication::fontMetrics().height() / 2;
    bRect.setWidth(bRect.width() + margin);
    bRect.setHeight(bRect.height() + margin);
    bRect.setX(bRect.x() - margin);
    bRect.setY(bRect.y() - margin);

    QColor rectColor(handler.UIMainColor());
    QColor textColor(handler.colorIsDark(rectColor)
                     ? Qt::white : Qt::black);

    painter->setBrush(QBrush(rectColor, Qt::SolidPattern));
    painter->setPen(textColor);

    painter->drawRect(bRect);
    painter->drawText(rect, Qt::AlignCenter, text);

    painter->restore();
}

void CaptureWidget::renderCaptureRectangle(QPainter *painter)
{
    int x1 = m_dragStartPoint.x(), y1 = m_dragStartPoint.y();
    int x2 = endPoint.x(), y2 = endPoint.y();
    if(x1 > x2) qSwap(x1,x2);
    if(y1 > y2) qSwap(y1,y2);

    selectionRect = QRect(QPoint(x1,y1), QPoint(x2,y2));
    painter->drawPixmap(selectionRect, img, selectionRect);
}

void CaptureWidget::getOCROption()
{
    int x1 = m_dragStartPoint.x(), y1 = m_dragStartPoint.y();
    int x2 = endPoint.x(), y2 = endPoint.y();

    if((x1 < x2 && y1 < y2) || (x1 > x2 && y1 > y2)){
        captureOption = 1;
    }else{
        captureOption = 2;
    }
}

void CaptureWidget::pushRequest(OCRPlatform platform, OCRMode mode)
{
    req.setPlatform(platform);
    req.setMode(mode);
    qDebug() << QString("push request (%1, %2)")
                .arg(ConfigHandler::asPlatformName(platform))
                .arg(ConfigHandler::asModeName(mode));
    emit captureTaken(req);
    hide();
}
*/
