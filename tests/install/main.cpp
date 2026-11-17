/**
 * Creates an UndecoratedWindow from the installed library, shows it and quits.
 */
#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QTimer>

#include <client_side_window_decoration/UndecoratedWindow.h>

class TestWindow : public UndecoratedWindow {
 Q_OBJECT
 public:
  explicit TestWindow(QWidget *parent = nullptr) : UndecoratedWindow(parent) {
    centralWidget()->layout()->addWidget(new QLabel("Installed library works"));
  }
};

int main(int argc, char *argv[]) {
  QApplication application(argc, argv);
  TestWindow window;
  window.show();
  qInfo("[InstallTest] SUPPORTS_RIGHT_SIDE_CONTROLS = %s",
        UndecoratedWindow::SUPPORTS_RIGHT_SIDE_CONTROLS ? "true" : "false");
  QTimer::singleShot(0, &application, &QApplication::quit);
  return QApplication::exec();
}

#include "main.moc"
