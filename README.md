# QtClientSideWindowDecorationsExamples

This repository contains an example of how ClientSide window decorations can be implemented. The full feature set of the common desktop platforms is preserved. Previous methods often have problems with aero snap on windows or resizing on macos. In addition, identical theming accross all platforms is presented.

| Operating System | Supported Versions | Features | Preview |
| ------------- | ------------- | ------------- | ------------- |
| MacOs | Big Sur  | Tilling and fullscreen ✅ <br> Save and restore window states ✅ <br> Resize ✅ <br> Move ✅ | ![](https://raw.githubusercontent.com/Foxei/QtClientSideWindowDecorations/main/doc/images/MacOs.png?token=AGDWGVXSIR6D36BD7L4ZRRLBW6Z6U) |
| Windows | 10  | Aero snap ✅ <br> Save and restore window states ✅ <br> Resize ✅ <br> Move ✅  | ![](https://raw.githubusercontent.com/Foxei/QtClientSideWindowDecorations/main/doc/images/Windows.png?token=AGDWGVRJRNLAMQY47OE3KWLBW62EM) |
| Ubuntu | 22.04 Jammy Jellyfish | Gnome snapping ✅ <br> Save and restore window states ✅ <br> Resize ✅ <br> Move ✅  | ![](https://raw.githubusercontent.com/Foxei/QtClientSideWindowDecorations/main/doc/images/Ubuntu.png?token=AGDWGVVD3OOGPWYIP4A6X3LBW62BI) |

### How?
I use native Objective-C code and the coca api, to hide the title bar. The calls are repeated for every window state change. 

### How?
I embed the QWindow in a native windows window. Corresponding system calls from the window decoration are forwarded. 

## How?
I use the startSystemResize and startSystemMove function from Qt 5.15 and a Qt::FramelessWindow.
