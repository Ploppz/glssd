#include "ScreenSaver.h"
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

ScreenSaverController::ScreenSaverController(XConnection connection)
    : connection(connection)
{
}

void ScreenSaverController::Run()
{
    ScreenSaverController::State state = INACTIVE;

    while (true)
    {
        switch (state)
        {
        case INACTIVE:
            state = WaitForIdle();
            break;
        case DEMO:
            state = RunScreenSaver();
            break;
        case LOGIN:
            assert(!"LOGIN state not implemented yet.");
            break;
        }
    }
}

ScreenSaverController::State ScreenSaverController::WaitForIdle()
{
    assert(screen_saver == NULL);
    while (true)
    {
        std::cout << "Idle: " << connection.GetIdleSeconds() << std::endl;

        if (connection.GetIdleSeconds() > patience) {
            /*** STATE TRANSITION **/
            // TODO Remove locker window if it exists
            StartScreenSaver();
            return DEMO;
        }
        sleep(1);
    }
}
ScreenSaverController::State ScreenSaverController::RunScreenSaver()
{
    assert( screen_saver );
    screen_saver->StartScreenSaver(demo_path);
    
    // XSelectInput(*connection, main_window, ButtonPressMask | KeyPressMask | PointerMotionMask);
    XSelectInput(*connection, screen_saver->GetMainWindow(), KeyPressMask);
    XEvent event;
    while (true) {
        XNextEvent(*connection, &event);
            
        if (event.type == connection.GetDamageEventOffset() + XDamageNotify) {
            screen_saver->ReceiveDamageEvent((XDamageNotifyEvent*) &event);
            continue;
        }

        // else..

        if (connection.GetIdleSeconds() < patience) {
            ExitScreenSaver(); // TODO perhaps run login
            return INACTIVE;
        }
    }
}

void ScreenSaverController::StartScreenSaver()
{
    std::cout << "Start screen saver." << std::endl;
    if ( ! screen_saver ) {
        screen_saver = new ScreenSaver(connection);
    }
}
void ScreenSaverController::ExitScreenSaver()
{
    std::cout << "Exit screen saver." << std::endl;
    XAllowEvents(*connection, AsyncKeyboard, CurrentTime);
    XAllowEvents(*connection, AsyncPointer, CurrentTime);

    if (screen_saver) {
        delete screen_saver;
        screen_saver = NULL;
    }
}


/*
TODO in the future
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
