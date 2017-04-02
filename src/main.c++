/* TODO
 *  - When demo has wrong permissions
 */
#include "daemon.h"
#include "ScreenSaverController.h"
#include "ScreenSaver.h"
#include "Configuration.h"


#define BLACK BlackPixel(*dpy, DefaultScreen(*dpy))
#define WHITE WhitePixel(*dpy, DefaultScreen(*dpy))

int main()
{
    XConnection dpy;
    XSynchronize(*dpy, True);

    //
    Configuration config("config.toml");

    ScreenSaverController controller(dpy, config);
    controller.Run();
}
