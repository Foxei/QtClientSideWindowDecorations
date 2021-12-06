/**
 * @file main.cpp
 * @authors Simon Schaefer
 * @date 05.05.2021
 */
#define APPLICATION_NAME "ExampleApplication"
#define ORGANIZATION_NAME "Simon Schäfer"
#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define REVISION 1

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFile>

#include "MainWindow.h"
void load_application_information()
{
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(QString(
        QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION) + "." + QString::number(REVISION)));
}

QString getStyleSheet()
{
    QFile file(":resources/style.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    return styleSheet;
}

int main(int argc, char *argv[])
{
    qInfo("[ExampleApplication] Executing %s, version %i.%i.%i", APPLICATION_NAME, MAJOR_VERSION, MINOR_VERSION, REVISION);
    load_application_information();

    QApplication application(argc, argv);

    QCommandLineParser commandLineParser;
    commandLineParser.setApplicationDescription("Example application for modern style in Qt5/6.");
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();
    commandLineParser.process(application);

    qInfo("[ExampleApplication] Executing Gui.");
    auto *mainWindow = new MainWindow;
    mainWindow->setWindowTitle(QApplication::applicationName());
    mainWindow->setStyleSheet(getStyleSheet());
    mainWindow->show();

    return QCoreApplication::exec();
}