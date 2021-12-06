#include "UndecoratedWindow.h"

#include <QApplication>
#include <QWindow>
#include <QSettings>
#include <QWindowStateChangeEvent>
#include <QDebug>

#ifdef Q_OS_DARWIN
#include "platform/OSXHideTitleBar.h"
#endif

#define RESIZE_HANDLES_WIDTH 4
#define MOVE_HANDLES_HEIGHT 20

void UndecoratedWindow::restoreWindowStates() {
  QSettings settings(QApplication::organizationName(), QApplication::applicationName(), this);
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("window_state").toByteArray());
}

void UndecoratedWindow::storeWindowState() {
  QSettings settings(QApplication::organizationName(), QApplication::applicationName(), this);
  settings.setValue("geometry", saveGeometry());
  settings.setValue("window_state", saveState());
}

UndecoratedWindow::UndecoratedWindow(QWidget *parent) : QMainWindow(parent) {
  setAttribute(Qt::WA_TranslucentBackground, true);
  setAttribute(Qt::WA_NoSystemBackground, false);

  setWindowFlag(Qt::FramelessWindowHint);

  this->restoreWindowStates();
  this->setMouseTracking(true);
}

void UndecoratedWindow::closeEvent(QCloseEvent * /*event*/) {
  storeWindowState();
}

void UndecoratedWindow::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
#ifdef Q_OS_DARWIN
  OSXHideTitleBar::Windowed(this->winId());
#endif
}
bool UndecoratedWindow::event(QEvent *event) {
  QMainWindow::event(event);
  if (event->type() == QEvent::WindowStateChange) {
    auto *state_change_event = dynamic_cast<QWindowStateChangeEvent *>(event);
    if (this->windowState() == Qt::WindowState::WindowFullScreen) {
#ifdef Q_OS_DARWIN
      OSXHideTitleBar::FullScreen(this->winId());
#endif
    } else {
#ifdef Q_OS_DARWIN
      OSXHideTitleBar::Windowed(this->winId());
#endif
    }
  } else if (event->type() == QEvent::HoverMove) {
    auto *hover_event = dynamic_cast<QHoverEvent *>(event);
    QPoint position = hover_event->pos();
    if (position.x() > this->width() - RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeHorCursor);
      qDebug() << "Right";
    } else if (position.x() < RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeHorCursor);
      qDebug() << "Left";
    } if (position.y() > this->height() - RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeVerCursor);
      qDebug() << "Bottom";
    } else if (position.y() < RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeVerCursor);
      qDebug() << "Top";
    }else if (position.y() < MOVE_HANDLES_HEIGHT) {
      this->setCursor(Qt::ArrowCursor);
      qDebug() << "Top";
    }
  }else if(event->type() == QEvent::HoverLeave){
    this->setCursor(Qt::ArrowCursor);
  }else if(event->type() == QEvent::MouseButtonPress){
    auto *mouse_event = dynamic_cast<QMouseEvent *>(event);
    QPoint position = mouse_event->pos();
    if (position.x() > this->width() - RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::RightEdge);
      this->setCursor(Qt::SizeHorCursor);
      qDebug() << "Right";
    } else if (position.x() < RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::LeftEdge);
      this->setCursor(Qt::SizeHorCursor);
      qDebug() << "Left";
    } if (position.y() > this->height() - RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::BottomEdge);
      this->setCursor(Qt::SizeVerCursor);
      qDebug() << "Bottom";
    } else if (position.y() < RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::TopEdge);
      this->setCursor(Qt::SizeVerCursor);
      qDebug() << "Top";
    }else if (position.y() < MOVE_HANDLES_HEIGHT) {
      this->windowHandle()->startSystemMove();
      this->setCursor(Qt::ArrowCursor);
      qDebug() << "Top";
    }
    this->setCursor(Qt::ArrowCursor);
  }
//  qDebug() << event;

  return false;
}
