#ifndef OSXHIDETITLEBAR_H
#define OSXHIDETITLEBAR_H

class OSXHideTitleBar {
 public:
    /**
     * Function that injects into the MacOs X native rendering process and changes the window behaviour.
     * @param windowId Id of native window to inject.
     */
  static void Windowed(unsigned long long windowId);
  static void FullScreen(unsigned long long windowId);
};

#endif //OSXHIDETITLEBAR_H
