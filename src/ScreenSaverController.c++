#include "ScreenSaverController.h"
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>


/* Assumptions:
 * Child processes close the window 
*/

ScreenSaverController::ScreenSaver(XConnection connection)
    : connection(connection)
{
}

void ScreenSaverController::Run()
{
    ScreenSaverController::State state = WAIT_IDLE;

    while (true)
    {
        switch (state)
        {
        case WAIT_IDLE:
            state = WaitForIdle();
            break;
        case WAIT_ACTIVE:
            state = WaitForActivity();
            break;
        }
    }
}

ScreenSaverController::State ScreenSaver::WaitForIdle()
{
    while (true)
    {
        std::cout << "Idle: " << getIdleSeconds() << std::endl;

        if (getIdleSeconds() > patience) {
            /*** STATE TRANSITION **/
            std::cout << "Idle!" << std::endl;
            ensureOverlayWindow();
            // TODO Remove locker window if it exists
            return WAIT_ACTIVE;
        }
        sleep(1);
    }
}
ScreenSaverController::State ScreenSaver::WaitForActivity()
{
    assert(main_window != None);

    // TODO We will need epoll to see if there are events while controling the demos
    //  - For now, run only one demo
    ScreenSaver screen_saver(connection);
    screen_saver.RunScreenSaver(demo_path, 0); // synonymous with StartSS

    
    /* epoll */
    int epoll_fd = epoll_create(1);
    int x_fd = XConnectionNumber(*connection); // File descriptor of the X connection
    epoll_event out_event;
    out_event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, x_fd, &out_event);

    
    // XSelectInput(*connection, main_window, ButtonPressMask | KeyPressMask | PointerMotionMask);
    XSelectInput(*connection, main_window, KeyPressMask);
    XEvent event;
    while (true) {
        assert (
            epoll_wait(epoll_fd, &out_event, 1, 1000)  >= 0
            // TODO why are we waiting here
        );
        while (XPending(*connection) > 0) {
            XNextEvent(*connection, &event);
            if (event.type == connection.GetDamageEventOffset() + XDamageNotify) {
                screen_saver.ReceiveDamageEvent((XDamageNotifyEvent) event);
                continue;
            } // else..
            if (getIdleSeconds() < patience) {
                { // For omniscience
                    XAllowEvents(*connection, AsyncKeyboard, CurrentTime);
                    XAllowEvents(*connection, AsyncPointer, CurrentTime);
                }

                // TODO run login if wanted
                screen_saver.CleanUp();

                return WAIT_IDLE;
            }
        }
    }
}

/* TODO in the future
    Several demos and logins: add to vector
*/
void ScreenSaverController::AddDemo(std::string path_to_executable)
{
    demo_path = path_to_executable;
}
void ScreenSaverController::AddLogin(std::string path_to_executable)
{
    login_path = path_to_executable;
}

/////////////
// HELPERS //
/////////////

void ScreenSaverController::spawnNextDemo()
{
    assert(main_window != None);

    bool ret = false;
    if ( demo_window != None ) {
        XDestroyWindow(*connection, demo_window);
        demo_window = None;
        ret = true;
    }
    // Quit other running demo
    if ( demo_process ) {
        delete demo_process;
        demo_process = NULL;
        ret = true;
    }
    ensureOverlayWindow();
    if ( ret ) return;

    // Create child window
    uint width, height;
    connection.getWindowSize(main_window, width, height);
    demo_window = connection.createARGBWindow(main_window, 0, 0, width, height);
    // Ask for events, including MapNotify events.
    /* XSelectInput(connection, demo_window, StructureNotifyMask); */
    // Make window appear on screen
    XMapWindow(*connection, demo_window);
    // Wait for window to appear on screen
    /* for(;;) {
        XEvent e;
        XNextEvent(connection, &e);
        if (e.type == MapNotify)
          break;
    } */
    // TODO not sure if we need to wait for mapnotify
    
    // Create process which uses this child window
    const char * args[] = {"-steal", std::to_string(demo_window).c_str()};

    demo_process = new Process(demo_path, sizeof(args) / sizeof(args[0]), args);
    return;
    
}

/* void ScreenSaverController::ensureOverlayWindow()
{
    if (main_window == None) {
        main_window = connection.createOverlayWindow();
    }
    [>For test I suppose: Fill overlay window<]
    if (false)
    {
        main_gc = XCreateGC(*connection, main_window, 0, 0);
        uint width, height;
        connection.getWindowSize(main_window, width, height);

        // Background color..
        XColor xcolour;
        xcolour.red = 32000; xcolour.green = 0; xcolour.blue = 32000;
        xcolour.flags = DoRed | DoGreen | DoBlue;
        XAllocColor(*connection, DefaultColormap(*connection, DefaultScreen(*connection)), &xcolour);

        XSetForeground(*connection, main_gc, xcolour.pixel);
        XFillRectangle(*connection, main_window, main_gc, 0, 0, width, height);
    }
} */
void ScreenSaverController::unlock()
{
    if ( demo_window  != None ) {
        XDestroyWindow(*connection, demo_window);
        XFlush(*connection);
        demo_window = None;
    }
    if ( demo_process ) {
        delete demo_process;
        demo_process = NULL;
    }
    if ( login_process ) {
        delete login_process;
        login_process = NULL;
    }
    // I assume this will close all the child windows
    if ( main_window != None ) {
        XDestroyWindow(*connection, main_window);
        XFlush(*connection);
        main_window = None;
    }
}

static int ScreenSaverController::getIdleSeconds()
{
    XScreenSaverControllerInfo *info = XScreenSaverAllocInfo();
    XScreenSaverControllerQueryInfo(*connection, connection.rootWindow(), info);
    return info->idle / 1000;
}

static void ScreenSaverController::create_main_window()
{

}
