#ifndef PREVIEWPATE_H
#define PREVIEWPATE_H

#include <QWebEnginePage>

class PreviewPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit PreviewPage(QObject *parent = nullptr) : QWebEnginePage(parent) {}

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

#endif // PREVIEWPATE_H
