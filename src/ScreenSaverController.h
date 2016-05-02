#pragma once
#include "XConnection.h"
#include "Compositor.h"
#include "Process.h"
#include "ScreenSaver.h"
#include <string>



class ScreenSaverController
{
 public:
    ScreenSaverController(XConnection connection);
    void Run();

    void SetPatience(int patience) {this->patience = patience;};
    void AddDemo(std::string path_to_executable);
    void AddLogin(std::string path_to_executable);
 private:
    /* State */
    XConnection connection;
    ScreenSaver* screen_saver = NULL;


    /* Algorithm */
    enum State { INACTIVE, DEMO, LOGIN};

    State WaitForIdle();
    State RunScreenSaver();

    /* Transitions */
    void StartScreenSaver();
    void ExitScreenSaver();

    static int getIdleSeconds();
    void spawnNextDemo();
    void unlock();


    
     /* Configuration */
    int patience = 2; /* Seconds */
    std::string demo_path;
    std::string login_path;
    
    /* Initialization */
    static void create_main_window();
};
