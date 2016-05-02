#pragma once
#include "Process.h"
#include "Compositor.h"
#include "XConnection.h"



class ScreenSaver
{
 public:
    ScreenSaver(XConnection connection);
    ~ScreenSaver() { CleanUp(); };

    // For now it's just an initial thing
    void RunScreenSaver(std::string executable);

    void Event(XEvent event);
    void ReceiveDamageEvent(XDamageNotifyEvent *event);


   void StartScreenSaver(std::string executable);
    //
    Window GetMainWindow() { return main_window; };
 private:
   XConnection connection;

   Window main_window = None;
   Window demo_window = None;
   Window locker_window = None;
   GC main_gc;
   Process* demo_process = NULL;
   Process* login_process = NULL;
   Compositor compositor;

    void CleanUp();

 private:
   static int get_idle_seconds();
   static int create_main_window(XConnection connection);
 private: //stash
    void Update();
};

