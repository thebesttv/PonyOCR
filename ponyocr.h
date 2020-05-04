#ifndef FUNOCR_H
#define FUNOCR_H

#include <QMainWindow>
#include "Configuation/confighandler.h"
#include "ocrrequest.h"
#include "API/generalapi.h"
#include "Markdown/document.h"
#include "Markdown/previewpage.h"

class QAction;
class QNetworkAccessManager;
class QPlainTextEdit;
class QWebEngineView;

class PonyOCR : public QMainWindow
{
    Q_OBJECT
public:
    explicit PonyOCR(QWidget *parent = nullptr);

private:
    ConfigHandler m_handler;
    QNetworkAccessManager *m_normalManager;
    QNetworkAccessManager *m_proxiedManager;
    GeneralAPI m_api;

    QAction *m_OCRAction;
    QAction *m_configAction;

    QPlainTextEdit *m_textEdit;
    // markdown preview
    QWebEngineView *m_view;
    Document m_content;
    PreviewPage *m_page;

    void about();
    void initAction();
    void initToolBar();
    void initMarkdownPreview();

public slots:
    void process(OCRRequest req);
    void onOCRSuccessful(QString s);
};

#endif // FUNOCR_H
