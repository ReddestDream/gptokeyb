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

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

static Display *display = NULL;

static inline int keysm_to_x11keysym(int keysym)
{
    switch (keysym)
    {
    case BTN_LEFT:       return 0x0001;
    case BTN_RIGHT:      return 0x0003;

    case KEY_ESC:        return XK_Escape;

    case KEY_LEFT:       return XK_Left;
    case KEY_RIGHT:      return XK_Right;
    case KEY_UP:         return XK_Up;
    case KEY_DOWN:       return XK_Down;
    case KEY_HOME:       return XK_Home;
    case KEY_END:        return XK_End;
    case KEY_ENTER:      return XK_Return;
    case KEY_DELETE:     return XK_Delete;
    case KEY_BACKSPACE:  return XK_BackSpace;
    case KEY_TAB:        return XK_Tab;
    case KEY_PAGEUP:     return XK_Page_Up;
    case KEY_PAGEDOWN:   return XK_Page_Down;
    case KEY_INSERT:     return XK_Insert;
    case KEY_CAPSLOCK:   return XK_Caps_Lock;
    case KEY_PAUSE:      return XK_Pause;
    case KEY_MENU:       return XK_Menu;

    case KEY_LEFTCTRL:   return XK_Control_L;
    case KEY_RIGHTCTRL:  return XK_Control_R;
    case KEY_LEFTALT:    return XK_Alt_L;
    case KEY_RIGHTALT:   return XK_Alt_R;
    case KEY_LEFTSHIFT:  return XK_Shift_L;
    case KEY_RIGHTSHIFT: return XK_Shift_R;

    case KEY_F1:         return XK_F1;
    case KEY_F2:         return XK_F2;
    case KEY_F3:         return XK_F3;
    case KEY_F4:         return XK_F4;
    case KEY_F5:         return XK_F5;
    case KEY_F6:         return XK_F6;
    case KEY_F7:         return XK_F7;
    case KEY_F8:         return XK_F8;
    case KEY_F9:         return XK_F9;
    case KEY_F10:        return XK_F10;
    case KEY_F11:        return XK_F11;
    case KEY_F12:        return XK_F12;

    case KEY_0:          return XK_0;
    case KEY_1:          return XK_1;
    case KEY_2:          return XK_2;
    case KEY_3:          return XK_3;
    case KEY_4:          return XK_4;
    case KEY_5:          return XK_5;
    case KEY_6:          return XK_6;
    case KEY_7:          return XK_7;
    case KEY_8:          return XK_8;
    case KEY_9:          return XK_9;

    case KEY_A:          return XK_a;
    case KEY_B:          return XK_b;
    case KEY_C:          return XK_c;
    case KEY_D:          return XK_d;
    case KEY_E:          return XK_e;
    case KEY_F:          return XK_f;
    case KEY_G:          return XK_g;
    case KEY_H:          return XK_h;
    case KEY_I:          return XK_i;
    case KEY_J:          return XK_j;
    case KEY_K:          return XK_k;
    case KEY_L:          return XK_l;
    case KEY_M:          return XK_m;
    case KEY_N:          return XK_n;
    case KEY_O:          return XK_o;
    case KEY_P:          return XK_p;
    case KEY_Q:          return XK_q;
    case KEY_R:          return XK_r;
    case KEY_S:          return XK_s;
    case KEY_T:          return XK_t;
    case KEY_U:          return XK_u;
    case KEY_V:          return XK_v;
    case KEY_W:          return XK_w;
    case KEY_X:          return XK_x;
    case KEY_Y:          return XK_y;
    case KEY_Z:          return XK_z;

    case KEY_KP0:        return XK_KP_0;
    case KEY_KP1:        return XK_KP_1;
    case KEY_KP2:        return XK_KP_2;
    case KEY_KP3:        return XK_KP_3;
    case KEY_KP4:        return XK_KP_4;
    case KEY_KP5:        return XK_KP_5;
    case KEY_KP6:        return XK_KP_6;
    case KEY_KP7:        return XK_KP_7;
    case KEY_KP8:        return XK_KP_8;
    case KEY_KP9:        return XK_KP_9;

    case KEY_KPASTERISK: return XK_KP_Multiply;
    case KEY_KPSLASH:    return XK_KP_Divide;
    case KEY_KPPLUS:     return XK_KP_Add;
    case KEY_KPMINUS:    return XK_KP_Subtract;
    case KEY_KPCOMMA:    return XK_KP_Separator;
    case KEY_SPACE:      return XK_space;
    case KEY_KPDOT:      return XK_KP_Decimal;
    case KEY_KPENTER:    return XK_KP_Enter;

    case KEY_EQUAL:      return XK_equal;
    case KEY_MINUS:      return XK_minus;
    case KEY_DOT:        return XK_period;
    case KEY_COMMA:      return XK_comma;
    case KEY_BACKSLASH:  return XK_backslash;

    case KEY_APOSTROPHE: return XK_apostrophe;
    case KEY_SEMICOLON:  return XK_semicolon;
    case KEY_SLASH:      return XK_slash;
    case KEY_GRAVE:      return XK_grave;
    case KEY_LEFTBRACE:  return XK_bracketleft;
    case KEY_RIGHTBRACE: return XK_bracketright;

    default:             return 0;
    }
}


int emit_init_x11()
{
    fprintf(stderr, "[GPTK]: Running in X11 output mode.\n");
    Display *display = XOpenDisplay(NULL);

    if (xbox360_mode)
        xbox360_mode = false;

    if (display == NULL)
    {
        fprintf(stderr, "[GPTK]: Cannot open display\n");
        return -1;
    }

    return 0;
}


void emit_quit_x11()
{
    if (display != NULL)
    {
        XCloseDisplay(display);
    }
}


void emitKey_x11(int keysym, bool is_pressed, int modifier)
{
    int x11_keysym = keysm_to_x11keysym(keysym);
    KeyCode keycode = 0;

    if (keysym != BTN_LEFT && keysym != BTN_RIGHT)
        keycode = XKeysymToKeycode(display, x11_keysym);
    KeyCode modcode = modifier != 0 ? XKeysymToKeycode(display, keysm_to_x11keysym(modifier)) : 0;

    if (modifier != 0 && is_pressed)
    {
        XTestFakeKeyEvent(display, modcode, True, CurrentTime);
        XFlush(display);
    }

    if (keysym == BTN_LEFT || keysym == BTN_RIGHT)
        XTestFakeButtonEvent(display, x11_keysym, is_pressed ? True : False, 0);
    else
        XTestFakeKeyEvent(display, keycode, is_pressed ? True : False, CurrentTime);
    XFlush(display);

    if (modifier != 0 && !is_pressed)
    {
        XTestFakeKeyEvent(display, modcode, False, CurrentTime);
        XFlush(display);
    }
}


void emitAxisMotion_x11(int x, int y)
{
    XTestFakeMotionEvent(display, -1, x, y, CurrentTime); // -1 means relative to the current position
    XFlush(display);
}


void emitTextInputKey_x11(int code, bool uppercase)
{
    if (uppercase) { //capitalise capital letters by holding shift
        emitKey_x11(KEY_LEFTSHIFT, true);
    }
    emitKey_x11(code, true);
    SDL_Delay(16);

    emitKey_x11(code, false);
    SDL_Delay(16);
    if (uppercase) { //release shift if held
        emitKey_x11(KEY_LEFTSHIFT, false);
    }
}


void emitMouseMotion_x11(int x, int y)
{
    if (x != 0 || y != 0)
    {
        XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
        XFlush(display);
    }
}