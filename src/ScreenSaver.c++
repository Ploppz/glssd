#include "ScreenSaver.h"
#include "XConnection.h"

ScreenSaver::ScreenSaver(XConnection connection)
    :connection(connection), main_window(create_main_window(connection)), compositor(main_window, connection)
{

    { /* For omniscience */
        XGrabKeyboard(*connection, main_window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
        // XGrabPointer(*connection, main_window, False, ButtonPressMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    }

}
void ScreenSaver::Update()
{
    compositor.Compose();
}
void ScreenSaver::ReceiveDamageEvent(XDamageNotifyEvent *event)
{
    compositor.ReceiveDamageEvent(event);
}

/* Design issue
1) poll. Update() or something.
2) blocking function
 */

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
    if ( ret ) return;
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

int ScreenSaver::create_main_window(XConnection connection)
{
    return connection.CreateOverlayWindow();
}
void ScreenSaver::CleanUp()
{
    // XFlush(*connection);

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
