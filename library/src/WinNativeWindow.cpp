#include "WinNativeWindow.h"

#include <dwmapi.h>
#include <stdexcept>
namespace platform_specific {

HWND WinNativeWindow::childWindow = nullptr;
QWidget *WinNativeWindow::childWidget = nullptr;

WinNativeWindow::WinNativeWindow(const int x, const int y, const int width, const int height)
    : hWnd(nullptr) {

  //The native window technically has a background color. You can set it here
  HBRUSH windowBackground = CreateSolidBrush(RGB(255, 255, 255));

  HINSTANCE hInstance = GetModuleHandleW(nullptr);
  WNDCLASSEXW wcx = {};

  wcx.cbSize = sizeof(WNDCLASSEXW);
  wcx.style = CS_HREDRAW | CS_VREDRAW;
  wcx.hInstance = hInstance;
  wcx.lpfnWndProc = WndProc;
  wcx.cbClsExtra = 0;
  wcx.cbWndExtra = 0;
  wcx.lpszClassName = L"WindowClass";
  wcx.hbrBackground = windowBackground;
  wcx.hCursor = LoadCursor(nullptr, IDC_ARROW);

  // Registering an already registered class fails, which is fine
  if (!RegisterClassExW(&wcx) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
    throw std::runtime_error("Couldn't register window class");
  }

  //Create a native window with the appropriate style
  hWnd = CreateWindowW(L"WindowClass", L"WindowTitle", aero_borderless, x, y, width, height, 0, 0, hInstance, nullptr);
  if (!hWnd) {
    throw std::runtime_error("couldn't create window because of reasons");
  }

  SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

  //This code may be required for aero shadows on some versions of Windows
  const MARGINS aero_shadow_on = {1, 1, 1, 1};
  DwmExtendFrameIntoClientArea(hWnd, &aero_shadow_on);

  SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

}

WinNativeWindow::~WinNativeWindow() {
  //Hide the window & send the destroy message
  ShowWindow(hWnd, SW_HIDE);
  DestroyWindow(hWnd);
}

LRESULT CALLBACK WinNativeWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  WinNativeWindow *window = reinterpret_cast<WinNativeWindow *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

  if (!window) {
    return DefWindowProcW(hWnd, message, wParam, lParam);
  }

  switch (message) {
    // ALT + SPACE or F10 system menu
    case WM_SYSCOMMAND: {
      if (wParam == SC_KEYMENU) {
        RECT winrect;
        GetWindowRect(hWnd, &winrect);
        TrackPopupMenu(GetSystemMenu(hWnd, false), TPM_TOPALIGN | TPM_LEFTALIGN,
                       winrect.left + 5, winrect.top + 5, 0, hWnd, nullptr);
        break;
      } else {
        return DefWindowProcW(hWnd, message, wParam, lParam);
      }
    }
    case WM_NCCALCSIZE: {
      //this kills the window frame and title bar we added with
      //WS_THICKFRAME and WS_CAPTION
      return 0;
    }

      //If the parent window gets any close messages, send them over to QWinWidget and don't actually close here
    case WM_CLOSE: {
      if (childWindow) {
        SendMessage(childWindow, WM_CLOSE, 0, 0);
        return 0;
      }
      break;
    }
    case WM_DESTROY: {
      PostQuitMessage(0);
      break;
    }

    case WM_NCHITTEST: {
      //This value can be arbitrarily large as only intentionally-HTTRANSPARENT'd messages arrive here
      const LONG borderWidth = 8 * childWidget->window()->devicePixelRatio();
      RECT winrect;
      GetWindowRect(hWnd, &winrect);
      long x = GET_X_LPARAM(lParam);
      long y = GET_Y_LPARAM(lParam);

      //bottom left corner
      if (x >= winrect.left && x < winrect.left + borderWidth &&
          y < winrect.bottom && y >= winrect.bottom - borderWidth) {
        return HTBOTTOMLEFT;
      }
      //bottom right corner
      if (x < winrect.right && x >= winrect.right - borderWidth &&
          y < winrect.bottom && y >= winrect.bottom - borderWidth) {
        return HTBOTTOMRIGHT;
      }
      //top left corner
      if (x >= winrect.left && x < winrect.left + borderWidth &&
          y >= winrect.top && y < winrect.top + borderWidth) {
        return HTTOPLEFT;
      }
      //top right corner
      if (x < winrect.right && x >= winrect.right - borderWidth &&
          y >= winrect.top && y < winrect.top + borderWidth) {
        return HTTOPRIGHT;
      }
      //left border
      if (x >= winrect.left && x < winrect.left + borderWidth) {
        return HTLEFT;
      }
      //right border
      if (x < winrect.right && x >= winrect.right - borderWidth) {
        return HTRIGHT;
      }
      //bottom border
      if (y < winrect.bottom && y >= winrect.bottom - borderWidth) {
        return HTBOTTOM;
      }
      //top border
      if (y >= winrect.top && y < winrect.top + borderWidth) {
        return HTTOP;
      }

      //If it wasn't a border but we still got the message, return HTCAPTION to allow click-dragging the window
      return HTCAPTION;
    }

      //When this native window changes size, it needs to manually resize the QWinWidget child
    case WM_SIZE: {
      RECT winrect;
      GetClientRect(hWnd, &winrect);

      WINDOWPLACEMENT wp;
      wp.length = sizeof(WINDOWPLACEMENT);
      GetWindowPlacement(hWnd, &wp);
      if (childWidget) {
        const qreal ratio = childWidget->window()->devicePixelRatio();
        if (wp.showCmd == SW_MAXIMIZE) {
          //Maximized window draw 8 pixels off screen
          childWidget->setGeometry(8, 8, winrect.right / ratio - 16, winrect.bottom / ratio - 16);
        } else {
          childWidget->setGeometry(0, 0, winrect.right / ratio, winrect.bottom / ratio);
        }
      }
      break;
    }

    case WM_GETMINMAXINFO: {
      MINMAXINFO *minMaxInfo = (MINMAXINFO *) lParam;
      if (window->minimumSize.required) {
        minMaxInfo->ptMinTrackSize.x = window->getMinimumWidth();
        minMaxInfo->ptMinTrackSize.y = window->getMinimumHeight();
      }

      if (window->maximumSize.required) {
        minMaxInfo->ptMaxTrackSize.x = window->getMaximumWidth();
        minMaxInfo->ptMaxTrackSize.y = window->getMaximumHeight();
      }
      return 0;
    }
  }

  return DefWindowProcW(hWnd, message, wParam, lParam);
}

void WinNativeWindow::setGeometry(const int x, const int y, const int width, const int height) {
  MoveWindow(hWnd, x, y, width, height, 1);
}

void WinNativeWindow::setMinimumSize(const int width, const int height) {
  this->minimumSize.required = true;
  this->minimumSize.width = width;
  this->minimumSize.height = height;
}

int WinNativeWindow::getMinimumWidth() {
  return minimumSize.width;
}

int WinNativeWindow::getMinimumHeight() {
  return minimumSize.height;
}

void WinNativeWindow::setMaximumSize(const int width, const int height) {
  this->maximumSize.required = true;
  this->maximumSize.width = width;
  this->maximumSize.height = height;
}

int WinNativeWindow::getMaximumWidth() {
  return maximumSize.width;
}

int WinNativeWindow::getMaximumHeight() {
  return maximumSize.height;
}
RECT WinNativeWindow::getGeometry() {
  RECT Rect;
  GetWindowRect(hWnd, &Rect);
  MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT) & Rect, 2);
  return Rect;
}

}