/**
 * @file main.cpp
 * @authors Simon Schaefer
 * @date 05.12.2021
 */
#define APPLICATION_NAME "ExampleClientSideDecoration"
#define ORGANIZATION_NAME "Simon Schäfer"
#define MAJOR_VERSION 0
#define MINOR_VERSION 1

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFile>

#include "ColorDefinition.h"
#include "MainWindow.h"

QString getStyleSheet() {
  // Parse stylesheet from resources file
  QFile file(":resources/application_style.qss");
  file.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(file.readAll());
  return styleSheet;
}

void load_static_style(QApplication *application) {
  QApplication::setWindowIcon(QIcon(":/resources/logo.png"));
  QApplication::setStyle("Fusion");

  application->setStyleSheet(getStyleSheet());

  QPalette darkPalette = QApplication::palette();
  darkPalette.setColor(QPalette::Window, Qt::black);
  darkPalette.setColor(QPalette::WindowText, Qt::white);

  darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
  darkPalette.setColor(QPalette::AlternateBase, QColor(75, 75, 75));

  darkPalette.setColor(QPalette::ToolTipBase, QColor(251, 211, 23));
  darkPalette.setColor(QPalette::ToolTipText, Qt::black);

  darkPalette.setColor(QPalette::Text, Qt::white);
  darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(180, 180, 180));
  darkPalette.setColor(QPalette::PlaceholderText, QColor(200, 200, 200));
  darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(180, 180, 180));

  darkPalette.setColor(QPalette::Button, QColor(25, 25, 25));
  darkPalette.setColor(QPalette::Disabled, QPalette::Button, QColor(10, 10, 10));

  darkPalette.setColor(QPalette::Dark, QColor(10, 10, 10));
  darkPalette.setColor(QPalette::Mid, QColor(30, 30, 30));
  darkPalette.setColor(QPalette::Midlight, QColor(50, 50, 50));
  darkPalette.setColor(QPalette::Light, QColor(70, 70, 70));

  darkPalette.setColor(QPalette::ButtonText, Qt::white);
  darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(180, 180, 180));

  darkPalette.setColor(QPalette::HighlightedText, Qt::white);
  darkPalette.setColor(QPalette::Highlight, ColorDefinitions::ORANGE);
  darkPalette.setColor(QPalette::BrightText, ColorDefinitions::ORANGE);
  darkPalette.setColor(QPalette::Link, ColorDefinitions::ORANGE);
  QApplication::setPalette(darkPalette);
}

void load_application_information() {
  QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
  QCoreApplication::setApplicationName(APPLICATION_NAME);
  QCoreApplication::setApplicationVersion(QString(
      QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION)));
}

int main(int argc, char *argv[]) {
  qInfo("[ExampleClientSideDecoration] Executing %s, version %i.%i", APPLICATION_NAME, MAJOR_VERSION, MINOR_VERSION);
  load_application_information();

  QApplication application(argc, argv);
  QCommandLineParser commandLineParser;
  commandLineParser
      .setApplicationDescription("Example of client side decorations for modern user interfaces in Qt5.15");
  commandLineParser.addHelpOption();
  commandLineParser.addVersionOption();
  commandLineParser.process(application);

  qInfo("[ExampleClientSideDecoration] Load static style information.");
  load_static_style(&application);

  qInfo("[ExampleClientSideDecoration] Executing Gui.");
  auto *mainWindow = new MainWindow;
  mainWindow->setWindowTitle(QApplication::applicationName());
  mainWindow->show();

  qInfo("[ExampleClientSideDecoration] Update dynamic style information.");
  mainWindow->onUpdateDynamicStyle();

  return QCoreApplication::exec();
}