#include "Compositor.h"
#include <iostream>
#include <X11/extensions/Xdamage.h>
#include <iostream>

Compositor::Compositor(Window parent_window, XConnection connection)
    : connection(connection)
{
    XCompositeRedirectSubwindows(*connection, parent_window, CompositeRedirectManual);

    XWindowAttributes attr;
    XGetWindowAttributes( *connection, parent_window, &attr );
    XRenderPictFormat *format = XRenderFindVisualFormat( *connection, attr.visual );
    XRenderPictureAttributes pa;
    pa.subwindow_mode = IncludeInferiors; // Don't clip child widgets
    parent_picture = XRenderCreatePicture( *connection, parent_window, format, CPSubwindowMode, &pa );

}
bool already_added_one = false;
void Compositor::AddWindow(Window new_window)
{
    WindowInfo new_window_info;

    //
    XWindowAttributes attr;
    XGetWindowAttributes( *connection, new_window, &attr );
    XRenderPictFormat *format = XRenderFindVisualFormat( *connection, attr.visual );

    //
    XRenderPictureAttributes pa;
    pa.subwindow_mode = IncludeInferiors; // Don't clip child widgets

    //
    new_window_info.window = new_window;
    new_window_info.picture = XRenderCreatePicture( *connection, new_window, format, CPSubwindowMode, &pa );
    new_window_info.damage = XDamageCreate( *connection, new_window, XDamageReportNonEmpty );
    new_window_info.has_alpha = ( format->type == PictTypeDirect && format->direct.alphaMask );
    new_window_info.x = attr.x;
    new_window_info.y = attr.y;
    new_window_info.width  = attr.width;
    new_window_info.height  = attr.height;

    windows.push_back(new_window_info);
}
void Compositor::RemoveWindow(Window existing_window)
{
    for (auto it = windows.begin(); it != windows.end(); it ++)
    {
        if (it->window == existing_window) {
            windows.erase(it);
            return;
        }
    }
}

void Compositor::ReceiveDamageEvent(XDamageNotifyEvent* event)
{
    
    /* Draw the window of the damage event and all windows above it */
    /* .. For now, not so clever: draw all windows */
    for (auto it = windows.begin(); it < windows.end(); it ++)
    {
        Window damage_drawable = ((XDamageNotifyEvent*)(&event))->drawable;
        std::cout << "Window " << it->window << " vs Drawable " << damage_drawable << " vs Picture " << it->picture << std::endl;

        // TODO They aren't equal..
        // if (it->window == damage_drawable)
        if (true)
        {
            /* std::cout << "Rendering window " << it->window << std::endl; */
            XRenderComposite (*connection,
                           it->has_alpha ? PictOpOver : PictOpSrc,
                           it->picture,
                           None,
                           parent_picture,
                           0,
                           0,
                           0,
                           0,
                           it->x,
                           it->y,
                           it->width,
                           it->height);
        }
        XDamageSubtract(*connection, it->damage, None, None);
    }
}

/* Old solution, not used anymore. Maybe for testing. Listens for _all_ events. */

void Compositor::Compose()
{
    bool already_drawn_all_windows = false;
    XEvent event;
    while (XPending(*connection) > 0)
    {
        XNextEvent(*connection, &event);
        std::cout << "Event type: " << event.type << std::endl;

        if (already_drawn_all_windows)
            continue; /* just to empty queue */

        if (event.type == connection.GetDamageEventOffset() + XDamageNotify)
        {
            /* Draw the window of the damage event and all windows above it */
            /* .. For now, not so clever: draw all windows */
            already_drawn_all_windows = true;
            for (auto it = windows.begin(); it < windows.end(); it ++)
            {
                Window damage_drawable = ((XDamageNotifyEvent*)(&event))->drawable;
                std::cout << "Window " << it->window << " vs Drawable " << damage_drawable << std::endl;

                // if (it->window == damage_drawable)
                if (true)
                {
                    //
                    std::cout << "Rendering window " << it->window << std::endl;
                    XRenderComposite (*connection,
                                   it->has_alpha ? PictOpOver : PictOpSrc,
                                   it->picture,
                                   None,
                                   parent_picture,
                                   0,
                                   0,
                                   0,
                                   0,
                                   it->x,
                                   it->y,
                                   it->width,
                                   it->height);
                }
                XDamageSubtract(*connection, it->damage, None, None);
            }
        
        }
    }
}
