#include "XConnection.h"
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/scrnsaver.h>
#include <cstdlib>
#include <iostream>
#include <sys/epoll.h>

#define BLACK BlackPixel(dpy, DefaultScreen(dpy))
#define WHITE WhitePixel(dpy, DefaultScreen(dpy))

XConnection::XConnection()
{
    dpy = XOpenDisplay(NULL);
    root_window = DefaultRootWindow(dpy);

    if (!XDamageQueryExtension(dpy, &ext_damage_event, &ext_damage_error)) {
        std::cerr << "No damage extension" << std::endl;
        exit (1);
    }

    /* Set up epoll, for WaitForEventOrTimeout() */
    epoll_fd = epoll_create(1);
    int x_fd = XConnectionNumber(dpy); // File descriptor of the X connection
    epoll_event out_event;
    out_event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, x_fd, &out_event);
}

Display*& XConnection::operator* ()
{
    return dpy;
}
void XConnection::GetWindowSize(Window window, uint& out_width, uint& out_height)
{
    Window root;
    int x, y;
    uint b_width, depth;
    XGetGeometry(dpy, window, &root, &x, &y, &out_width, &out_height, &b_width, &depth);
}
// Idempotent
Window XConnection::CreateOverlayWindow()
{
    XVisualInfo visualinfo ;
    XMatchVisualInfo(dpy, DefaultScreen(dpy), 32, TrueColor, &visualinfo);

    // Get dimentions of the root window
    XWindowAttributes root_attribs;
    XGetWindowAttributes( dpy, root_window, &root_attribs );

    // Set attributes for the new window
    //   - override-redirect: window not affected by WM
    XSetWindowAttributes new_attribs;
    new_attribs.override_redirect = true;
    int new_attribs_mask = CWOverrideRedirect;
    // Create the window
    Window w = XCreateWindow(dpy, root_window, 0, 0, root_attribs.width, root_attribs.height, 0,
            CopyFromParent, InputOutput, CopyFromParent,
            new_attribs_mask, &new_attribs);
    // Ask for events, including MapNotify events.
    XSelectInput(dpy, w, StructureNotifyMask);
    // Make window appear on screen
    XMapWindow(dpy, w);
    // Wait for window to appear on screen
    for(;;) {
        XEvent e;
        XNextEvent(dpy, &e);
        if (e.type == MapNotify)
          break;
    }
    // Don't know if this is necessary
    XSetInputFocus(dpy, w, RevertToParent, CurrentTime);

    return w;
}

Window XConnection::CreateWindow(int x, int y, int width, int height)
{
    // Create the window
    Window w = XCreateSimpleWindow(dpy, root_window, x, y, width, height, 0, BLACK, BLACK);
    // Ask for events, including MapNotify events.
    XSelectInput(dpy, w, StructureNotifyMask);
    // Make window appear on screen
    XMapWindow(dpy, w);
    // Wait for window to appear on screen
    for(;;) {
        XEvent e;
        XNextEvent(dpy, &e);
        if (e.type == MapNotify)
          break;
    }
    return w;
}
Window XConnection::CreateWindow(Window parent, int x, int y, int width, int height)
{
    return XCreateSimpleWindow(dpy, parent, 0, 0, width, height, 0, BLACK, BLACK);

}
Window XConnection::CreateARGBWindow(Window parent, int x, int y, int width, int height)
{
    XVisualInfo visualinfo ;
    XMatchVisualInfo(dpy, DefaultScreen(dpy), 32, TrueColor, &visualinfo);

    // create window
    XSetWindowAttributes attr ;
    attr.colormap   = XCreateColormap( dpy, parent, visualinfo.visual, AllocNone) ;
    attr.event_mask = ExposureMask | KeyPressMask ;
    attr.background_pixmap = None ;
    attr.border_pixel      = 0 ;
    return XCreateWindow(    dpy, parent,
                           x, y, width, height, // x,y,width,height : are possibly overwriteen by window manager
                           0,
                           visualinfo.depth,
                           InputOutput,
                           visualinfo.visual,
                           CWColormap|CWEventMask|CWBackPixmap|CWBorderPixel,
                           &attr
                           ) ;
}

int XConnection::GetIdleSeconds()
{
    XScreenSaverInfo *info = XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(dpy, GetRootWindow(), info);
    return info->idle / 1000;
}

void XConnection::WaitForEventOrTimeout(int timeout_ms)
{
    epoll_event out_event;
    assert(
        epoll_wait(epoll_fd, &out_event, 1, timeout_ms)  >= 0
    );
}
