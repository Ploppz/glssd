#pragma once
#include "Process.h"
#include "Compositor.h"
#include "XConnection.h"



class ScreenSaver
{
 public:
    ScreenSaver(XConnection connection);
    ~ScreenSaver();


    void Event(XEvent event);
    void ReceiveDamageEvent(XDamageNotifyEvent *event);


    /* TODO behaviour:
        quit previous process, start new process
    */
    void StartScreenSaver(std::string executable);
    void ExitScreenSaver(); // TODO implement
    void StartLogin(std::string executable);
    void ExitLogin();
    //
    Window GetMainWindow() { return main_window; };
 private:
    XConnection connection;
  
    Window main_window = None;
    Window demo_window = None;
    Window login_window = None;
    GC main_gc;
    Process* demo_process = NULL;
    Process* login_process = NULL;
    Compositor compositor;

    void CleanUp();

 private:
    static int get_idle_seconds();
    static int create_main_window(XConnection connection);
};

