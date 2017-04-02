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

ScreenSaverController::ScreenSaverController(XConnection connection, Configuration config)
    : connection(connection), config(config)
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
            state = RunLogin();
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

        if (connection.GetIdleSeconds() > config.patience) {
            StartScreenSaver();
            return DEMO;
        }
        sleep(1);
    }
}
ScreenSaverController::State ScreenSaverController::RunScreenSaver()
{
    assert( screen_saver );
    
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

        if (connection.GetIdleSeconds() < config.patience) {
            
            if (config.authentication) {
                StartLogin();
                return LOGIN;
            } else {
                ExitScreenSaver();
                return INACTIVE;
            }
        }
    }
}
ScreenSaverController::State ScreenSaverController::RunLogin()
{
    assert( screen_saver );

    XEvent event;
    while (true) {
        connection.WaitForEventOrTimeout(1000);
        std::cout << "Wakey wake" << std::endl;
        bool any_events = false;
        while (XPending(*connection) > 0) {
            any_events = true;
            std::cout << " - event " << std::endl;
            XNextEvent(*connection, &event);
            if (event.type == connection.GetDamageEventOffset() + XDamageNotify) {
                screen_saver->ReceiveDamageEvent((XDamageNotifyEvent*) &event);
                continue;
            }
        }
        assert(any_events);
        // Assume that at least the login program will produce frequent enough DamageNotify events
        if (connection.GetIdleSeconds() > config.login_patience) {
            ExitLogin();
            return DEMO;
        }

        // TODO check if authentication successful
    }
}

void ScreenSaverController::StartLogin()
{
    std::cout << "Start login." << std::endl;
    screen_saver->StartLogin(config.login_path);
}
void ScreenSaverController::ExitLogin()
{
    std::cout << "Quit login" << std::endl;
    screen_saver->ExitLogin();
}
void ScreenSaverController::StartScreenSaver()
{
    std::cout << "Start screen saver." << std::endl;
    if ( ! screen_saver ) {
        screen_saver = new ScreenSaver(connection);
    }
    screen_saver->StartScreenSaver(config.demos.front().path);
}
void ScreenSaverController::ExitScreenSaver()
{
    std::cout << "Exit screen saver." << std::endl;

    if (screen_saver) {
        delete screen_saver;
        screen_saver = NULL;
    }
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
