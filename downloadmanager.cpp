#include "downloadmanager.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QFile>
#include <QUrl>
#include <iostream>
#include <QPlainTextEdit>
#include <QFileInfo>

DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    xmlParser = std::unique_ptr<XmlParser>{ new XmlParser };
    textEdit = new QPlainTextEdit;
}

void DownloadManager::doDownload(const QUrl &url, QPlainTextEdit *edit)
{

    textEdit = edit;
    saveToDisk(url);
    startRequest(url);
}

void DownloadManager::httpFinished()
{

    if (reply->error()) {
        std::cerr << "Download of " << currentUrl.toEncoded().constData()
                  << " failed: " << std::endl
                  << qPrintable(reply->errorString()) << std::endl;
        return;
    } else {
        std::cout << "Download of " << currentUrl.toEncoded().constData()
                  << " succeeded..." << std::endl;
    }

    file->flush();
    file->close();

    xmlParser->getWeatherData(file, textEdit);

    reply->deleteLater();
    if (file->remove())
        file = nullptr;
}

void DownloadManager::saveToDisk(const QUrl &url)
{

    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        // if already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    file = new QFile(basename);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        std::cerr << "Could not open " << qPrintable(fileName)
                  << " for writing: " << std::endl;
        std::cerr << qPrintable(file->errorString()) << std::cerr;

        if (file->remove())
            file = nullptr;
        return;
    }
}

void DownloadManager::startRequest(const QUrl &url)
{

    currentUrl = url;
    QNetworkRequest request(url);
    reply = manager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    std::cout << "Downloading " << currentUrl.toEncoded().constData()
              << std::endl;
}

void DownloadManager::httpReadyRead()
{
    // read all data, this could also be done in httpFinished slot but
    // in order to save memory it's done here
    if (file) {
        file->write(reply->readAll());
    }
}
