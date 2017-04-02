#pragma once
#include "XConnection.h"
#include "Configuration.h"
#include "Compositor.h"
#include "Process.h"
#include "ScreenSaver.h"
#include <string>



class ScreenSaverController
{
 public:
    ScreenSaverController(XConnection connection, Configuration config);
    void Run();

 private:
    /* State */
    XConnection connection;
    Configuration config;
    ScreenSaver* screen_saver = NULL;


    /* State Algorithm */
    enum State { INACTIVE, DEMO, LOGIN};

    State WaitForIdle();
    State RunScreenSaver();
    State RunLogin();

    /* Transitions */
    void StartScreenSaver();
    void ExitScreenSaver();
    void StartLogin();
    void ExitLogin();

    static int getIdleSeconds();
    void spawnNextDemo();
    void unlock();
    
    /* Initialization */
    static void create_main_window();
};
