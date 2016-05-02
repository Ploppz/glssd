#include <Process.h>
#include <XConnection.h>

#include <iostream>
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xdamage.h>

#define BLACK BlackPixel(*dpy, DefaultScreen(*dpy))
#define WHITE WhitePixel(*dpy, DefaultScreen(*dpy))

XConnection dpy;
Window main_window;
Window demo_window;

Process* demo_process = NULL;

struct WindowInfo
{
    Window window;
    Picture picture;
    bool has_alpha;
    int x, y, width, height;

    Damage damage;
};

int main()
{
    XSynchronize(*dpy, True);

    const uint WIN_SIZE = 400;
    main_window = dpy.createARGBWindow(DefaultRootWindow(*dpy), 200, 200, WIN_SIZE, WIN_SIZE);
    XMapWindow(*dpy, main_window);
    // Create child window
    uint width, height;
    dpy.getWindowSize(main_window, width, height);
    demo_window = dpy.createARGBWindow(main_window, 0, 0, width, height);

    // Make window appear on screen
    XMapWindow(*dpy, demo_window);
    
    // Create process which uses this child window
    const char * args[] = {"-steal", std::to_string(demo_window).c_str()};

    demo_process = new Process("/home/ploppz/work/c++/glss-demo/bin/landscape", sizeof(args) / sizeof(args[0]), args);

    Picture parent_picture;
    int damage_event, damage_error;
    /** Initialize compositing on main_window **/
    {
        XCompositeRedirectSubwindows(*dpy, main_window, CompositeRedirectManual);

        XWindowAttributes attr;
        XGetWindowAttributes( *dpy, main_window, &attr );
        XRenderPictFormat *format = XRenderFindVisualFormat( *dpy, attr.visual );
        XRenderPictureAttributes pa;
        pa.subwindow_mode = IncludeInferiors; // Don't clip child widgets
        parent_picture = XRenderCreatePicture( *dpy, main_window, format, CPSubwindowMode, &pa );

        if (!XDamageQueryExtension(*dpy, &damage_event, &damage_error)) {
            fprintf (stderr, "No damage extension\n");
            exit (1);
        }
    }

    WindowInfo demo_window_info;
    demo_window_info.window = demo_window;
    /** Add window to be composited, demo_window **/
    {
        demo_window_info.damage = XDamageCreate( *dpy, demo_window, XDamageReportNonEmpty );

        XWindowAttributes attr;
        XGetWindowAttributes( *dpy, demo_window, &attr );
        //
        XRenderPictFormat *format = XRenderFindVisualFormat( *dpy, attr.visual );

        demo_window_info.has_alpha = ( format->type == PictTypeDirect && format->direct.alphaMask );
        demo_window_info.x = attr.x;
        demo_window_info.y = attr.y;
        demo_window_info.width  = attr.width;
        demo_window_info.height  = attr.height;
        //
        XRenderPictureAttributes pa;
        pa.subwindow_mode = IncludeInferiors; // Don't clip child widgets

        demo_window_info.picture = XRenderCreatePicture( *dpy, demo_window, format, CPSubwindowMode, &pa );
    }
    while (true) {
        XEvent event;
        XNextEvent(*dpy, &event);
        std::cout << "Event type: " << event.type << std::endl;
        if (event.type == damage_event + XDamageNotify) {
            std::cout << "Rendering window " << demo_window_info.window << std::endl; 
            Window damage_drawable = ((XDamageNotifyEvent*)(&event))->drawable;
            std::cout << "Window " << demo_window_info.window << " vs Drawable " << damage_drawable << std::endl;
            std::cout << "\tvs" << demo_window_info.picture << std::endl;
            std::cout << "\tvs" << parent_picture << std::endl;
            XRenderComposite (*dpy,
                           demo_window_info.has_alpha ? PictOpOver : PictOpSrc,
                           demo_window_info.picture,
                           None,
                           parent_picture,
                           0,
                           0,
                           0,
                           0,
                           demo_window_info.x,
                           demo_window_info.y,
                           demo_window_info.width,
                           demo_window_info.height);
            /* Repair */
            XDamageSubtract(*dpy, demo_window_info.damage, None, None);
        }
    }
}
