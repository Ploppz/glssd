/* TODO
 *  - When demo has wrong permissions
 */
#include "daemon.h"
#include "ScreenSaverController.h"
#include "ScreenSaver.h"
#include "Configuration.h"


#define BLACK BlackPixel(*dpy, DefaultScreen(*dpy))
#define WHITE WhitePixel(*dpy, DefaultScreen(*dpy))



Configuration config("config");


int main()
{
    XConnection dpy;
    XSynchronize(*dpy, True);

    Configuration config("config");

    ScreenSaverController controller(dpy);
    controller.AddDemo(config.demo_path);
    controller.Run();
}

/////////////
// STATES //
///////////

