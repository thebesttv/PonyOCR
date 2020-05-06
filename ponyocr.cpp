#include "ponyocr.h"
#include "Configuation/configdialog.h"
#include "Capture/capturewidget.h"
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QAction>
#include <QPlainTextEdit>
#include <QFont>
#include <QApplication> // used for clipboard action
#include <QClipboard>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QWebEngineView>
#include <QWebChannel>

PonyOCR::PonyOCR(QWidget *parent)
    : QMainWindow(parent)
    , m_normalManager(new QNetworkAccessManager)
    , m_proxiedManager(new QNetworkAccessManager)
    , m_api(m_normalManager, m_proxiedManager, this)
    , m_outputMode(Replace) // default to replace
{
    setWindowTitle(tr("PonyOCR"));

    initAction();
    initToolBar();

    QNetworkProxy proxy;
    proxy.setType(m_handler.proxyType());
    if(proxy.type() != QNetworkProxy::NoProxy) {
        proxy.setHostName(m_handler.proxyHost(proxy.type()));
        proxy.setPort(m_handler.proxyPort(proxy.type()));
    }
    m_proxiedManager->setProxy(proxy);

    connect(&m_api, &GeneralAPI::OCRSuccessful,
            this, &PonyOCR::onOCRSuccessful);

    m_textEdit = new QPlainTextEdit();
    m_textEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_textEdit->setMinimumSize(400, 400);

    initMarkdownPreview();

    m_splitter = new QSplitter(this);
    m_splitter->addWidget(m_textEdit);
    m_splitter->addWidget(m_view);
    // evenly distribute two widgets
    // https://stackoverflow.com/questions/43831474/how-to-equally-distribute-the-width-of-qsplitter/43835396
    m_splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));
    setCentralWidget(m_splitter);
    // forbid collapsing
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);

    if(!m_showPreviewAction->isChecked()) {
        m_view->hide();
    }

    // load readme
    QFile file(":/README.md");
    file.open(QIODevice::ReadOnly);
    m_textEdit->setPlainText(file.readAll());
    file.close();
}

PonyOCR::~PonyOCR()
{
    m_handler.setPreviewVisible(m_showPreviewAction->isChecked());
}

void PonyOCR::process(OCRRequest req)
{
    m_api.process(req);
}

void PonyOCR::onOCRSuccessful(QString s)
{
    if(m_outputMode == Replace) {
        m_textEdit->setPlainText(s);
    } else if(m_outputMode == Append) {
        m_textEdit->moveCursor(QTextCursor::End);
        m_textEdit->insertPlainText('\n' + s);
    } else {
        m_textEdit->insertPlainText(s);
    }

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
    m_OCRAction = new QAction("OCR", this);
    m_OCRAction->setToolTip("Take screenshot and recognize");
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
        // TODO: DEBUG
        ConfigDialog *dialog = new ConfigDialog(m_proxiedManager, nullptr);
        dialog->show();
    });

    // output mode
    m_outputModeReplaceAction = new QAction(QIcon(":/img/ins_replace.svg"),
                                            tr("Replace current text"), this);
    m_outputModeReplaceAction->setData(static_cast<int>(OutputMode::Replace));

    m_outputModeAppendAction = new QAction(QIcon(":/img/ins_append.svg"),
                                           tr("Append to text"), this);
    m_outputModeAppendAction->setData(static_cast<int>(OutputMode::Append));

    m_outputModeInsertAction = new QAction(QIcon(":/img/ins_insert.svg"),
                                           tr("Insert at cursor"), this);
    m_outputModeInsertAction->setData(static_cast<int>(OutputMode::Insert));

    m_outputModeMenu = new QMenu(this);
    m_outputModeMenu->addAction(m_outputModeReplaceAction);
    m_outputModeMenu->addAction(m_outputModeAppendAction);
    m_outputModeMenu->addAction(m_outputModeInsertAction);
    connect(m_outputModeMenu, &QMenu::triggered, [this](QAction *action) {
        m_outputMode = static_cast<OutputMode>(action->data().toInt());
        m_outputModeButton->setIcon(action->icon());
    });

    m_outputModeButton = new QToolButton;
    m_outputModeButton->setIcon(m_outputModeReplaceAction->icon());
    m_outputModeButton->setToolTip(tr("Select insert mode"));
    m_outputModeButton->setPopupMode(QToolButton::InstantPopup);
    m_outputModeButton->setMenu(m_outputModeMenu);

    // show preview
    m_showPreviewAction = new QAction(QIcon(":/img/markdown.svg"),
                                      tr("Show markdown preview"), this);
    m_showPreviewAction->setCheckable(true);
    m_showPreviewAction->setChecked(m_handler.previewVisible());

    connect(m_showPreviewAction, &QAction::toggled, [this] (bool checked) {
        m_view->setVisible(checked);
        m_splitter->refresh();
    });
}

void PonyOCR::initToolBar()
{
    QToolBar *toolBar = addToolBar(tr("MainToolBar"));
    toolBar->addAction(m_OCRAction);
    toolBar->addAction(m_configAction);
    toolBar->addSeparator();
    toolBar->addWidget(m_outputModeButton);
    toolBar->addAction(m_showPreviewAction);
}

void PonyOCR::initMarkdownPreview()
{
    // set parent of m_page to this to avoid problems with releasing profiles
    m_page = new PreviewPage(this);
    m_view = new QWebEngineView;
    m_view->setPage(m_page);
    m_view->setZoomFactor(1.2);
    m_view->setMinimumSize(400, 400);

    connect(m_textEdit, &QPlainTextEdit::textChanged,
            [this]() { m_content.setText(m_textEdit->toPlainText());
    });

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    m_page->setWebChannel(channel);
    m_view->setUrl(QUrl("qrc:/index-katex.html"));
}
