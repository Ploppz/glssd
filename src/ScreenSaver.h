#include "Process.h"
#include "Compositor.h"
#include "XConnection.h"



class ScreenSaver
{
 public:
    ScreenSaver(XConnection connection);

    // For now it's just an initial thing
    void RunScreenSaver(std::string executable);

    void Event(XEvent event);
    void ReceiveDamageEvent(XDamageNotifyEvent event);

    // Update
    void Update();
    //
    void CleanUp();
 private:
   XConnection connection;

   Window main_window = None;
   Window demo_window = None;
   Window locker_window = None;
   GC main_gc;
   Process* demo_process = NULL;
   Process* login_process = NULL;
   Compositor compositor;

   void StartScreenSaver(std::string executable);

 private:
   static int get_idle_seconds();
   static void create_main_window();
};

