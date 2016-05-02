#include <daemon.h>
#include <Process.h>
#include <Configuration.h>
#include <XConnection.h>
#include <Compositor.h>

#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>

#define BLACK BlackPixel(*dpy, DefaultScreen(*dpy))
#define WHITE WhitePixel(*dpy, DefaultScreen(*dpy))

/* Assumptions:
 * Child processes close the window 
*/

XConnection dpy;
Window main_window;
Window demo_window;
Window test_window;
//
Process* demo_process = NULL;
Process* test_process = NULL;

/*** Progress:
- Swap interval acts weird. Will try to make compositor only compose on Damage event.
- Context creation takes a long time when compositor gets activated (after process creation).
   - try small delay after process creation?
***/
int main()
{
    XSynchronize(*dpy, True);

    const uint WIN_SIZE = 400;
    main_window = dpy.createARGBWindow(DefaultRootWindow(*dpy), 200, 200, WIN_SIZE, WIN_SIZE);
    /* overlay_window = XCompositeGetOverlayWindow(*dpy, main_window); */ // doesn't work like that...
    XMapWindow(*dpy, main_window);
    // Create child window
    uint width, height;
    dpy.getWindowSize(main_window, width, height);
    demo_window = dpy.createARGBWindow(main_window, 0, 0, width, height);
    test_window = dpy.createARGBWindow(main_window, width/8, height/8, width - width/4, height- height/4);

    // Make windows appear on screen
    XMapWindow(*dpy, demo_window);
    XMapWindow(*dpy, test_window);
    
    // Create process which uses this child window
    const char * args[] = {"-steal", std::to_string(demo_window).c_str()};

    demo_process = new Process("/home/ploppz/work/c++/glss-demo/bin/landscape", sizeof(args) / sizeof(args[0]), args);

    args[1] = std::to_string(test_window).c_str();
    test_process = new Process("/home/ploppz/work/c++/glss-demo/bin/alpha_test", sizeof(args) / sizeof(args[0]), args);

    /* Compositing test */
    Compositor C(main_window, dpy);
    C.AddWindow(demo_window);
    C.AddWindow(test_window);
    while (true) {
        C.Compose();   
    }
}
