#ifndef CONFIGWIDGIT_H
#define CONFIGWIDGIT_H

#include <QDialog>
#include "confighandler.h"

class QDialogButtonBox;
class QTabWidget;
class APIConfigWidget;
class GeneralConfigWidget;
class ProxyConfigWidget;
class QNetworkAccessManager;

class ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConfigDialog(QNetworkAccessManager *proxiedManager,
                          QWidget *parent = nullptr);

private:
    QTabWidget *m_tab;
    GeneralConfigWidget *m_generalWidget;
    APIConfigWidget *m_baiduWidget,
                    *m_tencentWidget,
                    *m_ocrspaceWidget,
                    *m_mathpixWidget;
    ProxyConfigWidget *m_proxyWidget;
    QDialogButtonBox *m_buttonBox;
    ConfigHandler m_handler;

    void createButtonBox();
    void createBaiduWidget();
    void createTencentWidget();
    void createOCRSpaceWidget();
    void createMathpixWidget();

public slots:
    void saveConfig();

protected:
    void keyPressEvent(QKeyEvent *event) override;
};
#endif // CONFIGWIDGIT_H
