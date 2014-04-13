// A class to handle downloading forecast data

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <memory>
#include "xmlparser.h"

class QNetworkAccessManager;
class QNetworkReply;
class QIODevice;
class QPlainTextEdit;
class QFile;

class DownloadManager : public QObject
{
    Q_OBJECT
  public:
    explicit DownloadManager(QObject *parent = 0);
    void doDownload(const QUrl &, QPlainTextEdit *);
    void saveToDisk(const QUrl &url);
    void startRequest(const QUrl &url);

  private
slots:
    void httpFinished();
    void httpReadyRead();

  private:
    QNetworkAccessManager *manager;
    QPlainTextEdit *textEdit;
    QString fileName;
    QNetworkReply *reply;
    QUrl currentUrl;
    QFile *file;
    std::unique_ptr<XmlParser> xmlParser;
};

#endif // DOWNLOADMANAGER_H
