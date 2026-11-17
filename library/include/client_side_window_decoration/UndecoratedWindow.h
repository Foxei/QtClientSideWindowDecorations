#ifndef UNDECORATEDWINDOW_H_
#define UNDECORATEDWINDOW_H_

#include <QMainWindow>

#include "client_side_window_decoration/client_side_window_decoration_export.h"

namespace Ui {
class UndecoratedWindow;
}
namespace platform_specific {
class QWinWidget;
}

/**
 * Undecorated window is a QMainWindows that allow full control other the window
 * decorations but keeps all platform  specific features like aero snap or macOS
 * full screen. It uses native code for windows and macOS and Qt 6 System
 * calls for linux. Us it as you would normally would with a QMainWindow, but do
 * not load a second QMainWindow from a ui file. To add Components add the to
 * the central widget.
 */
class CLIENT_SIDE_WINDOW_DECORATION_EXPORT UndecoratedWindow : public QMainWindow {
 Q_OBJECT
 private:
  Ui::UndecoratedWindow *ui; ///< Ui file containing the decoration and title bar

#if defined(Q_OS_WINDOWS)
  platform_specific::QWinWidget* win_widget; ///< Widget to control the native Windows window
#endif

  /**
   * Restores the window state and geometry form last usage.
   */
  void restoreWindowStatesAndGeometry();

  /**
   * Stores the window state and geometry for later usage.
   */
  void storeWindowStateAndGeometry();

#if defined(Q_OS_DARWIN)
  /**
   * Fits the background of the controls around the native macOS window buttons.
   */
  void updateMacOSControlsGeometry();
#endif

 public:
  /**
   * Whether the window controls can be placed on the right side of the title
   * bar. On macOS the native controls are used, which are always on the left.
   */
#if defined(Q_OS_DARWIN)
  static constexpr bool SUPPORTS_RIGHT_SIDE_CONTROLS = false;
#else
  static constexpr bool SUPPORTS_RIGHT_SIDE_CONTROLS = true;
#endif

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

  void showEvent(QShowEvent *event) override;

  bool event(QEvent *event) override;

 public slots:

  void setTitleBarDirection(Qt::LayoutDirection direction);
  Qt::LayoutDirection titleBarDirection();

#ifdef Q_OS_WINDOWS
  friend class platform_specific::QWinWidget;
#endif
  void setOsSpecificObjectName();
};

#endif // UNDECORATEDWINDOW_H_
