#ifndef OSXHIDETITLEBAR_H
#define OSXHIDETITLEBAR_H

#include <QRect>

namespace platform_specific {

/**
 * Handler that can interact with a macOs title bar using the coca API and
 * Objective-C code. The current implementation can make the required changes to
 * a windowed window and a full screen window
 */
class OSXHideTitleBar {
 public:
  /**
   * Will hide the title bar and control from the window.
   * @param windowId Id of the current QWindow running.
   */
  static void Windowed(unsigned long long windowId);

  /**
   * Will add a transparent title bar and the control back in
   * @param windowId Id of the current QWindow running.
   */
  static void FullScreen(unsigned long long windowId);

  /**
   * Returns the area covered by the native window controls (close, minimize
   * and zoom button) in the coordinates of the Qt window.
   * @param windowId Id of the current QWindow running.
   * @return Geometry of the controls or a null rect if they are not visible.
   */
  static QRect ControlsGeometry(unsigned long long windowId);
};
}
#endif //OSXHIDETITLEBAR_H
