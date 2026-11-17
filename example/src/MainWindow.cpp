/**
 * @file MainWindow.cpp
 * @authors Simon Schaefer
 * @date 05.12.2021
 */

#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QEvent>
#include <QLabel>
#include "ColorDefinition.h"

void MainWindow::updateDynamicStyle(const QColor &accent, const QString &background_image) {
  // Set new stylesheet
  this->setStyleSheet(
      QString(""
              "QMainWindow {"
              "background-image: url(\":/resources/backgrounds/%1.svg\");"
              "background-repeat: repeat-xy;"
              "}").arg(background_image)
  );

  // Update palette colors
  QPalette darkPalette = QApplication::palette();
  darkPalette.setColor(QPalette::Highlight, accent);
  darkPalette.setColor(QPalette::BrightText, accent);
  darkPalette.setColor(QPalette::Link, accent);
  QApplication::setPalette(darkPalette);
}

MainWindow::MainWindow(QWidget *parent)
    : UndecoratedWindow(parent),
      ui(new Ui::MainWindow),
      background_to_accent_map_({{0, ColorDefinitions::GREEN},
                                 {1, ColorDefinitions::LIGHT_BLUE},
                                 {2, ColorDefinitions::BLUE},
                                 {3, ColorDefinitions::VIOLET},
                                 {4, ColorDefinitions::LIGHT_GREEN},
                                 {5, ColorDefinitions::VIOLET},
                                 {6, ColorDefinitions::LIGHT_GREEN},
                                 {7, ColorDefinitions::ORANGE}}) {

  // Setup ui
  this->ui->setupUi(this);
  this->centralWidget()->layout()->addWidget(this->ui->centralwidget);
  this->ui->notch_left_check->setChecked(this->titleBarDirection() == Qt::RightToLeft);
  // Some platforms (macOS) only support the controls on the left side
  this->ui->notch_left_check->setEnabled(UndecoratedWindow::SUPPORTS_RIGHT_SIDE_CONTROLS);

  // Show the capabilities of the decoration library on this platform
  this->ui->supports_right_side_controls_value_lbl->setText(
      UndecoratedWindow::SUPPORTS_RIGHT_SIDE_CONTROLS
      ? tr("Supported (SUPPORTS_RIGHT_SIDE_CONTROLS = true)")
      : tr("Not supported, native controls are always on the left (SUPPORTS_RIGHT_SIDE_CONTROLS = false)"));

  // Never shrink wrapped texts past their content
  for (QLabel *label : {this->ui->description_lbl, this->ui->supports_right_side_controls_value_lbl}) {
    label->installEventFilter(this);
  }

  // Connect actions
  connect(this->ui->background_image_list, &QListWidget::itemSelectionChanged, this, &MainWindow::onUpdateDynamicStyle);
  connect(this->ui->notch_left_check, &QCheckBox::toggled, this, &MainWindow::onUpdateControlNotchPosition);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
  auto *label = qobject_cast<QLabel *>(watched);
  if (label && label->wordWrap() && event->type() == QEvent::Resize) {
    label->setMinimumHeight(label->heightForWidth(label->width()));
  }
  return UndecoratedWindow::eventFilter(watched, event);
}

void MainWindow::onUpdateDynamicStyle() {
  // Get selected index and update style accordingly
  auto selected_index = this->ui->background_image_list->currentIndex().row();
  this->updateDynamicStyle(this->background_to_accent_map_[selected_index], QString::number(selected_index));

  // Force update of stylesheet
  this->repaint();
  qInfo("[MainWindow] Changed background to image number %i", selected_index);
}

void MainWindow::onUpdateControlNotchPosition() {
  // Check there the controls should be placed and update the parent accordingly
  bool checked = this->ui->notch_left_check->isChecked();
  this->setTitleBarDirection(checked ? Qt::RightToLeft : Qt::LeftToRight);
  qInfo("[MainWindow] Toggled control notch to the %s side", checked ? "left" : "right");
}
