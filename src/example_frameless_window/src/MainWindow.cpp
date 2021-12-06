#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <utility/ColorDefinition.h>

void load_style(const QColor &accent) {
  QApplication::setWindowIcon(QIcon(":/resources/logo.png"));

  QApplication::setStyle("Fusion");

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

  darkPalette.setColor(QPalette::Highlight, accent);
  darkPalette.setColor(QPalette::HighlightedText, Qt::white);

  darkPalette.setColor(QPalette::BrightText, accent);
  darkPalette.setColor(QPalette::Link, accent);
  QApplication::setPalette(darkPalette);
}

MainWindow::MainWindow(QWidget *parent)
    : UndecoratedWindow(parent), ui(new Ui::MainWindow) {
  // Load default style
  load_style(ColorDefinitions::GREEN);
  this->background_to_accent_map_.insert(0, ColorDefinitions::GREEN);
  this->background_to_accent_map_.insert(1, ColorDefinitions::LIGHT_BLUE);
  this->background_to_accent_map_.insert(2, ColorDefinitions::BLUE);
  this->background_to_accent_map_.insert(3, ColorDefinitions::VIOLET);
  this->background_to_accent_map_.insert(4, ColorDefinitions::LIGHT_GREEN);
  this->background_to_accent_map_.insert(5, ColorDefinitions::VIOLET);
  this->background_to_accent_map_.insert(6, ColorDefinitions::LIGHT_GREEN);
  this->background_to_accent_map_.insert(7, ColorDefinitions::ORANGE);

  this->ui->setupUi(this);
  this->centralWidget()->layout()->addWidget(this->ui->centralwidget);
  connect(this->ui->background_image_list, &QListWidget::itemSelectionChanged,
          this, &MainWindow::onUpdateBackgroundImage);
  connect(this->ui->notch_left_check, &QCheckBox::stateChanged,
          this, &MainWindow::onUpdateVisibilityOfLeftSideNotch);

  this->ui->notch_left_check->setChecked(this->titleBarDirection() == Qt::RightToLeft);
}

void MainWindow::onUpdateBackgroundImage() {
  if (this->base_stylesheet_.isEmpty()) {
    this->base_stylesheet_ = this->styleSheet();
  }
  auto selected_index = this->ui->background_image_list->selectionModel()->selectedIndexes().first().row();
  load_style(this->background_to_accent_map_[selected_index]);

  qInfo("[MainWindow] Changing background to image number %i", selected_index);
  this->setStyleSheet(this->base_stylesheet_ + QString(
      "QMainWindow {background-image: url(\":/resources/backgrounds/%1.svg\");background-repeat: repeat-xy;}")
      .arg(selected_index));
}

void MainWindow::onUpdateVisibilityOfLeftSideNotch() {
  bool checked = this->ui->notch_left_check->isChecked();
  this->setTitleBarDirection(checked?Qt::RightToLeft:Qt::LeftToRight);
  qInfo("[MainWindow] Toggling control notch");
}

bool MainWindow::event(QEvent *event) {
  UndecoratedWindow::event(event);
  if (event->type() == QEvent::WindowStateChange) {
//    this->ui->notch_left_check->setChecked(this->windowState() != Qt::WindowState::WindowFullScreen);
  }
  return false;
}
