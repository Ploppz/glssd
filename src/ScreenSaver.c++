#include "ScreenSaver.h"
#include "XConnection.h"

ScreenSaver::ScreenSaver(XConnection connection)
    :connection(connectino), main_window(create_main_window()), compositor(main_window)
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

/* Design issue
1) poll. Update() or something.
2) blocking function
 */

 void ScreenSaver::RunScreenSaver(std::string executable, int time_s)
{
    StartScreenSaver(executable);
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
    ensureOverlayWindow();
    if ( ret ) return;
    // Create child window
    uint width, height;
    connection.getWindowSize(main_window, width, height);
    demo_window = connection.createARGBWindow(main_window, 0, 0, width, height);
    XMapWindow(*connection, demo_window);
    // Start process
    const char * args[] = {"-steal", std::to_string(demo_window).c_str()};
    demo_process = new Process(demo_path, sizeof(args) / sizeof(args[0]), args);

    compositor.AddWindow(demo_window);
}

static void ScreenSaver::create_main_window()
{
    return connection.CreateOverlayWindow();
}
