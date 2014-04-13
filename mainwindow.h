#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QMap>
#include "downloadmanager.h"
#include "xmlparser.h"

class QComboBox;
class QNetworkReply;
class QString;
class QPlainTextEdit;
class QLabel;
class QNetworkReply;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  public
slots:
    void getWeatherForecast(QString);

  private:
    Ui::MainWindow *ui;

    void createWidgets();
    void createLayout();
    void populateComboBox();
    void createConnections();

    QComboBox *comboBox;
    QLabel *stateLabel;
    QPlainTextEdit *textEdit;
    std::unique_ptr<DownloadManager> manager;
    std::unique_ptr<XmlParser> xmlParser;
    QMap<QString, QString> states;
    QNetworkReply *reply;
};

#endif // MAINWINDOW_H
