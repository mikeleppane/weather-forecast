#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <iostream>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QInputDialog>
#include <QLocale>
#include <QLabel>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = std::unique_ptr<DownloadManager>{ new DownloadManager };
    xmlParser = std::unique_ptr<XmlParser>{ new XmlParser };

    QLocale::setDefault(QLocale::English);

    createWidgets();
    createLayout();
    createConnections();

    QApplication::setApplicationName(tr("Weather Forecast"));

    setWindowTitle(QApplication::applicationName());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createWidgets()
{

    comboBox = new QComboBox(this);
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    stateLabel = new QLabel(this);
    stateLabel->setText(tr("Select a state"));
    stateLabel->setFont(QFont("Verdana", 12, QFont::Bold));
    stateLabel->setStyleSheet("QLabel{color: steelblue; }");

    textEdit = new QPlainTextEdit(this);
    textEdit->setVisible(true);
    textEdit->setReadOnly(true);
}

void MainWindow::createLayout()
{
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(comboBox);
    hLayout->addStretch(2);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(stateLabel);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(textEdit);

    QWidget *widget = new QWidget;
    widget->setLayout(vLayout);

    widget->setAttribute(Qt::WA_DeleteOnClose);
    setCentralWidget(widget);
}

void MainWindow::createConnections()
{
    connect(comboBox, SIGNAL(activated(QString)), this,
            SLOT(getWeatherForecast(QString)));

    populateComboBox();
}

void MainWindow::populateComboBox()
{
    // get all states and populate combobox
    states = xmlParser->getStateData();
    if (!states.isEmpty()) {
        int k = 0;
        for (const auto &item : states) {
            comboBox->insertItem(k, states.key(item));
            ++k;
        }
    }
}

void MainWindow::getWeatherForecast(QString text)
{
    // when user selects a state, provide a input dialog
    // where user can choose forecast days between 1 to 5 and
    // only these values can be chosen.
    bool ok;
    int number = QInputDialog::getInt(
        this, tr("Weather Forecast"),
        tr("Enter a forecast days: (current day == 1)"), 1, 1, 5, 1, &ok);
    QString urlForForecast("http://weather.yahooapis.com/forecastrss?w=" +
                           states.value(text) + "&u=c" + "&d=" +
                           QString::number(number));
    if (ok) {
        textEdit->clear();
        textEdit->setVisible(true);
        manager->doDownload(QUrl(urlForForecast), textEdit);
    }
}
