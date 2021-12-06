#ifndef UNDECORATEDWINDOW_H_
#define UNDECORATEDWINDOW_H_

#include <QMainWindow>

namespace Ui {
class UndecoratedWindow;
}
class QWinWidget;

/**
 * Main class that communicates with all sub classes.
 */
class UndecoratedWindow : public QMainWindow {
 Q_OBJECT
 private:
  Ui::UndecoratedWindow *ui; ///< Ui from .ui file
#ifdef Q_OS_WINDOWS
  QWinWidget* win_widget;
#endif
  /**
   * Restores the window state form last usage.
   */
  void restoreWindowStates();

  /**
   * Stores the window state for later usage.
   */
  void storeWindowState();

 public:
  /**
   * Creates the instance, initialises the ui and connects the required bindings.
   * @param parent
   */
  explicit UndecoratedWindow(QWidget *parent = nullptr);

 protected:

  /**
   * Makes sure the current window state is stored.
   * @param event Possible reasons how the window was closed.
   */
  void closeEvent(QCloseEvent *event) override;

  void showEvent(QShowEvent* event) override;

  bool event(QEvent* event) override;

 public slots:

  void setTitleBarDirection(Qt::LayoutDirection direction);
  Qt::LayoutDirection titleBarDirection();

#ifdef Q_OS_WINDOWS
  friend QWinWidget;
#endif
};

#endif // UNDECORATEDWINDOW_H_
