#ifndef UNDECORATEDWINDOW_H_
#define UNDECORATEDWINDOW_H_

#include <QMainWindow>

/**
 * Main class that communicates with all sub classes.
 */
class UndecoratedWindow : public QMainWindow {
 Q_OBJECT

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
};

#endif // UNDECORATEDWINDOW_H_