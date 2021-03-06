/*
 * Copyright © 2005 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Red Hat, Inc. not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Red Hat, Inc. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * RED HAT, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL RED HAT, INC. BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carl Worth <cworth@cworth.org>
 */

#include "cairo-test.h"

#include <stdlib.h>

#define WIDTH 2
#define HEIGHT 2

static cairo_test_draw_function_t draw;

cairo_test_t test = {
    "create-from-png",
    "Tests the creation of an image surface from a PNG file",
    WIDTH, HEIGHT,
    draw
};

static cairo_status_t
no_memory_error (void *closure, unsigned char *data, unsigned int size)
{
    return CAIRO_STATUS_NO_MEMORY;
}

static cairo_status_t
read_error (void *closure, unsigned char *data, unsigned int size)
{
    return CAIRO_STATUS_READ_ERROR;
}

static cairo_test_status_t
draw (cairo_t *cr, int width, int height)
{
    char *srcdir = getenv ("srcdir");
    char *filename;
    cairo_surface_t *surface;

    xasprintf (&filename, "%s/%s", srcdir ? srcdir : ".",
	       "create-from-png-ref.png");

    surface = cairo_image_surface_create_from_png (filename);
    if (cairo_surface_status (surface)) {
	cairo_test_log ("Error reading PNG image %s: %s\n",
			filename,
			cairo_status_to_string (cairo_surface_status (surface)));
	free (filename);
	return CAIRO_TEST_FAILURE;
    }
    free (filename);

    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_paint (cr);

    cairo_surface_destroy (surface);

    return CAIRO_TEST_SUCCESS;
}

int
main (void)
{
    char *srcdir = getenv ("srcdir");
    char *filename;
    cairo_surface_t *surface;
    cairo_status_t status;

    surface = cairo_image_surface_create_from_png ("___THIS_FILE_DOES_NOT_EXIST___");
    if (cairo_surface_status (surface) != CAIRO_STATUS_FILE_NOT_FOUND) {
	cairo_test_log ("Error: expected \"file not found\", but got: %s\n",
			cairo_status_to_string (cairo_surface_status (surface)));
	cairo_surface_destroy (surface);
	return CAIRO_TEST_FAILURE;
    }

    surface = cairo_image_surface_create_from_png_stream (no_memory_error, NULL);
    if (cairo_surface_status (surface) != CAIRO_STATUS_NO_MEMORY) {
	cairo_test_log ("Error: expected \"out of memory\", but got: %s\n",
			cairo_status_to_string (cairo_surface_status (surface)));
	cairo_surface_destroy (surface);
	return CAIRO_TEST_FAILURE;
    }

    surface = cairo_image_surface_create_from_png_stream (read_error, NULL);
    if (cairo_surface_status (surface) != CAIRO_STATUS_READ_ERROR) {
	cairo_test_log ("Error: expected \"read error\", but got: %s\n",
			cairo_status_to_string (cairo_surface_status (surface)));
	cairo_surface_destroy (surface);
	return CAIRO_TEST_FAILURE;
    }

    /* cheekily test error propagation from the user write funcs as well ... */
    xasprintf (&filename, "%s/%s", srcdir ? srcdir : ".",
	       "create-from-png-ref.png");

    surface = cairo_image_surface_create_from_png (filename);
    if (cairo_surface_status (surface)) {
	cairo_test_log ("Error reading PNG image %s: %s\n",
			filename,
			cairo_status_to_string (cairo_surface_status (surface)));
	free (filename);
	return CAIRO_TEST_FAILURE;
    }
    free (filename);

    status = cairo_surface_write_to_png_stream (surface,
	                                   (cairo_write_func_t) no_memory_error,
					   NULL);
    if (status != CAIRO_STATUS_NO_MEMORY) {
	cairo_test_log ("Error: expected \"out of memory\", but got: %s\n",
			cairo_status_to_string (status));
	cairo_surface_destroy (surface);
	return CAIRO_TEST_FAILURE;
    }
    status = cairo_surface_write_to_png_stream (surface,
	                                        (cairo_write_func_t) read_error,
						NULL);
    if (status != CAIRO_STATUS_READ_ERROR) {
	cairo_test_log ("Error: expected \"read error\", but got: %s\n",
			cairo_status_to_string (status));
	cairo_surface_destroy (surface);
	return CAIRO_TEST_FAILURE;
    }
    cairo_surface_destroy (surface);

    return cairo_test (&test);
}
