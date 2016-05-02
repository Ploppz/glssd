#pragma once
#include <vector>
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xdamage.h>
#include "XConnection.h"

struct WindowInfo
{
    Window window;
    Picture picture;
    Damage damage;
    bool has_alpha;
    int x, y, width, height;
};
// TODO (global), maybe signify in var names that they are IDs not objects?

class Compositor
{
public:
    Compositor(Window parent_window, XConnection connection);
    void AddWindow(Window new_window);
    void RemoveWindow(Window existing_window);
    void Compose();
private:
    Picture parent_picture;
    XConnection connection;

    // Start of vector is bottom-most window
    std::vector<WindowInfo> windows;

    int damage_event;
    int damage_error;
};
