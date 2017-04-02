# Current #

- Login must communicate successful login
    - authentication is simple, although login needs to be run as root (work/c++/test/shadow)
    - test without authentication
- Login window flickers on top of background  -- Could this be 'tearind' because of no vsync?



# ? #
- adding login window: no DamageNotify events? cout
  - segfault. (still?)

- CreateOverlayWindow maps the window, but CreateARGBWindow etc, doesn't

##
ScreenSaverDaemon / ScreenWaiter / ScreenServer / ScreenDaemon / ScreenSaverController
    runs a ScreenSaver / ScreenLocker on idle
        runs a Compositor
        runs processes

# Thoughts #
- State transitions happen in the 'old' state

- demos should be installed somewhere, system and/or user
