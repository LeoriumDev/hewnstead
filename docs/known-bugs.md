## Known bugs

### Stuck Shift after macOS screenshot shortcut

**Symptom**: Pressing Cmd+Shift+5 to take a screenshot, then returning to
Hewnstead, causes the camera to descend continuously (because LEFT_SHIFT is
treated as held). Cursor lock also breaks during the screenshot capture mode.

**Root cause**: macOS screenshot system intercepts keyboard input system-wide.
Modifier RELEASE events (Shift, Cmd) that occur during screenshot capture
mode are swallowed by the OS — GLFW receives neither callback nor polling
notification. Both key callback and `glfwGetKey` polling get stuck reporting
PRESS even after the user physically releases.

This is a known limitation of GLFW's Cocoa backend, not a Hewnstead bug.

**Workaround (dev environment)**:

1. Disable macOS system screenshot shortcuts:
   System Settings → Keyboard → Keyboard Shortcuts → Screenshots → uncheck all.
2. Use a third-party screenshot tool (Shottr, CleanShot X) with a non-Shift
   hotkey.

**Possible code-level fixes (deferred)**:

- macOS native `CGEventSourceKeyState` to query HID-level modifier state
  every frame and reconcile against callback state. Requires `#ifdef __APPLE__`,
  ApplicationServices.framework link.
- Block-level `CGEventTap` to intercept Cmd+Shift+4 before it reaches the
  screenshot system. Requires Accessibility permission, macOS-only.

Both deferred — for now doesn't justify either.

**Filed**: 2026-05-08