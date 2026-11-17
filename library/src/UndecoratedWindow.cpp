#include "client_side_window_decoration/UndecoratedWindow.h"
#include "ui_undecoratedwindow.h"

#include <QApplication>
#include <QFile>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QWindow>
#include <QSettings>
#include <QWindowStateChangeEvent>
#include <QStyle>
#include <QAbstractButton>
#include <QDebug>


#if defined(Q_OS_DARWIN)
#include "OSXHideTitleBar.h"
#include <QMargins>

// Space between the native macOS window buttons and the edge of their background
static const QMargins MACOS_CONTROLS_PADDING(8, 6, 8, 6);
#elif defined(Q_OS_WINDOWS)
#include "QWinWidget.h"
#elif defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
#include "NativeDefinitions.h"
#endif

QString getStyleSheet() {
  // Parse stylesheet from resources file
  QFile file(":resources/decoration_style.qss");
  if (!file.open(QFile::ReadOnly)) {
    qWarning("Could not load decoration_style.qss");
    return {};
  }
  QString styleSheet = QString::fromUtf8(file.readAll());
  return styleSheet;
}

void UndecoratedWindow::restoreWindowStatesAndGeometry() {
  // Load user space settings file
  QSettings settings(QApplication::organizationName(), QApplication::applicationName(), this);
  // Restore if state is full screen or windowed
  restoreState(settings.value("window_state").toByteArray());
  // Restore window geometry
#ifdef Q_OS_WINDOWS
  // For native Windows widget
  setGeometry(settings.value("geometry").toRect());
//  qDebug() << "Restore window geometry: " << this->geometry();
#else
  // For QMainWindow
  restoreGeometry(settings.value("geometry").toByteArray());
//  qDebug() << "Restore window geometry: " << this->geometry();
#endif
}

void UndecoratedWindow::storeWindowStateAndGeometry() {
  // Load user space settings file
  QSettings settings(QApplication::organizationName(), QApplication::applicationName(), this);
  // Store if state is full screen or windowed
  settings.setValue("window_state", saveState());
  // Store window geometry
#if defined(Q_OS_WINDOWS)
  // Pull form native Windows window
  settings.setValue("geometry", this->win_widget->nativeGeometry());
//  qDebug() << "Store window geometry: " << this->win_widget->nativeGeometry();
#else
  // Pull from QMainWindow
  settings.setValue("geometry", saveGeometry());
//  qDebug() << "Store window geometry: " << this->geometry();
#endif
}

void UndecoratedWindow::setOsSpecificObjectName() {
  // Set object name to allow platform dependent qss styling
#if defined(Q_OS_DARWIN)
  setObjectName("macos");
#elif defined(Q_OS_WINDOWS)
  this->setObjectName("windows");
#elif defined(Q_OS_UNIX)
    this->setObjectName("linux");
#endif
}

UndecoratedWindow::UndecoratedWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::UndecoratedWindow) {

  // Make round corners on Windows and Ubuntu possible by removing background
  setAttribute(Qt::WA_TranslucentBackground, true);
  setAttribute(Qt::WA_NoSystemBackground, false);

  // Set object name to allow platform dependent qss styling
  setOsSpecificObjectName();

  // Load decoration from
  this->ui->setupUi(this);
  this->setStyleSheet(getStyleSheet());

#if defined(Q_OS_DARWIN)
  // The content is drawn below the transparent title bar, do not let Qt push
  // it down by the safe area of the native title bar
  for (QWidget *widget : {static_cast<QWidget *>(this), this->ui->centralwidget,
                          this->ui->titleBarContainer, this->ui->controlsContainerRight}) {
    widget->setAttribute(Qt::WA_ContentsMarginsRespectsSafeArea, false);
  }
  // We use system controls and frame on macOS, hide custom buttons from the
  // decoration and keep the rest
  this->ui->closeBtn->hide();
  this->ui->minimizeBtn->hide();
  this->ui->maximizeBtn->hide();
  // Move box around the control buttons to the left
  this->setTitleBarDirection(Qt::RightToLeft);
#else
  // We do not use the system frame and controls on windows and linux so hide
  // everything from the system
  this->setMouseTracking(true);
  this->setWindowFlag(Qt::FramelessWindowHint);
  // Move control buttons to the right
  this->setTitleBarDirection(Qt::LeftToRight);
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
  // On linux we use the custom buttons and the default QMainWindow, connect the
  // custom buttons to the appropiate actions
  connect(this->ui->closeBtn, &QAbstractButton::clicked, this, &QMainWindow::close);
  connect(this->ui->minimizeBtn, &QAbstractButton::clicked, this, &QMainWindow::showMinimized);
  connect(this->ui->maximizeBtn, &QAbstractButton::clicked, [this]() {
    this->isMaximized() ? this->showNormal() : this->showMaximized();
  });
#endif

  // Restore the window geometry from the last use of the application
  this->restoreWindowStatesAndGeometry();

#if defined(Q_OS_WINDOWS)
  // Embed this QMainWindow into a native Windows window
  win_widget = new platform_specific::QWinWidget(this);
  win_widget->show();
#endif
}

void UndecoratedWindow::closeEvent(QCloseEvent * /*event*/) {
  // Store window state every time the use closes the window
  // This is not done at a force quit
  storeWindowStateAndGeometry();
}

void UndecoratedWindow::showEvent(QShowEvent *event) {
  // Do normal show event first
  QMainWindow::showEvent(event);
  // Do native stuff now
#if defined(Q_OS_DARWIN)
  // Hide system title bar using native code
  platform_specific::OSXHideTitleBar::Windowed(this->winId());
  this->updateMacOSControlsGeometry();
#elif defined(Q_OS_WINDOWS)
  // Update values for the windows native window
  win_widget->updateMinimumAndMaximumSize();
#endif
}
bool UndecoratedWindow::event(QEvent *event) {
  // Do normal window events first
  QMainWindow::event(event);

#if defined(Q_OS_DARWIN)
  // Hide and show native element of the macOS window frame
  if (event->type() == QEvent::WindowStateChange) {
    if (this->windowState() == Qt::WindowState::WindowFullScreen) {
      // Make native full screen call to hide black bar
       platform_specific::OSXHideTitleBar::FullScreen(this->winId());
      // Hide controls background
      this->ui->controlsContainerRight->hide();
    } else {
      // Make native windowed call hide title bar again
       platform_specific::OSXHideTitleBar::Windowed(this->winId());
      // Show controls background
      this->updateMacOSControlsGeometry();
      this->ui->controlsContainerRight->show();
    }
  }
#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
  // Handle window resizing on linux
  if (event->type() == QEvent::HoverMove) {
    auto *hover_event = dynamic_cast<QHoverEvent *>(event);
    QPoint position = hover_event->position().toPoint();
    if (position.x() > this->width() - RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeHorCursor);
    } else if (position.x() < RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeHorCursor);
    }
    if (position.y() > this->height() - RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeVerCursor);
    } else if (position.y() < RESIZE_HANDLES_WIDTH) {
      this->setCursor(Qt::SizeVerCursor);
    } else if (position.y() < MOVE_HANDLES_HEIGHT) {
      this->setCursor(Qt::ArrowCursor);
    }
  } else if (event->type() == QEvent::HoverLeave) {
    this->setCursor(Qt::ArrowCursor);
  } else if (event->type() == QEvent::MouseButtonPress) {
    auto *mouse_event = dynamic_cast<QMouseEvent *>(event);
    QPoint position = mouse_event->position().toPoint();
    if (position.x() > this->width() - RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::RightEdge);
      this->setCursor(Qt::SizeHorCursor);
    } else if (position.x() < RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::LeftEdge);
      this->setCursor(Qt::SizeHorCursor);
    }
    if (position.y() > this->height() - RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::BottomEdge);
      this->setCursor(Qt::SizeVerCursor);
    } else if (position.y() < RESIZE_HANDLES_WIDTH) {
      this->windowHandle()->startSystemResize(Qt::Edge::TopEdge);
      this->setCursor(Qt::SizeVerCursor);
    } else if (position.y() < MOVE_HANDLES_HEIGHT) {
      this->windowHandle()->startSystemMove();
      this->setCursor(Qt::ArrowCursor);
    }
    this->setCursor(Qt::ArrowCursor);
  }
#endif
  return false;
}

#if defined(Q_OS_DARWIN)
void UndecoratedWindow::updateMacOSControlsGeometry() {
  const QRect controls = platform_specific::OSXHideTitleBar::ControlsGeometry(this->winId());
  if (controls.isNull()) {
    return;
  }
  // Place the background exactly behind the native buttons, the title bar
  // starts at the top left corner of the window
  const QRect background = controls.marginsAdded(MACOS_CONTROLS_PADDING);
  this->ui->controlsContainerRight->setFixedSize(background.size());
  this->ui->titleBarContainer->layout()->setContentsMargins(
      background.x(), background.y(), background.x(), background.y());
}
#endif

void UndecoratedWindow::setTitleBarDirection(Qt::LayoutDirection direction) {
  // The native controls on macOS can not be moved to the right side
  if (!SUPPORTS_RIGHT_SIDE_CONTROLS && direction == Qt::LeftToRight) {
    qWarning("[UndecoratedWindow] Window controls can not be placed on the right side on this platform.");
    return;
  }
  // Flip direction of title bar and button order
  this->ui->titleBarContainer->setLayoutDirection(direction);
  this->ui->controlsContainerRight->setLayoutDirection(direction);
}

Qt::LayoutDirection UndecoratedWindow::titleBarDirection() {
  // Getter for current title bar direction
  return this->ui->titleBarContainer->layoutDirection();
}
