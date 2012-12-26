/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * testgdi.c : simple test hack
 *
 * Authors:
 *	Vladimir Vukicevic <vladimir@pobox.com>
 *
 * Based on cairo test code.
 *
 * Modify the body of win_draw() to test your own code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

#include "GdiPlusFlat.h"
#include <X11/Xlib.h>

typedef struct win {
	Display *dpy;
	int scr;
	Window win;
	GC gc;
	int width, height;
	KeyCode quit_code;
	int color;
}
win_t;

static void win_init(win_t *win);
static void win_deinit(win_t *win);
static void win_draw(win_t *win);
static void win_handle_events(win_t *win);

#define CHECK_GDIP_ST(_st, _fail, _file)	do { if((!(_fail) && (_st) != Ok) || ((_fail) && (_st) == Ok)) { g_print("%s:%d: got st: %d expected %s%d (Test File: %s)\n", __FILE__, __LINE__, (_st), (_fail) ? "!" : "", Ok, (_file)); } } while (0)

static void
win_draw(win_t *win)
{
	const uint32_t COLORS[] = {
		0xffff0000,
		0xff00ff00,
		0xff0000ff,
		0xffffff00,
		0xffff00ff,
		0xff00ffff,
		0xffffffff,
		0xff000000
	};

	const struct {
		const char *name;
		int x;
		int y;
		int width;
		int height;
		int flags;
	} PNG_TESTS[] = {
		{ "png_suite/basn0g01.png", 0, 0, 32, 32, 0},
		{ "png_suite/basn0g02.png", 32, 0, 32, 32, 0},
		{ "png_suite/basn0g04.png", 64, 0, 32, 32, 0},
		{ "png_suite/basn0g08.png", 96, 0, 32, 32, 0},
		{ "png_suite/basn0g16.png", 128, 0, 32, 32, 2},
		{ "png_suite/basn2c08.png", 160, 0, 32, 32, 0},
		{ "png_suite/basn2c16.png", 192, 0, 32, 32, 2},
		{ "png_suite/basn3p01.png", 224, 0, 32, 32, 0},
		{ "png_suite/basn3p02.png", 256, 0, 32, 32, 0},
		{ "png_suite/basn3p04.png", 288, 0, 32, 32, 0},
		{ "png_suite/basn3p08.png", 320, 0, 32, 32, 0},
		{ "png_suite/basn4a08.png", 352, 0, 32, 32, 0},
		{ "png_suite/basn4a16.png", 384, 0, 32, 32, 2},
		{ "png_suite/basn6a08.png", 416, 0, 32, 32, 0},
		{ "png_suite/basn6a16.png", 448, 0, 32, 32, 2},

		{ "png_suite/basi0g01.png", 0, 32, 32, 32, 0},
		{ "png_suite/basi0g02.png", 32, 32, 32, 32, 0},
		{ "png_suite/basi0g04.png", 64, 32, 32, 32, 0},
		{ "png_suite/basi0g08.png", 96, 32, 32, 32, 0},
		{ "png_suite/basi0g16.png", 128, 32, 32, 32, 2},
		{ "png_suite/basi2c08.png", 160, 32, 32, 32, 0},
		{ "png_suite/basi2c16.png", 192, 32, 32, 32, 2},
		{ "png_suite/basi3p01.png", 224, 32, 32, 32, 0},
		{ "png_suite/basi3p02.png", 256, 32, 32, 32, 0},
		{ "png_suite/basi3p04.png", 288, 32, 32, 32, 0},
		{ "png_suite/basi3p08.png", 320, 32, 32, 32, 0},
		{ "png_suite/basi4a08.png", 352, 32, 32, 32, 0},
		{ "png_suite/basi4a16.png", 384, 32, 32, 32, 2},
		{ "png_suite/basi6a08.png", 416, 32, 32, 32, 0},
		{ "png_suite/basi6a16.png", 448, 32, 32, 32, 2},

		{ "png_suite/bgai4a08.png", 0, 64, 32, 32, 0},
		{ "png_suite/bgai4a16.png", 32, 64, 32, 32, 2},
		{ "png_suite/bgan6a08.png", 64, 64, 32, 32, 0},
		{ "png_suite/bgan6a16.png", 96, 64, 32, 32, 2},
		{ "png_suite/bgbn4a08.png", 128, 64, 32, 32, 0},
		{ "png_suite/bggn4a16.png", 160, 64, 32, 32, 2},
		{ "png_suite/bgwn6a08.png", 192, 64, 32, 32, 0},
		{ "png_suite/bgyn6a16.png", 224, 64, 32, 32, 2},

		{ "png_suite/ccwn2c08.png", 0, 96, 32, 32, 0},
		{ "png_suite/ccwn3p08.png", 32, 96, 32, 32, 0},
		{ "png_suite/cdfn2c08.png", 64, 96, 32, 32, 0},
		{ "png_suite/cdhn2c08.png", 96, 96, 32, 32, 0},
		{ "png_suite/cdsn2c08.png", 128, 96, 32, 32, 0},
		{ "png_suite/cdun2c08.png", 160, 96, 32, 32, 0},
		{ "png_suite/ch1n3p04.png", 192, 96, 32, 32, 0},
		{ "png_suite/ch2n3p08.png", 224, 96, 32, 32, 0},
		{ "png_suite/cm0n0g04.png", 256, 96, 32, 32, 0},
		{ "png_suite/cm7n0g04.png", 288, 96, 32, 32, 0},
		{ "png_suite/cm9n0g04.png", 320, 96, 32, 32, 0},

		{ "png_suite/cs3n2c16.png", 0, 128, 32, 32, 2},
		{ "png_suite/cs3n3p08.png", 32, 128, 32, 32, 0},
		{ "png_suite/cs5n2c08.png", 64, 128, 32, 32, 0},
		{ "png_suite/cs5n3p08.png", 96, 128, 32, 32, 0},
		{ "png_suite/cs8n2c08.png", 128, 128, 32, 32, 0},
		{ "png_suite/cs8n3p08.png", 160, 128, 32, 32, 0},
		{ "png_suite/ct0n0g04.png", 192, 128, 32, 32, 0},
		{ "png_suite/ct1n0g04.png", 224, 128, 32, 32, 0},
		{ "png_suite/cten0g04.png", 256, 128, 32, 32, 0},
		{ "png_suite/ctfn0g04.png", 288, 128, 32, 32, 0},
		{ "png_suite/ctgn0g04.png", 320, 128, 32, 32, 0},
		{ "png_suite/cthn0g04.png", 352, 128, 32, 32, 0},
		{ "png_suite/ctjn0g04.png", 384, 128, 32, 32, 0},
		{ "png_suite/ctzn0g04.png", 416, 128, 32, 32, 0},

		{ "png_suite/f00n0g08.png", 0, 160, 32, 32, 0},
		{ "png_suite/f00n2c08.png", 32, 160, 32, 32, 0},
		{ "png_suite/f01n0g08.png", 64, 160, 32, 32, 0},
		{ "png_suite/f01n2c08.png", 96, 160, 32, 32, 0},
		{ "png_suite/f02n0g08.png", 128, 160, 32, 32, 0},
		{ "png_suite/f02n2c08.png", 160, 160, 32, 32, 0},
		{ "png_suite/f03n0g08.png", 192, 160, 32, 32, 0},
		{ "png_suite/f03n2c08.png", 224, 160, 32, 32, 0},
		{ "png_suite/f04n0g08.png", 256, 160, 32, 32, 0},
		{ "png_suite/f04n2c08.png", 288, 160, 32, 32, 0},
		{ "png_suite/f99n0g04.png", 320, 160, 32, 32, 0},

		{ "png_suite/g03n0g16.png", 0, 192, 32, 32, 2},
		{ "png_suite/g03n2c08.png", 32, 192, 32, 32, 0},
		{ "png_suite/g03n3p04.png", 64, 192, 32, 32, 0},
		{ "png_suite/g04n0g16.png", 96, 192, 32, 32, 2},
		{ "png_suite/g04n2c08.png", 128, 192, 32, 32, 0},
		{ "png_suite/g04n3p04.png", 160, 192, 32, 32, 0},
		{ "png_suite/g05n0g16.png", 192, 192, 32, 32, 2},
		{ "png_suite/g05n2c08.png", 224, 192, 32, 32, 0},
		{ "png_suite/g05n3p04.png", 256, 192, 32, 32, 0},
		{ "png_suite/g07n0g16.png", 288, 192, 32, 32, 2},
		{ "png_suite/g07n2c08.png", 320, 192, 32, 32, 0},
		{ "png_suite/g07n3p04.png", 352, 192, 32, 32, 0},
		{ "png_suite/g10n0g16.png", 384, 192, 32, 32, 2},
		{ "png_suite/g10n2c08.png", 416, 192, 32, 32, 0},
		{ "png_suite/g10n3p04.png", 448, 192, 32, 32, 0},

		{ "png_suite/g25n0g16.png", 0, 224, 32, 32, 2},
		{ "png_suite/g25n2c08.png", 32, 224, 32, 32, 0},
		{ "png_suite/g25n3p04.png", 64, 224, 32, 32, 0},

		{ "png_suite/oi1n0g16.png", 0, 256, 32, 32, 2},
		{ "png_suite/oi1n2c16.png", 32, 256, 32, 32, 2},
		{ "png_suite/oi2n0g16.png", 64, 256, 32, 32, 2},
		{ "png_suite/oi2n2c16.png", 96, 256, 32, 32, 2},
		{ "png_suite/oi4n0g16.png", 128, 256, 32, 32, 2},
		{ "png_suite/oi4n2c16.png", 160, 256, 32, 32, 2},
		{ "png_suite/oi9n0g16.png", 192, 256, 32, 32, 2},
		{ "png_suite/oi9n2c16.png", 224, 256, 32, 32, 2},
		{ "png_suite/pp0n2c16.png", 256, 256, 32, 32, 2},
		{ "png_suite/pp0n6a08.png", 288, 256, 32, 32, 0},
		{ "png_suite/ps1n0g08.png", 320, 256, 32, 32, 0},
		{ "png_suite/ps1n2c16.png", 352, 256, 32, 32, 2},
		{ "png_suite/ps2n0g08.png", 384, 256, 32, 32, 0},
		{ "png_suite/ps2n2c16.png", 416, 256, 32, 32, 2},

		{ "png_suite/s01i3p01.png", 0, 288, 1, 1, 0},
		{ "png_suite/s01n3p01.png", 32, 288, 1, 1, 0},
		{ "png_suite/s02i3p01.png", 64, 288, 2, 2, 0},
		{ "png_suite/s02n3p01.png", 96, 288, 2, 2, 0},
		{ "png_suite/s03i3p01.png", 128, 288, 3, 3, 0},
		{ "png_suite/s03n3p01.png", 160, 288, 3, 3, 0},
		{ "png_suite/s04i3p01.png", 192, 288, 4, 4, 0},
		{ "png_suite/s04n3p01.png", 224, 288, 4, 4, 0},
		{ "png_suite/s05i3p02.png", 256, 288, 5, 5, 0},
		{ "png_suite/s05n3p02.png", 288, 288, 5, 5, 0},
		{ "png_suite/s06i3p02.png", 320, 288, 6, 6, 0},
		{ "png_suite/s06n3p02.png", 352, 288, 6, 6, 0},
		{ "png_suite/s07i3p02.png", 384, 288, 7, 7, 0},
		{ "png_suite/s07n3p02.png", 416, 288, 7, 7, 0},
		{ "png_suite/s08i3p02.png", 448, 288, 8, 8, 0},

		{ "png_suite/s08n3p02.png", 0, 320, 8, 8, 0},
		{ "png_suite/s09i3p02.png", 32, 320, 9, 9, 0},
		{ "png_suite/s09n3p02.png", 64, 320, 9, 9, 0},
		{ "png_suite/s32i3p04.png", 96, 320, 32, 32, 0},
		{ "png_suite/s32n3p04.png", 128, 320, 32, 32, 0},
		{ "png_suite/s33i3p04.png", 160, 320, 33, 33, 0},
		{ "png_suite/s33n3p04.png", 192, 320, 33, 33, 0},
		{ "png_suite/s34i3p04.png", 224, 320, 34, 34, 0},
		{ "png_suite/s34n3p04.png", 256, 320, 34, 34, 0},
		{ "png_suite/s35i3p04.png", 288, 320, 35, 35, 0},
		{ "png_suite/s35n3p04.png", 320, 320, 35, 35, 0},
		{ "png_suite/s36i3p04.png", 352, 320, 36, 36, 0},
		{ "png_suite/s36n3p04.png", 384, 320, 36, 36, 0},
		{ "png_suite/s37i3p04.png", 416, 320, 37, 37, 0},
		{ "png_suite/s37n3p04.png", 448, 320, 37, 37, 0},

		{ "png_suite/s38i3p04.png", 0, 360, 38, 38, 0},
		{ "png_suite/s38n3p04.png", 32, 360, 38, 38, 0},
		{ "png_suite/s39i3p04.png", 64, 360, 39, 39, 0},
		{ "png_suite/s39n3p04.png", 96, 360, 39, 39, 0},
		{ "png_suite/s40i3p04.png", 128, 360, 40, 40, 0},
		{ "png_suite/s40n3p04.png", 160, 360, 40, 40, 0},

		{ "png_suite/tbbn0g04.png", 0, 400, 32, 32, 0},
		{ "png_suite/tbbn2c16.png", 32, 400, 32, 32, 2},
		{ "png_suite/tbbn3p08.png", 64, 400, 32, 32, 0},
		{ "png_suite/tbgn2c16.png", 96, 400, 32, 32, 2},
		{ "png_suite/tbgn3p08.png", 128, 400, 32, 32, 0},
		{ "png_suite/tbrn2c08.png", 160, 400, 32, 32, 0},
		{ "png_suite/tbwn0g16.png", 192, 400, 32, 32, 2},
		{ "png_suite/tbwn3p08.png", 224, 400, 32, 32, 0},
		{ "png_suite/tbyn3p08.png", 256, 400, 32, 32, 0},
		{ "png_suite/tp0n0g08.png", 288, 400, 32, 32, 0},
		{ "png_suite/tp0n2c08.png", 320, 400, 32, 32, 0},
		{ "png_suite/tp0n3p08.png", 352, 400, 32, 32, 0},
		{ "png_suite/tp1n3p08.png", 384, 400, 32, 32, 0},

		{ "png_suite/z00n2c08.png", 0, 432, 32, 32, 0},
		{ "png_suite/z03n2c08.png", 32, 432, 32, 32, 0},
		{ "png_suite/z06n2c08.png", 64, 432, 32, 32, 0},
		{ "png_suite/z09n2c08.png", 96, 432, 32, 32, 0},

		{ "png_suite/xc1n0g08.png", 0, 464, 32, 32, 1},
		{ "png_suite/xc9n2c08.png", 32, 464, 32, 32, 1},
		{ "png_suite/xcrn0g04.png", 64, 464, 32, 32, 1},
		{ "png_suite/xcsn0g01.png", 96, 464, 32, 32, 1},
		{ "png_suite/xd0n2c08.png", 128, 464, 32, 32, 1},
		{ "png_suite/xd3n2c08.png", 160, 464, 32, 32, 1},
		{ "png_suite/xd9n2c08.png", 192, 464, 32, 32, 1},
		{ "png_suite/xdtn0g01.png", 224, 464, 32, 32, 1},
		{ "png_suite/xhdn0g08.png", 256, 464, 32, 32, 1},
		{ "png_suite/xlfn0g04.png", 288, 464, 32, 32, 1},
		{ "png_suite/xs1n0g01.png", 320, 464, 32, 32, 1},
		{ "png_suite/xs2n0g01.png", 352, 464, 32, 32, 1},
		{ "png_suite/xs4n0g01.png", 384, 464, 32, 32, 1},
		{ "png_suite/xs7n0g01.png", 416, 464, 32, 32, 1}
	};

	GpGraphics *gp;
	GpStatus st;
	GpImage *img;
	gunichar2 *unis;
	size_t i;
	GpSolidFill *brush;

	GdipCreateFromXDrawable_linux (win->win, win->dpy, &gp);

	GdipCreateSolidFill (COLORS[win->color % (sizeof(COLORS) / sizeof(COLORS[0]))], &brush);


	for (i = 0; i < (size_t)(sizeof(PNG_TESTS)/sizeof(PNG_TESTS[0])); i++) {
		img = NULL;
		unis = g_utf8_to_utf16 (PNG_TESTS[i].name, -1, NULL, NULL, NULL);
		GdipFillRectangle(gp, brush, PNG_TESTS[i].x, PNG_TESTS[i].y, PNG_TESTS[i].width, PNG_TESTS[i].height);

		st = GdipLoadImageFromFile (unis, &img);
		CHECK_GDIP_ST(st, PNG_TESTS[i].flags & 1, PNG_TESTS[i].name);
		if (st == Ok) {
			st = GdipDrawImage (gp, img, PNG_TESTS[i].x, PNG_TESTS[i].y);
			CHECK_GDIP_ST(st, 0, PNG_TESTS[i].name);
		}
		g_free (unis);
		GdipDisposeImage (img);
	}

	GdipDeleteBrush(brush);
	GdipDeleteGraphics(gp);
}

int
  main(int argc, char *argv[])
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	win_t win;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	win.color = 0;
	win.dpy = XOpenDisplay(0);

	if (win.dpy == NULL) {
		fprintf(stderr, "Failed to open display\n");
		return 1;
	}

	win_init(&win);

	win_draw(&win);

	win_handle_events(&win);

	win_deinit(&win);

	XCloseDisplay(win.dpy);

	GdiplusShutdown(gdiplusToken);
	return 0;
}

static void
  win_init(win_t *win)
{
	Window root;

	win->width = 480;
	win->height = 496;

	root = DefaultRootWindow(win->dpy);
	win->scr = DefaultScreen(win->dpy);

	win->win = XCreateSimpleWindow(win->dpy, root, 0, 0,
				       win->width, win->height, 0,
				       BlackPixel(win->dpy, win->scr), BlackPixel(win->dpy, win->scr));

	win->quit_code = XKeysymToKeycode(win->dpy, XStringToKeysym("Q"));

	XSelectInput(win->dpy, win->win,
		     KeyPressMask
		     |StructureNotifyMask
		     |ExposureMask);

	XMapWindow(win->dpy, win->win);
}

static void
  win_deinit(win_t *win)
{
	XDestroyWindow(win->dpy, win->win);
}

static void
  win_handle_events(win_t *win)
{
	XEvent xev;

	while (1) {
		XNextEvent(win->dpy, &xev);
		switch(xev.type) {
		 case KeyPress:
			{
				XKeyEvent *kev = &xev.xkey;

				if (kev->keycode == win->quit_code) {
					return;
				} else {
					win->color++;
					win_draw(win);
				}
			}
			break;
		 case ConfigureNotify:
			{
				XConfigureEvent *cev = &xev.xconfigure;

				win->width = cev->width;
				win->height = cev->height;
			}
			break;
		 case Expose:
			{
				XExposeEvent *eev = &xev.xexpose;

				if (eev->count == 0)
				  win_draw(win);
			}
			break;
		}
	}
}
