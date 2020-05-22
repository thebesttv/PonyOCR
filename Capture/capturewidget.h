#ifndef CAPTUREWIGIT_H
#define CAPTUREWIGIT_H

#include <QWidget>
#include "ocrrequest.h"
#include "capturebuttonhandler.h"
#include "selectionwidget.h"
#include "Configuation/confighandler.h"

class CaptureWidget : public QWidget
{
    Q_OBJECT

public:
    enum WidgetState {
        InitialReadyToSelect,
        ReadyToSelect,
        Selecting,
        SelectionDone,
        ReadyToResize,
        Resizing,
        ReadyToGrab,
        Grabbing
    };

    CaptureWidget(QWidget *parent = nullptr);
    ~CaptureWidget();

public slots:
    void close();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    ConfigHandler m_handler;
    SelectionWidget m_selectionWidget;
    SelectionWidget::SideType m_mouseSide;
    CaptureButtonHandler m_buttonHandler;
    WidgetState m_state;
    bool m_showSizeTooSmallWarning;
    QPixmap m_img;
    QPoint m_startPos;
    int m_minimalWidth;
    void updateSide(const QPoint &pos);
    void updateCursor();

    void renderHelpMenu(QPainter &painter);
    void renderSizeTooSmallWarning(QPainter &painter);
    void renderMenu(QPainter &painter, const QString &text);
    void pushRequest(OCRPlatform platform, OCRMode mode);

signals:
    void captureTaken(OCRRequest req);
    void finished();
};

/*
class CaptureWidget : public QWidget
{
    Q_OBJECT
public:
    CaptureWidget(QWidget *parent = nullptr);
    ~CaptureWidget();

public slots:
    void requestCapture();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_showHelpMenu;
    bool m_showSizeTooSmallWarning;
    bool m_leftClick;
    bool m_rightClick;
    bool m_grabbing;    // grab selection rectangle to change its geometry position
    bool m_captureInProgress, m_captureDone;

    ScreenGrabber grabber;
    QPixmap img;
    ConfigHandler handler;
    CaptureButton *captureButton;

    int captureOption;
    QPoint m_dragStartPoint, endPoint;
    QRect selectionRect;
    OCRRequest req;

    void renderHelpMenu(QPainter *painter);
    void renderSizeTooSmallWarning(QPainter *painter);
    void renderMenu(QPainter *painter, const QString &text);
    void renderCaptureRectangle(QPainter *painter);
    void getOCROption();
    void pushRequest(OCRPlatform platform, OCRMode mode);

signals:
    void captureTaken(OCRRequest req);
};
*/

#endif // CAPTUREWIGIT_H
