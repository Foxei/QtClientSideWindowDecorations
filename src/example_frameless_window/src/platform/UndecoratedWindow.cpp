#include "UndecoratedWindow.h"
#include "ui_undecoratedwindow.h"
#include "QWinWidget.h"

#include <QApplication>
#include <QWindow>
#include <QSettings>
#include <QWindowStateChangeEvent>
#include <QStyle>
#include <QAbstractButton>
#include <QDebug>

#include "platform/NativeDefinitions.h"

#ifdef Q_OS_DARWIN
#include "platform/OSXHideTitleBar.h"
#endif

QString getStyleSheet() {
  QFile file(":resources/style.qss");
  file.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(file.readAll());
  return styleSheet;
}

void UndecoratedWindow::restoreWindowStates() {
  QSettings settings
      (QApplication::organizationName(), QApplication::applicationName(), this);
  restoreState(settings.value("window_state").toByteArray());
  restoreGeometry(settings.value("geometry").toByteArray());
#ifdef Q_OS_WINDOWS
  setGeometry(settings.value("geometry_windows").toRect());
#endif
  qDebug() << "Restore" << this->geometry();
}

void UndecoratedWindow::storeWindowState() {
  qDebug() << "Store" << this->win_widget->nativeGeometry();

  QSettings settings
      (QApplication::organizationName(), QApplication::applicationName(), this);
  settings.setValue("window_state", saveState());
  settings.setValue("geometry", saveGeometry());
#ifdef Q_OS_WINDOWS
  settings.setValue("geometry_windows", this->win_widget->nativeGeometry());
#endif
}

UndecoratedWindow::UndecoratedWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::UndecoratedWindow) {
  setAttribute(Qt::WA_TranslucentBackground, true);
  setAttribute(Qt::WA_NoSystemBackground, false);

#ifdef Q_OS_UNIX
  this->setObjectName("linux");
#endif
#ifdef Q_OS_WINDOWS
  this->setObjectName("windows");
#endif

  this->setMouseTracking(true);
  this->ui->setupUi(this);
  this->setStyleSheet(getStyleSheet());

#ifdef Q_OS_DARWIN
  this->ui->closeBtn->hide();
  this->ui->minimizeBtn->hide();
  this->ui->maximizeBtn->hide();
  setTitleBarDirection(Qt::RightToLeft);
#else
  this->setWindowFlag(Qt::FramelessWindowHint);
  this->setTitleBarDirection(Qt::LeftToRight);
#endif
  connect(this->ui->closeBtn, &QAbstractButton::clicked, this, &QMainWindow::close);

#ifdef Q_OS_UNIX
  connect(this->ui->maximizeBtn, &QAbstractButton::clicked, [this](){
    this->isMaximized() ? this->showNormal() :  this->showMaximized();
  });
  connect(this->ui->minimizeBtn, &QAbstractButton::clicked, this, &QMainWindow::showMinimized);
#endif
  this->restoreWindowStates();
#ifdef Q_OS_WINDOWS
  win_widget = new QWinWidget(this);
  win_widget->show();
#endif
}

void UndecoratedWindow::closeEvent(QCloseEvent * /*event*/) {
  storeWindowState();
}

void UndecoratedWindow::showEvent(QShowEvent *event) {
  QMainWindow::showEvent(event);
#ifdef Q_OS_DARWIN
  OSXHideTitleBar::Windowed(this->winId());
#endif
#ifdef Q_OS_WINDOWS
  win_widget->updateMinimumAndMaximumSize();
#endif
}
bool UndecoratedWindow::event(QEvent *event) {
  QMainWindow::event(event);
  if (event->type() == QEvent::WindowStateChange) {
    auto *state_change_event = dynamic_cast<QWindowStateChangeEvent *>(event);
    if (this->windowState() == Qt::WindowState::WindowFullScreen) {
#ifdef Q_OS_DARWIN
      OSXHideTitleBar::FullScreen(this->winId());
      this->ui->controlsContainerRight->hide();
#endif
    } else {
#ifdef Q_OS_DARWIN
      OSXHideTitleBar::Windowed(this->winId());
      this->ui->controlsContainerRight->show();
#endif
    }
  } else if (event->type() == QEvent::HoverMove) {
#ifdef Q_OS_UNIX
    auto *hover_event = dynamic_cast<QHoverEvent *>(event);
    QPoint position = hover_event->pos();
    if (position.x() > this->width() - RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeHorCursor);
      qDebug() << "Right";
    } else if (position.x() < RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeHorCursor);
      qDebug() << "Left";
    }
    if (position.y() > this->height() - RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeVerCursor);
      qDebug() << "Bottom";
    } else if (position.y() < RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeVerCursor);
      qDebug() << "Top";
    } else if (position.y() < MOVE_HANDLES_HEIGHT) {
      this->setCursor(Qt::ArrowCursor);
      qDebug() << "Top";
    }
  } else if (event->type() == QEvent::HoverLeave) {
    this->setCursor(Qt::ArrowCursor);
  } else if (event->type() == QEvent::MouseButtonPress) {
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
    }
    if (position.y() > this->height() - RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::BottomEdge);
      this->setCursor(Qt::SizeVerCursor);
      qDebug() << "Bottom";
    } else if (position.y() < RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::TopEdge);
      this->setCursor(Qt::SizeVerCursor);
      qDebug() << "Top";
    } else if (position.y() < MOVE_HANDLES_HEIGHT) {
      this->windowHandle()->startSystemMove();
      this->setCursor(Qt::ArrowCursor);
      qDebug() << "Top";
    }
    this->setCursor(Qt::ArrowCursor);
#endif
  }
//  qDebug() << event;

  return false;
}

void UndecoratedWindow::setTitleBarDirection(Qt::LayoutDirection direction) {
  this->ui->titleBarContainer->setLayoutDirection(direction);
  this->ui->controlsContainerRight->setLayoutDirection(direction);
  if (direction == Qt::LeftToRight) {
    this->ui->controlsContainerRight->setObjectName("controlsContainerRight");
  } else {
    this->ui->controlsContainerRight->setObjectName("controlsContainerLeft");
  }
  this->style()->polish(this->ui->controlsContainerRight);
}

Qt::LayoutDirection UndecoratedWindow::titleBarDirection(){
    return this->ui->titleBarContainer->layoutDirection();
}
