#include "ponyocr.h"
#include "Configuation/configdialog.h"
#include "Capture/capturewidget.h"
#include <QNetworkAccessManager>
#include <QAction>
#include <QPlainTextEdit>
#include <QFont>
#include <QApplication> // used for clipboard action
#include <QClipboard>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QWebEngineView>
#include <QWebChannel>

PonyOCR::PonyOCR(QWidget *parent)
    : QMainWindow(parent)
    , m_manager(new QNetworkAccessManager)
    , m_api(m_manager, this)
{
    setWindowTitle(tr("PonyOCR"));

    initAction();
    initToolBar();

    connect(&m_api, &GeneralAPI::OCRSuccessful,
            this, &PonyOCR::onOCRSuccessful);

    m_textEdit = new QPlainTextEdit();
    m_textEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    initMarkdownPreview();

    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(m_textEdit);
    splitter->addWidget(m_view);
    // evenly distribute two widgets
    // https://stackoverflow.com/questions/43831474/how-to-equally-distribute-the-width-of-qsplitter/43835396
    splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));
    setCentralWidget(splitter);

    setMinimumWidth(800);
    setMinimumHeight(450);

    // load readme
    QFile file(":/README.md");
    file.open(QIODevice::ReadOnly);
    m_textEdit->setPlainText(file.readAll());
    file.close();
}

void PonyOCR::process(OCRRequest req)
{
    m_api.process(req);
}

void PonyOCR::onOCRSuccessful(QString s)
{
    m_textEdit->setPlainText(s);
    if(m_handler.copyToClipboard())
        qApp->clipboard()->setText(s);
}

void PonyOCR::about()
{
    QFile file(":/readme.md");
    file.open(QIODevice::ReadOnly);
    m_textEdit->setPlainText(file.readAll());
}

void PonyOCR::initAction()
{
    m_OCRAction = new QAction(tr("OCR"), this);
    connect(m_OCRAction, &QAction::triggered, [this] () {
        CaptureWidget *widget = new CaptureWidget(this);
#ifdef Q_OS_WIN
        widget->show();
#else
        widget->showFullScreen();
#endif
        connect(widget, &CaptureWidget::captureTaken,
                this, &PonyOCR::process);
    });

    m_configAction = new QAction(tr("Settings"), this);
    connect(m_configAction, &QAction::triggered, [this] () {
//        ConfigDialog *dialog = new ConfigDialog(this);
        ConfigDialog *dialog = new ConfigDialog(nullptr);   // TODO: DEBUG
        dialog->show();
    });
}

void PonyOCR::initToolBar()
{
    QToolBar *toolBar = addToolBar(tr("MainToolBar"));
    toolBar->addAction(m_OCRAction);
    toolBar->addAction(m_configAction);
}

void PonyOCR::initMarkdownPreview()
{
    // set parent of m_page to this to avoid problems with releasing profiles
    m_page = new PreviewPage(this);
    m_view = new QWebEngineView;
    m_view->setPage(m_page);
    m_view->setZoomFactor(1.2);

    connect(m_textEdit, &QPlainTextEdit::textChanged,
            [this]() { m_content.setText(m_textEdit->toPlainText());
    });

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    m_page->setWebChannel(channel);
    m_view->setUrl(QUrl("qrc:/index-katex.html"));
}
