// A class to parse weather forecast data

#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QObject>
#include <QDomDocument>
#include <memory>
#include <QList>
#include <QMap>

class QFileDevice;
class QPlainTextEdit;
class QString;

struct Forecast
{
    QString day;
    QString date;
    int low;
    int high;
    QString desc;
};

class XmlParser : public QObject
{
    Q_OBJECT
  public:
    explicit XmlParser(QObject *parent = 0);

    bool readFile(const QString &fileName);
    QMap<QString, QString> getStateData();
    void getWeatherData(QFileDevice *, QPlainTextEdit *);

  private:
    void parseForecastFile(QFileDevice *, QPlainTextEdit *);
    void parseForecastItemElement(const QDomElement &);

    QString forecastText;
    QDomDocument doc;
    QList<Forecast> forecastList;
};

#endif // XMLPARSER_H
