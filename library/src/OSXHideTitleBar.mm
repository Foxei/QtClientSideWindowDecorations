#include "OSXHideTitleBar.h"
#include <Cocoa/Cocoa.h>

namespace platform_specific {

void OSXHideTitleBar::Windowed(unsigned long long windowId) {
  auto *nativeView = reinterpret_cast<NSView *>(windowId);
  NSWindow *nativeWindow = [nativeView window];
  // Removes native title bar
  nativeWindow.styleMask = nativeWindow.styleMask | NSWindowStyleMaskFullSizeContentView;
  nativeWindow.titlebarAppearsTransparent = YES;
  nativeWindow.titleVisibility = NSWindowTitleHidden;
  nativeView.wantsLayer = YES;

  //[nativeWindow setMovableByWindowBackground: true];
  //nativeWindow.opaque = false;
  //nativeWindow.backgroundColor = NSColor.clearColor;
  //nativeView.layerUsesCoreImageFilters = true;

  // Increases the distance between buttons and border by injecting an empty toolbar
  NSToolbar *toolbar = [[NSToolbar alloc] initWithIdentifier:@"Toolbar"];
  toolbar.showsBaselineSeparator = false;
  toolbar.displayMode = NSToolbarDisplayMode::NSToolbarDisplayModeIconOnly;
  [nativeWindow setToolbar:toolbar];
  [toolbar release];

  //NSVisualEffectView *blurView = [[NSVisualEffectView alloc] initWithFrame:NSMakeRect(0,0,800,600)];
  //blurView.blendingMode = N
  //NSVisualEffectBlendingMode::NSVisualEffectBlendingModeBehindWindow;
  //blurView.material = NSVisualEffectMaterial::NSVisualEffectMaterialFullScreenUI;
  //blurView.state = NSVisualEffectState::NSVisualEffectStateActive;
  //[blurView release];*/
}

void OSXHideTitleBar::FullScreen(unsigned long long windowId) {
  auto *nativeView = reinterpret_cast<NSView *>(windowId);
  NSWindow *nativeWindow = [nativeView window];
  // Removes native title bar
  nativeWindow.styleMask = nativeWindow.styleMask | NSWindowStyleMaskFullSizeContentView;
  nativeWindow.titlebarAppearsTransparent = YES;
  nativeWindow.titleVisibility = NSWindowTitleHidden;
  nativeView.wantsLayer = YES;

  // Remove black bar by deleting the empty toolbar
  [nativeWindow setToolbar:nullptr];
}

QRect OSXHideTitleBar::ControlsGeometry(unsigned long long windowId) {
  auto *nativeView = reinterpret_cast<NSView *>(windowId);
  NSWindow *nativeWindow = [nativeView window];
  // Make sure the buttons are at their final position, e.g. after adding the toolbar
  [nativeWindow.contentView.superview layoutSubtreeIfNeeded];

  NSRect controls = NSZeroRect;
  for (NSWindowButton type : {NSWindowCloseButton, NSWindowMiniaturizeButton, NSWindowZoomButton}) {
    NSButton *button = [nativeWindow standardWindowButton:type];
    if (!button || button.hidden) {
      continue;
    }
    NSRect rect = [button convertRect:button.bounds toView:nativeView];
    controls = NSIsEmptyRect(controls) ? rect : NSUnionRect(controls, rect);
  }
  if (NSIsEmptyRect(controls)) {
    return {};
  }

  // Qt uses a top left origin, flip if the view uses the Cocoa bottom left origin
  if (!nativeView.isFlipped) {
    controls.origin.y = nativeView.bounds.size.height - NSMaxY(controls);
  }
  return QRectF(controls.origin.x, controls.origin.y, controls.size.width, controls.size.height).toAlignedRect();
}
}