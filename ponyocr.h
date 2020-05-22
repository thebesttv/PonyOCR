#ifndef FUNOCR_H
#define FUNOCR_H

#include <QMainWindow>
#include "Configuation/confighandler.h"
#include "ocrrequest.h"
#include "API/generalapi.h"
#include "Markdown/document.h"
#include "Markdown/previewpage.h"

class QAction;
class QMenu;
class QToolButton;
class QNetworkAccessManager;
class QPlainTextEdit;
class QWebEngineView;
class QSplitter;

class PonyOCR : public QMainWindow
{
    Q_OBJECT
public:
    explicit PonyOCR(QWidget *parent = nullptr);
    ~PonyOCR();

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
    QSplitter *m_splitter;

    QAction *m_showPreviewAction;

    enum OutputMode {
        Replace, Append, Insert
    } m_outputMode;
    QAction *m_outputModeReplaceAction;
    QAction *m_outputModeAppendAction;
    QAction *m_outputModeInsertAction;
    QMenu *m_outputModeMenu;
    QToolButton *m_outputModeButton;

    void about();
    void initAction();
    void initToolBar();
    void initMarkdownPreview();

public slots:
    void requestOCR();
    void process(OCRRequest req);
    void onOCRSuccessful(QString s);
    void onOCRFailure(OCRPlatform platform, int errCode, QString errDescription);
    void onAuthorizationFailure(OCRPlatform platform, QString errDescription);
};

#endif // FUNOCR_H
