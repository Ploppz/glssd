#include "ScreenSaver.h"
#include "XConnection.h"
#include <assert.h>

#define NO_FULLSCREEN

ScreenSaver::ScreenSaver(XConnection connection)
    :connection(connection), main_window(create_main_window(connection)), compositor(main_window, connection)
{

#ifndef DONT_CAPTURE_SCREEN
    { /* For omniscience */
        XGrabKeyboard(*connection, main_window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
        // XGrabPointer(*connection, main_window, False, ButtonPressMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    }
#endif

}
void ScreenSaver::ReceiveDamageEvent(XDamageNotifyEvent *event)
{
    compositor.ReceiveDamageEvent(event);
}


void ScreenSaver::StartScreenSaver(std::string executable)
{
    bool ret = false;

    if ( demo_process ) {
        delete demo_process;
        demo_process = NULL;
        ret = true;
    }
    if ( demo_window != None ) {
        compositor.RemoveWindow(demo_window);
        XDestroyWindow(*connection, demo_window);
        demo_window = None;
        ret = true;
    }
    assert( !ret ); // not really sure what I was thinking above
    // if ( ret ) return;
    // Create child window
    uint width, height;
    connection.GetWindowSize(main_window, width, height);
    demo_window = connection.CreateARGBWindow(main_window, 0, 0, width, height);
    XMapWindow(*connection, demo_window);
    // Start process
    const char * args[] = {"-steal", std::to_string(demo_window).c_str()};
    demo_process = new Process(executable, sizeof(args) / sizeof(args[0]), args);

    compositor.AddWindow(demo_window);
}
void ScreenSaver::StartLogin(std::string executable)
{
    assert( ! login_process );
    assert( login_window == None );

    uint width, height;
    connection.GetWindowSize(main_window, width, height);
    login_window = connection.CreateARGBWindow(main_window, 0, 0, width, height);
    XMapWindow(*connection, login_window);
    // Start process
    const char * args[] = {"-steal", std::to_string(login_window).c_str()};
    login_process = new Process(executable, sizeof(args) / sizeof(args[0]), args);

    compositor.AddWindow(login_window);
}
void ScreenSaver::ExitLogin()
{
    assert( login_process );
    assert( login_window != None );

    compositor.RemoveWindow(login_window);
    XDestroyWindow(*connection, login_window);
    login_window = None;

    delete login_process;
    login_process = NULL;
}

int ScreenSaver::create_main_window(XConnection connection)
{
#ifdef DONT_CAPTURE_SCREEN
    Window w = connection.CreateARGBWindow(connection.GetRootWindow(), 0, 0, 10, 10);
    XMapWindow(*connection, w);
    return w;
#else
    return connection.CreateOverlayWindow();
#endif
}


/** Clean Up **/
ScreenSaver::~ScreenSaver()
{
#ifndef DONT_CAPTURE_SCREEN
    XAllowEvents(*connection, AsyncKeyboard, CurrentTime);
    XAllowEvents(*connection, AsyncPointer, CurrentTime);
#endif

    // XFlush(*connection); // where anyway?
    if (demo_process) {
        delete demo_process;
        demo_process = NULL;
    }
    if ( login_process ) {
        delete login_process;
        login_process = NULL;
    }
    if (main_window != None) {
        XDestroyWindow(*connection, main_window);
        main_window = None;
    }
}
