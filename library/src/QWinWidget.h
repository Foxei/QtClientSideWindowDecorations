#ifndef QWINWIDGET_H
#define QWINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "WinNativeWindow.h"
#include "NativeDefinitions.h"

class UndecoratedWindow;

namespace platform_specific {

class QWinWidget : public QWidget {
 Q_OBJECT
 private:
  QVBoxLayout layout_;

  UndecoratedWindow *undecorated_window_;

  WinNativeWindow *windows_native_window_;
  HWND window_native_window_handle_;

  HWND windows_previous_focus_native_window_handle_;
  bool reenable_parent_;

  int effective_border_width = RESIZE_HANDLES_WIDTH;
  int effective_move_handle_height = MOVE_HANDLES_HEIGHT;

  void saveFocus();
  void resetFocus();

 public:
  QWinWidget(UndecoratedWindow *undecorated_window);
  ~QWinWidget();

  void show();
  void center();
  void showCentered();
  void setNativeGeometry(int x, int y, int w, int h);
  QRect nativeGeometry();
  HWND getParentWindow() const;
  void updateMinimumAndMaximumSize();

 public slots:
  void onMaximizeButtonClicked();
  void onMinimizeButtonClicked();
  void onCloseButtonClicked();

 protected:
  void childEvent(QChildEvent *e) override;
  bool eventFilter(QObject *o, QEvent *e) override;

  bool focusNextPrevChild(bool next) override;
  void focusInEvent(QFocusEvent *e) override;

  bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
};
}
#endif // QWINWIDGET_H
