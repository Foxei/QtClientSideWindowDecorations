/**
 * @file MainWindow.h
 * @authors Simon Schaefer
 * @date 05.12.2021
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include <client_side_window_decoration/UndecoratedWindow.h>

namespace Ui {
class MainWindow;
}

/**
 * Main class that communicates with all sub classes.
 */
class MainWindow : public UndecoratedWindow {
 Q_OBJECT
 private:
  Ui::MainWindow *ui; ///< Ui from mainwindow.ui file
  const QMap<int, QColor> background_to_accent_map_; ///< Mapping to match the accent color to the background image.

  /**
   * Updates the stylesheet and palette according to the input.
   * @param accent Accent color.
   * @param background_image New background image name loaded from the application_resources.qrc.
   */
  void updateDynamicStyle(const QColor &accent, const QString &background_image);
 public:

  /**
   * Creates the instance, initialises the ui and connects the required bindings.
   * @param parent
   */
  explicit MainWindow(QWidget *parent = nullptr);

 public slots:
  /**
   * Updates style based on user input.
   */
  void onUpdateDynamicStyle();

  /**
   * Update controls position based on the users input. Does not correctly work on macOS as here the system controls
   * are used.
   */
  void onUpdateControlNotchPosition();

};

#endif // MAINWINDOW_H
