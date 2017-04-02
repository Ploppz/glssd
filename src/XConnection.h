#pragma once
#include <X11/Xlib.h>

/* Utilities for managing windows */
/* This class takes an already open connection */
/* The connection pointer can be extracted using the * operator */

class XConnection
{
public:
    XConnection();

    Display*& operator* ();


    void GetWindowSize(Window window, uint& out_width, uint& out_height);
    Window CreateOverlayWindow();
    Window CreateWindow(int x, int y, int width, int height);
    Window CreateWindow(Window parent, int x, int y, int width, int height);
    Window CreateARGBWindow(Window parent, int x, int y, int width, int height);

    Window GetRootWindow() { return root_window; }

    int GetDamageEventOffset() { return ext_damage_event; }
    int GetDamageErrorOffset() { return ext_damage_error; }

    int GetIdleSeconds();

    void WaitForEventOrTimeout(int timeout_ms);
private:
    Display* dpy;
    Window root_window;

    // Extensions
    int ext_damage_event;
    int ext_damage_error;

    // Epoll
    int epoll_fd;
};
