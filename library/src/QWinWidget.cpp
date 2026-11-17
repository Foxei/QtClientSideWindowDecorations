#include "QWinWidget.h"

#include <QApplication>
#include <QEvent>
#include <QFocusEvent>
#include <QWindow>
#include <QDebug>

#include "client_side_window_decoration/UndecoratedWindow.h"
#include "ui_undecoratedwindow.h"

namespace platform_specific {

QWinWidget::QWinWidget(UndecoratedWindow *undecorated_window)
    : QWidget(nullptr),
      layout_(),
      undecorated_window_(undecorated_window),
      window_native_window_handle_(nullptr),
      windows_previous_focus_native_window_handle_(nullptr),
      reenable_parent_(false) {

  setObjectName("windows");
  qDebug() << "QWinWidget" << undecorated_window_->geometry();
  //Create a native window and give it geometry values * devicePixelRatio for HiDPI support
  windows_native_window_ = new WinNativeWindow(undecorated_window_->geometry().x() * window()->devicePixelRatio(),
                                               undecorated_window_->geometry().y() * window()->devicePixelRatio(),
                                               undecorated_window_->geometry().width() * window()->devicePixelRatio(),
                                               undecorated_window_->geometry().height() * window()->devicePixelRatio());




  //Save the native window handle for shorthand use
  window_native_window_handle_ = windows_native_window_->hWnd;
  Q_ASSERT(window_native_window_handle_);


  //Create the child window & embed it into the native one
  if (window_native_window_handle_) {
    setWindowFlags(Qt::FramelessWindowHint);
    setProperty("_q_embedded_native_parent_handle", (WId) window_native_window_handle_);
    SetWindowLong((HWND) winId(), GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

    SetParent((HWND) winId(), window_native_window_handle_);
    QEvent e(QEvent::EmbeddingControl);
    QApplication::sendEvent(this, &e);
  }

  //Pass along our window handle & widget pointer to WinFramelessWidget so we can exchange messages
  windows_native_window_->childWindow = (HWND) winId();
  windows_native_window_->childWidget = this;

  //Clear margins & spacing & add the layout to prepare for the MainAppWidget
  setContentsMargins(0, 0, 0, 0);
  setLayout(&layout_);
  layout_.setContentsMargins(0, 0, 0, 0);
  layout_.setSpacing(0);

  //Create the true app widget
  layout_.addWidget(undecorated_window_);
  undecorated_window_->setParent(this, Qt::Widget);
  undecorated_window_->setVisible(true);

  //Update the effective_border_width value if needed for HiDPI displays
  effective_border_width = effective_border_width * window()->devicePixelRatio();

  //Update the effective_move_handle_height value to match the height of toolBar * if needed, the HiDPI display
//	if (p_Widget->toolBar)
//	{
//		effective_move_handle_height = p_Widget->toolBar->height() * window()->devicePixelRatio();
//	}


  //You need to keep the native window in sync with the Qt window & children, so wire min/max/close buttons to
  //slots inside of QWinWidget. QWinWidget can then talk with the native window as needed

  connect(undecorated_window_->ui->minimizeBtn, &QAbstractButton::clicked, this, &QWinWidget::onMinimizeButtonClicked);

  connect(undecorated_window_->ui->maximizeBtn, &QAbstractButton::clicked, this, &QWinWidget::onMaximizeButtonClicked);

  connect(undecorated_window_->ui->closeBtn, &QAbstractButton::clicked, this, &QWinWidget::onCloseButtonClicked);




  //Send the parent native window a WM_SIZE message to update the widget size
  SendMessage(window_native_window_handle_, WM_SIZE, 0, 0);

}

/*!
    Destroys this object, freeing all allocated resources.
*/
QWinWidget::~QWinWidget() {

}

/*!
    Returns the handle of the native Win32 parent window.
*/
HWND QWinWidget::getParentWindow() const {
  return window_native_window_handle_;
}

/*!
    \reimp
*/
void QWinWidget::childEvent(QChildEvent *e) {
  QObject *obj = e->child();
  if (obj->isWidgetType()) {
    if (e->added()) {
      if (obj->isWidgetType()) {
        obj->installEventFilter(this);
      }
    } else if (e->removed() && reenable_parent_) {
      reenable_parent_ = false;
      EnableWindow(window_native_window_handle_, true);
      obj->removeEventFilter(this);
    }
  }
  QWidget::childEvent(e);
}

/*! \internal */
void QWinWidget::saveFocus() {
  if (!windows_previous_focus_native_window_handle_)
    windows_previous_focus_native_window_handle_ = ::GetFocus();
  if (!windows_previous_focus_native_window_handle_)
    windows_previous_focus_native_window_handle_ = getParentWindow();
}

/*!
    Shows this widget. Overrides QWidget::show().

    \sa showCentered()
*/
void QWinWidget::show() {
  ShowWindow(window_native_window_handle_, true);
  saveFocus();
  QWidget::show();
}

/*!
    Centers this widget over the native parent window. Use this
    function to have Qt toplevel windows (i.e. dialogs) positioned
    correctly over their native parent windows.

    \code
    QWinWidget qwin(hParent);
    qwin.center();

    QMessageBox::information(&qwin, "Caption", "Information Text");
    \endcode

    This will center the message box over the client area of hParent.
*/
void QWinWidget::center() {
  const QWidget *child = findChild<QWidget *>();
  if (child && !child->isWindow()) {
    qWarning("QWinWidget::center: Call this function only for QWinWidgets with toplevel children");
  }
  RECT r;
  GetWindowRect(window_native_window_handle_, &r);
  setNativeGeometry((r.right - r.left) / 2 + r.left, (r.bottom - r.top) / 2 + r.top, 0, 0);
}

/*!
    \obsolete

    Call center() instead.
*/
void QWinWidget::showCentered() {
  center();
  show();
}

void QWinWidget::setNativeGeometry(int x, int y, int w, int h) {
  windows_native_window_->setGeometry(x * window()->devicePixelRatio(),
                                      y * window()->devicePixelRatio(),
                                      w * window()->devicePixelRatio(),
                                      h * window()->devicePixelRatio());
}

/*!
    Sets the focus to the window that had the focus before this widget
    was shown, or if there was no previous window, sets the focus to
    the parent window.
*/
void QWinWidget::resetFocus() {
  if (windows_previous_focus_native_window_handle_)
    ::SetFocus(windows_previous_focus_native_window_handle_);
  else
    ::SetFocus(getParentWindow());
}

//Tell the parent native window to minimize
void QWinWidget::onMinimizeButtonClicked() {
  SendMessage(window_native_window_handle_, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

//Tell the parent native window to maximize or restore as appropriate
void QWinWidget::onMaximizeButtonClicked() {
  if (undecorated_window_->ui->maximizeBtn->isChecked()) {
    SendMessage(window_native_window_handle_, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
  } else {
    SendMessage(window_native_window_handle_, WM_SYSCOMMAND, SC_RESTORE, 0);
  }
}

void QWinWidget::onCloseButtonClicked() {
  if (true /* put your check for it if it safe to close your app here */) //eg, does the user need to save a document
  {
    //Safe to close, so hide the parent window
    ShowWindow(window_native_window_handle_, false);

    //And then quit
    QApplication::quit();
  } else {
    //Do nothing, and thus, don't actually close the window
  }
}

bool QWinWidget::nativeEvent(const QByteArray &, void *message, qintptr *result) {
  MSG *msg = (MSG *) message;

  if (msg->message == WM_SETFOCUS) {
    Qt::FocusReason reason;
    if (::GetKeyState(VK_LBUTTON) < 0 || ::GetKeyState(VK_RBUTTON) < 0)
      reason = Qt::MouseFocusReason;
    else if (::GetKeyState(VK_SHIFT) < 0)
      reason = Qt::BacktabFocusReason;
    else
      reason = Qt::TabFocusReason;
    QFocusEvent e(QEvent::FocusIn, reason);
    QApplication::sendEvent(this, &e);
  }

  //Only close if safeToClose clears()
  if (msg->message == WM_CLOSE) {
    if (true /* put your check for it if it safe to close your app here */) //eg, does the user need to save a document
    {
      //Safe to close, so hide the parent window
      ShowWindow(window_native_window_handle_, false);

      //And then quit
      QApplication::quit();
    } else {
      *result = 0; //Set the message to 0 to ignore it, and thus, don't actually close
      return true;
    }
  }

  //Double check WM_SIZE messages to see if the parent native window is maximized
  if (msg->message == WM_SIZE) {

    //Get the window state
    WINDOWPLACEMENT wp;
    GetWindowPlacement(window_native_window_handle_, &wp);

    //If we're maximized,
    if (wp.showCmd == SW_MAXIMIZE) {
      //Maximize button should show as Restore
      undecorated_window_->ui->maximizeBtn->setChecked(true);
    } else {
      //Maximize button should show as Maximize
      undecorated_window_->ui->maximizeBtn->setChecked(false);
    }

  }

  //Pass NCHITTESTS on the window edges as determined by effective_border_width & effective_move_handle_height through to the parent native window
  if (msg->message == WM_NCHITTEST) {
    RECT WindowRect;
    int x, y;

    GetWindowRect(msg->hwnd, &WindowRect);
    x = GET_X_LPARAM(msg->lParam) - WindowRect.left;
    y = GET_Y_LPARAM(msg->lParam) - WindowRect.top;

    if (x >= effective_border_width && x <= WindowRect.right - WindowRect.left - effective_border_width
        && y >= effective_border_width
        && y <= effective_move_handle_height) {

      //If the mouse is over top of the toolbar area BUT is actually positioned over a child widget of the toolbar,
      //Then we don't want to enable dragging. This allows for buttons in the toolbar, eg, a Maximize button, to keep the mouse interaction
      if (QApplication::widgetAt(QCursor::pos()) != undecorated_window_->ui->titleBarContainer)
        return false;


      //The mouse is over the toolbar area & is NOT over a child of the toolbar, so pass this message
      //through to the native window for HTCAPTION dragging
      *result = HTTRANSPARENT;
      return true;

    } else if (x < effective_border_width && y < effective_border_width) {
      *result = HTTRANSPARENT;
      return true;
    } else if (x > WindowRect.right - WindowRect.left - effective_border_width && y < effective_border_width) {
      *result = HTTRANSPARENT;
      return true;
    } else if (x > WindowRect.right - WindowRect.left - effective_border_width
        && y > WindowRect.bottom - WindowRect.top - effective_border_width) {
      *result = HTTRANSPARENT;
      return true;
    } else if (x < effective_border_width && y > WindowRect.bottom - WindowRect.top - effective_border_width) {
      *result = HTTRANSPARENT;
      return true;
    } else if (x < effective_border_width) {
      *result = HTTRANSPARENT;
      return true;
    } else if (y < effective_border_width) {
      *result = HTTRANSPARENT;
      return true;
    } else if (x > WindowRect.right - WindowRect.left - effective_border_width) {
      *result = HTTRANSPARENT;
      return true;
    } else if (y > WindowRect.bottom - WindowRect.top - effective_border_width) {
      *result = HTTRANSPARENT;
      return true;
    }

    return false;
  }

  return false;
}

/*!
    \reimp
*/
bool QWinWidget::eventFilter(QObject *o, QEvent *e) {
  QWidget *w = (QWidget *) o;

  switch (e->type()) {
    case QEvent::WindowDeactivate:
      if (w->isModal() && w->isHidden())
        BringWindowToTop(window_native_window_handle_);
      break;

    case QEvent::Hide:
      if (reenable_parent_) {
        EnableWindow(window_native_window_handle_, true);
        reenable_parent_ = false;
      }
      resetFocus();

      if (w->testAttribute(Qt::WA_DeleteOnClose) && w->isWindow())
        deleteLater();
      break;

    case QEvent::Show:
      if (w->isWindow()) {
        saveFocus();
        hide();
        if (w->isModal() && !reenable_parent_) {
          EnableWindow(window_native_window_handle_, false);
          reenable_parent_ = true;
        }
      }
      break;

    case QEvent::Close: {
      ::SetActiveWindow(window_native_window_handle_);
      if (w->testAttribute(Qt::WA_DeleteOnClose))
        deleteLater();
      break;
    }
    default:break;
  }

  return QWidget::eventFilter(o, e);
}

/*! \reimp
*/
void QWinWidget::focusInEvent(QFocusEvent *e) {
  QWidget *candidate = this;

  switch (e->reason()) {
    case Qt::TabFocusReason:
    case Qt::BacktabFocusReason:
      while (!(candidate->focusPolicy() & Qt::TabFocus)) {
        candidate = candidate->nextInFocusChain();
        if (candidate == this) {
          candidate = 0;
          break;
        }
      }
      if (candidate) {
        candidate->setFocus(e->reason());
        if (e->reason() == Qt::BacktabFocusReason || e->reason() == Qt::TabFocusReason) {
          candidate->setAttribute(Qt::WA_KeyboardFocusChange);
          candidate->window()->setAttribute(Qt::WA_KeyboardFocusChange);
        }
        if (e->reason() == Qt::BacktabFocusReason)
          QWidget::focusNextPrevChild(false);
      }
      break;
    default:break;
  }
}

/*! \reimp
*/
bool QWinWidget::focusNextPrevChild(bool next) {
  QWidget *curFocus = focusWidget();
  if (!next) {
    if (!curFocus->isWindow()) {
      QWidget *nextFocus = curFocus->nextInFocusChain();
      QWidget *prevFocus = 0;
      QWidget *topLevel = 0;
      while (nextFocus != curFocus) {
        if (nextFocus->focusPolicy() & Qt::TabFocus) {
          prevFocus = nextFocus;
          topLevel = 0;
        }
        nextFocus = nextFocus->nextInFocusChain();
      }

      if (!topLevel) {
        return QWidget::focusNextPrevChild(false);
      }
    }
  } else {
    QWidget *nextFocus = curFocus;
    while (1 && nextFocus != 0) {
      nextFocus = nextFocus->nextInFocusChain();
      if (nextFocus->focusPolicy() & Qt::TabFocus) {
        return QWidget::focusNextPrevChild(true);
      }
    }
  }

  ::SetFocus(window_native_window_handle_);

  return true;
}
void QWinWidget::updateMinimumAndMaximumSize() {
  //If you want to set a minimize size for your app, do so here
  windows_native_window_->setMinimumSize(
      undecorated_window_->centralWidget()->minimumWidth() * window()->devicePixelRatio(),
      undecorated_window_->centralWidget()->minimumHeight() * window()->devicePixelRatio());

  //If you want to set a maximum size for your app, do so here
  windows_native_window_->setMaximumSize(
      undecorated_window_->centralWidget()->maximumWidth() * window()->devicePixelRatio(),
      undecorated_window_->centralWidget()->maximumHeight() * window()->devicePixelRatio());

}
QRect QWinWidget::nativeGeometry() {
  QRect rect;
  auto native_rect = windows_native_window_->getGeometry();
  rect.setX(native_rect.left);
  rect.setY(native_rect.top);
  rect.setWidth(native_rect.right - native_rect.left);
  rect.setHeight(native_rect.bottom - native_rect.top);
  return rect;
}

}