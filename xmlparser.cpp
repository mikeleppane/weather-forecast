#include "xmlparser.h"
#include <QFile>
#include <iostream>
#include <QDebug>
#include <QString>
#include <QPlainTextEdit>
#include <QDate>
#include <QLocale>

XmlParser::XmlParser(QObject *parent) : QObject(parent)
{
    // a states file is in store to resource file because
    // the given web link does not work properly and
    // the program is not able to make the necessary download.
    // For the parsing I'm using QDomDocument because it's
    // much more intuitive(at least for me) than the QXmlStreamReader
    // and it also first checks the validity of a file content.
    // The penalty for the decision is slightly lower performance and
    // higher memory usage.
}

bool XmlParser::readFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        std::cerr << "Error: Cannot read file " << qPrintable(filename) << ": "
                  << qPrintable(file.errorString()) << std::endl;
        return false;
    }

    return true;
}

void XmlParser::getWeatherData(QFileDevice *file, QPlainTextEdit *text)
{
    forecastText.clear();
    forecastList.clear();
    if (readFile(file->fileName())) {
        parseForecastFile(file, text);
    }
}

QMap<QString, QString> XmlParser::getStateData()
{
    QMap<QString, QString> states;

    QFile file(":/xml/states.xml");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        std::cerr << "Error: Cannot read file " << qPrintable(file.fileName())
                  << ": " << qPrintable(file.errorString()) << std::endl;
        return states;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        std::cerr << "Error: Parse error at line " << errorLine << ", "
                  << "column " << errorColumn << ": " << qPrintable(errorStr)
                  << std::endl;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (root.tagName() == "query") {
        QDomElement element = root.firstChildElement("results");
        if (!element.isNull()) {
            QDomNode childNode = element.firstChild();
            while (!childNode.isNull()) {
                if (childNode.toElement().tagName() == "place") {
                    states.insert(
                        childNode.toElement().firstChildElement("name").text(),
                        childNode.firstChildElement("woeid").text());
                }
                childNode = childNode.nextSibling();
            }
        }
        return states;

    } else {
        std::cerr << "Not a valid file..." << std::endl;
        return states;
    }
}

void XmlParser::parseForecastFile(QFileDevice *file, QPlainTextEdit *textEdit)
{

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;

    if (!doc.setContent(file, false, &errorStr, &errorLine, &errorColumn)) {
        std::cerr << "Error: Parse error at line " << errorLine << ", "
                  << "column " << errorColumn << ": " << qPrintable(errorStr)
                  << std::endl;
        return;
    }
    file->close();

    QDomElement root = doc.documentElement();
    if (root.tagName() != "rss") {
        std::cerr << "Error: Not a states rss file" << std::endl;
        return;
    } else {
        QDomElement element = root.firstChildElement("channel");
        if (!element.isNull()) {
            QDomNode childNode = element.firstChild();
            while (!childNode.isNull()) {
                if (childNode.toElement().tagName() == "item") {
                    parseForecastItemElement(childNode.toElement());
                } else if (childNode.toElement().tagName() ==
                           "yweather:astronomy") {
                    QLocale loc;
                    forecastText =
                        "Current day: " +
                        loc.toString(QDate::currentDate(), "d MMMM yyyy") +
                        "\n\n";
                    forecastText += "Sunrise: " +
                                    childNode.toElement().attribute("sunrise") +
                                    "\n\n";
                    forecastText += "Sunset: " +
                                    childNode.toElement().attribute("sunset") +
                                    "\n\n";
                }
                childNode = childNode.nextSibling();
            }
        }

        int k = 0;
        for (const auto &item : forecastList) {
            if (k == 0) {
                forecastText +=
                    "High Temp: " + QString::number(item.high) + "\n\n";
                forecastText +=
                    "Low Temp: " + QString::number(item.low) + "\n\n";
                forecastText += "Text: " + item.desc + "\n\n";
            } else {
                QLocale loc;
                QString longDayName =
                    loc.toString(QDate::currentDate().addDays(k), "dddd");
                longDayName =
                    longDayName.replace(0, 1, longDayName.at(0).toUpper());
                forecastText += "Date: " + item.date + ",\n";
                forecastText += "Day: " + item.day + "/" + longDayName + ",\n";
                forecastText +=
                    "High Temp: " + QString::number(item.high) + ",\n";
                forecastText +=
                    "Low Temp: " + QString::number(item.low) + ",\n";
                forecastText += "Text: " + item.desc + "\n\n";
            }
            k++;
            textEdit->setPlainText(forecastText);
        }
    }
}

void XmlParser::parseForecastItemElement(const QDomElement &elem)
{
    QDomNode node = elem.firstChild();
    while (!node.isNull()) {
        if (node.toElement().tagName() == "yweather:forecast") {
            QDomNamedNodeMap map = node.toElement().attributes();
            Forecast fc;
            fc.desc = map.namedItem("text").nodeValue();
            fc.high = map.namedItem("high").nodeValue().toInt();
            fc.low = map.namedItem("low").nodeValue().toInt();
            fc.date = map.namedItem("date").nodeValue();
            fc.day = map.namedItem("day").nodeValue();
            forecastList.append(fc);
        }
        node = node.nextSibling();
    }
}
