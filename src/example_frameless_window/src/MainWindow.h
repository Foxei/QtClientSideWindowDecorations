#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "platform/UndecoratedWindow.h"

namespace Ui {
class MainWindow;
}

/**
 * Main class that communicates with all sub classes.
 */
class MainWindow : public UndecoratedWindow {
 Q_OBJECT
 private:
  Ui::MainWindow *ui; ///< Ui from .ui file
  QString base_stylesheet_;
  QMap<int, QColor> background_to_accent_map_;

 public:
  /**
   * Creates the instance, initialises the ui and connects the required bindings.
   * @param parent
   */
  explicit MainWindow(QWidget *parent = nullptr);

 protected:
  bool event(QEvent* event) override;

 private slots:
  void onUpdateBackgroundImage();
  void onUpdateVisibilityOfLeftSideNotch();
};

#endif // MAINWINDOW_H
