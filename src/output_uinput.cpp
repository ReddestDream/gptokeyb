/* Copyright (c) 2021-2023
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
#
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
#
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301 USA
#
* Authored by: Kris Henriksen <krishenriksen.work@gmail.com>
#
* AnberPorts-Keyboard-Mouse
* 
* Part of the code is from from https://github.com/krishenriksen/AnberPorts/blob/master/AnberPorts-Keyboard-Mouse/main.c (mostly the fake keyboard)
* Fake Xbox code from: https://github.com/Emanem/js2xbox
* 
* Modified (badly) by: Shanti Gilbert for EmuELEC
* Modified further by: Nikolai Wuttke for EmuELEC (Added support for SDL and the SDLGameControllerdb.txt)
* Modified further by: Jacob Smith
* 
* Any help improving this code would be greatly appreciated! 
* 
* DONE: Xbox360 mode: Fix triggers so that they report from 0 to 255 like real Xbox triggers
*       Xbox360 mode: Figure out why the axis are not correctly labeled?  SDL_CONTROLLER_AXIS_RIGHTX / SDL_CONTROLLER_AXIS_RIGHTY / SDL_CONTROLLER_AXIS_TRIGGERLEFT / SDL_CONTROLLER_AXIS_TRIGGERRIGHT
*       Keyboard mode: Add a config file option to load mappings from.
*       add L2/R2 triggers
* 
*/

#include "gptokeyb.h"

int uinp_fd;
uinput_user_dev uidev;

int emit_init_uinput()
{
    fprintf(stderr, "Running in UINPUT output mode.\n");
    int success = 0;

    uinp_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinp_fd < 0) {
        fprintf(stderr, "Unable to open /dev/uinput\n");
        return -1;
    }

    // Intialize the uInput device to NULL
    memset(&uidev, 0, sizeof(uidev));
    uidev.id.version = 1;
    uidev.id.bustype = BUS_USB;

    if (xbox360_mode) {
        printf("Running in Fake Xbox 360 Mode\n");
        success = setupFakeXbox360Device(uidev, uinp_fd);

        if (success < 0)
        {
            shutdownFakeKeyboardMouseDevice();
            return success;
        }
    } else {
        printf("Running in Fake Keyboard mode\n");
        success = setupFakeKeyboardMouseDevice(uidev, uinp_fd);

        if (success < 0)
        {
            shutdownFakeKeyboardMouseDevice();
            return success;
        }
    }

    // Create input device into input sub-system
    write(uinp_fd, &uidev, sizeof(uidev));

    if (ioctl(uinp_fd, UI_DEV_CREATE)) {
        fprintf(stderr, "Unable to create UINPUT device.");
        shutdownFakeKeyboardMouseDevice();
        uinp_fd = -1;
        return -1;
    }

    return 0;
}

void emit_quit_uinput()
{
    shutdownFakeKeyboardMouseDevice();
}

void emit(int type, int code, int val)
{
    struct input_event ev;

    ev.type = type;
    ev.code = code;
    ev.value = val;
    /* timestamp values below are ignored */
    ev.time.tv_sec = 0;
    ev.time.tv_usec = 0;

    write(uinp_fd, &ev, sizeof(ev));
}

void emitKey_uinput(int code, bool is_pressed, int modifier)
{
    if (code == 0)
        return;

    if (!(modifier == 0) && is_pressed) {
        emit(EV_KEY, modifier, is_pressed ? 1 : 0);
        emit(EV_SYN, SYN_REPORT, 0);
    }
    emit(EV_KEY, code, is_pressed ? 1 : 0);
    emit(EV_SYN, SYN_REPORT, 0);
    if (!(modifier == 0) && !(is_pressed)) {
        emit(EV_KEY, modifier, is_pressed ? 1 : 0);
        emit(EV_SYN, SYN_REPORT, 0);
    }
}

void emitTextInputKey_uinput(int code, bool uppercase)
{
    if (uppercase) { //capitalise capital letters by holding shift
        emitKey_uinput(KEY_LEFTSHIFT, true);
    }
    emitKey_uinput(code, true);
    SDL_Delay(16);
    emitKey_uinput(code, false);
    SDL_Delay(16);
    if (uppercase) { //release shift if held
        emitKey_uinput(KEY_LEFTSHIFT, false);
    }
}

void emitAxisMotion_uinput(int code, int value)
{
    emit(EV_ABS, code, value);
    emit(EV_SYN, SYN_REPORT, 0);
}

void emitMouseMotion_uinput(int x, int y)
{
    if (x != 0) {
        emit(EV_REL, REL_X, x);
    }
    if (y != 0) {
        emit(EV_REL, REL_Y, y);
    }

    if (x != 0 || y != 0) {
        emit(EV_SYN, SYN_REPORT, 0);
    }
}
