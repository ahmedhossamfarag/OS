#include "graphicslib.h"
#include "math.h"

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

#define GFX_MIN(a, b) ((a) < (b) ? (a) : (b))
#define GFX_MAX(a, b) ((a) > (b) ? (a) : (b))

#define GFX_SWAP(a, b) do { int _t = (a); (a) = (b); (b) = _t; } while (0)

/* Integer square root (floor) */
static int gfx_isqrt(int n) {
    if (n <= 0) return 0;
    int lo = 0, hi = n < 46340 ? n : 46340; /* 46340^2 fits in int32 */
    while (lo < hi) {
        int mid = (lo + hi + 1) / 2;
        if (mid * mid <= n) lo = mid;
        else hi = mid - 1;
    }
    return lo;
}

/* Corner masks used by the rounded-rect helpers */
#define CORNER_TR 1
#define CORNER_TL 2
#define CORNER_BL 4
#define CORNER_BR 8

/* ------------------------------------------------------------------ */
/* Pixels                                                              */
/* ------------------------------------------------------------------ */

void set_pixel(window_t* window, int x, int y, uint32_t color) {
    if ((uint32_t)x < window->bounds.width && (uint32_t)y < window->bounds.height) {
        window->buffer[y * window->bounds.width + x] = color;
    }
}

uint32_t get_pixel(window_t* window, int x, int y) {
    if ((uint32_t)x < window->bounds.width && (uint32_t)y < window->bounds.height) {
        return window->buffer[y * window->bounds.width + x];
    }
    return 0;
}

/* Blend `color` (0xAARRGGBB) over the existing pixel using its alpha byte */
void set_pixel_alpha(window_t* window, int x, int y, uint32_t color) {
    if ((uint32_t)x >= window->bounds.width || (uint32_t)y >= window->bounds.height)
        return;

    uint32_t a = color >> 24;
    if (a == 255) { window->buffer[y * window->bounds.width + x] = color | 0xFF000000; return; }
    if (a == 0) return;

    uint32_t dst = window->buffer[y * window->bounds.width + x];
    uint32_t ia = 255 - a;

    uint32_t r = (((color >> 16) & 0xFF) * a + ((dst >> 16) & 0xFF) * ia) / 255;
    uint32_t g = (((color >> 8)  & 0xFF) * a + ((dst >> 8)  & 0xFF) * ia) / 255;
    uint32_t b = ((color         & 0xFF) * a + (dst         & 0xFF) * ia) / 255;

    window->buffer[y * window->bounds.width + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
}

void clear_window(window_t* window, uint32_t color) {
    uint32_t total = window->bounds.width * window->bounds.height;
    for (uint32_t i = 0; i < total; i++)
        window->buffer[i] = color;
}

/* ------------------------------------------------------------------ */
/* Lines                                                               */
/* ------------------------------------------------------------------ */

/* Fast clipped horizontal line (inclusive x0..x1) */
void draw_hline(window_t* window, int x0, int x1, int y, uint32_t color) {
    if (x0 > x1) GFX_SWAP(x0, x1);
    if (y < 0 || (uint32_t)y >= window->bounds.height) return;
    if (x1 < 0 || x0 >= (int)window->bounds.width) return;
    if (x0 < 0) x0 = 0;
    if (x1 >= (int)window->bounds.width) x1 = window->bounds.width - 1;

    uint32_t* p = &window->buffer[y * window->bounds.width + x0];
    for (int x = x0; x <= x1; x++) *p++ = color;
}

/* Fast clipped vertical line (inclusive y0..y1) */
void draw_vline(window_t* window, int x, int y0, int y1, uint32_t color) {
    if (y0 > y1) GFX_SWAP(y0, y1);
    if (x < 0 || (uint32_t)x >= window->bounds.width) return;
    if (y1 < 0 || y0 >= (int)window->bounds.height) return;
    if (y0 < 0) y0 = 0;
    if (y1 >= (int)window->bounds.height) y1 = window->bounds.height - 1;

    for (int y = y0; y <= y1; y++)
        window->buffer[y * window->bounds.width + x] = color;
}

/* Bresenham's Line Algorithm */
void draw_line(window_t* window, int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = math_abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -math_abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        set_pixel(window, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

/* Line with thickness (draws a filled square brush along the line) */
void draw_thick_line(window_t* window, int x0, int y0, int x1, int y1,
                     int thickness, uint32_t color) {
    if (thickness <= 1) { draw_line(window, x0, y0, x1, y1, color); return; }

    int dx = math_abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -math_abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    int half = thickness / 2;

    while (1) {
        fill_rect(window, x0 - half, y0 - half, thickness, thickness, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

/* ------------------------------------------------------------------ */
/* Rectangles                                                          */
/* ------------------------------------------------------------------ */

void draw_rectangle(window_t* window, int x, int y, int width, int height, uint32_t color) {
    if (width <= 0 || height <= 0) return;
    draw_hline(window, x, x + width - 1, y, color);
    draw_hline(window, x, x + width - 1, y + height - 1, color);
    draw_vline(window, x, y, y + height - 1, color);
    draw_vline(window, x + width - 1, y, y + height - 1, color);
}

void fill_rect(window_t* window, int x, int y, int width, int height, uint32_t color) {
    if (width <= 0 || height <= 0) return;
    for (int j = 0; j < height; j++)
        draw_hline(window, x, x + width - 1, y + j, color);
}

/* Filled rectangle with alpha blending (color = 0xAARRGGBB) */
void fill_rect_alpha(window_t* window, int x, int y, int width, int height, uint32_t color) {
    int x0 = GFX_MAX(x, 0), y0 = GFX_MAX(y, 0);
    int x1 = GFX_MIN(x + width,  (int)window->bounds.width);
    int y1 = GFX_MIN(y + height, (int)window->bounds.height);
    for (int j = y0; j < y1; j++)
        for (int i = x0; i < x1; i++)
            set_pixel_alpha(window, i, j, color);
}

/* Rectangle outline with custom border thickness (drawn inward) */
void draw_rectangle_thick(window_t* window, int x, int y, int width, int height,
                          int thickness, uint32_t color) {
    for (int i = 0; i < thickness; i++)
        draw_rectangle(window, x + i, y + i, width - 2 * i, height - 2 * i, color);
}

/* ------------------------------------------------------------------ */
/* Rounded rectangles                                                  */
/* ------------------------------------------------------------------ */

/* Draw selected quadrants of a midpoint circle */
static void circle_corners(window_t* window, int xc, int yc, int radius,
                           int mask, uint32_t color) {
    int x = radius, y = 0;
    int p = 1 - radius;

    while (x >= y) {
        if (mask & CORNER_BR) { set_pixel(window, xc + x, yc + y, color); set_pixel(window, xc + y, yc + x, color); }
        if (mask & CORNER_BL) { set_pixel(window, xc - y, yc + x, color); set_pixel(window, xc - x, yc + y, color); }
        if (mask & CORNER_TL) { set_pixel(window, xc - x, yc - y, color); set_pixel(window, xc - y, yc - x, color); }
        if (mask & CORNER_TR) { set_pixel(window, xc + y, yc - x, color); set_pixel(window, xc + x, yc - y, color); }
        y++;
        if (p <= 0) {
            p = p + 2 * y + 1;
        } else {
            x--;
            p = p + 2 * y - 2 * x + 1;
        }
    }
}

void draw_rounded_rect(window_t* window, int x, int y, int width, int height,
                       int radius, uint32_t color) {
    if (width <= 0 || height <= 0) return;
    int maxr = GFX_MIN(width, height) / 2;
    if (radius > maxr) radius = maxr;
    if (radius <= 0) { draw_rectangle(window, x, y, width, height, color); return; }

    int x1 = x + width - 1, y1 = y + height - 1;

    draw_hline(window, x + radius, x1 - radius, y,  color);
    draw_hline(window, x + radius, x1 - radius, y1, color);
    draw_vline(window, x,  y + radius, y1 - radius, color);
    draw_vline(window, x1, y + radius, y1 - radius, color);

    circle_corners(window, x + radius,  y + radius,  radius, CORNER_TL, color);
    circle_corners(window, x1 - radius, y + radius,  radius, CORNER_TR, color);
    circle_corners(window, x + radius,  y1 - radius, radius, CORNER_BL, color);
    circle_corners(window, x1 - radius, y1 - radius, radius, CORNER_BR, color);
}

void fill_rounded_rect(window_t* window, int x, int y, int width, int height,
                       int radius, uint32_t color) {
    if (width <= 0 || height <= 0) return;
    int maxr = GFX_MIN(width, height) / 2;
    if (radius > maxr) radius = maxr;
    if (radius <= 0) { fill_rect(window, x, y, width, height, color); return; }

    for (int row = 0; row < height; row++) {
        int inset = 0;
        if (row < radius) {
            int dy = radius - row;                       /* distance from arc centre row */
            inset = radius - gfx_isqrt(radius * radius - dy * dy);
        } else if (row >= height - radius) {
            int dy = row - (height - radius - 1);
            inset = radius - gfx_isqrt(radius * radius - dy * dy);
        }
        draw_hline(window, x + inset, x + width - 1 - inset, y + row, color);
    }
}

/* ------------------------------------------------------------------ */
/* Circles                                                             */
/* ------------------------------------------------------------------ */

/* Midpoint Circle Algorithm */
void draw_circle(window_t* window, int xc, int yc, int radius, uint32_t color) {
    if (radius < 0) return;
    int x = radius, y = 0;
    int p = 1 - radius;

    while (x >= y) {
        set_pixel(window, xc + x, yc + y, color);
        set_pixel(window, xc + y, yc + x, color);
        set_pixel(window, xc - y, yc + x, color);
        set_pixel(window, xc - x, yc + y, color);
        set_pixel(window, xc - x, yc - y, color);
        set_pixel(window, xc - y, yc - x, color);
        set_pixel(window, xc + y, yc - x, color);
        set_pixel(window, xc + x, yc - y, color);
        y++;
        if (p <= 0) {
            p = p + 2 * y + 1;
        } else {
            x--;
            p = p + 2 * y - 2 * x + 1;
        }
    }
}

/* Filled circle: one horizontal span per scanline (no overdraw) */
void fill_circle(window_t* window, int xc, int yc, int radius, uint32_t color) {
    if (radius < 0) return;
    int r2 = radius * radius;
    for (int dy = 0; dy <= radius; dy++) {
        int dx = gfx_isqrt(r2 - dy * dy);
        draw_hline(window, xc - dx, xc + dx, yc + dy, color);
        if (dy != 0)
            draw_hline(window, xc - dx, xc + dx, yc - dy, color);
    }
}

/* Circle ring with thickness */
void draw_ring(window_t* window, int xc, int yc, int outer_r, int inner_r, uint32_t color) {
    if (inner_r < 0) inner_r = 0;
    if (inner_r >= outer_r) return;
    int o2 = outer_r * outer_r, i2 = inner_r * inner_r;
    for (int dy = 0; dy <= outer_r; dy++) {
        int ox = gfx_isqrt(o2 - dy * dy);
        int ix = (dy < inner_r) ? gfx_isqrt(i2 - dy * dy) : -1;
        for (int s = -1; s <= 1; s += 2) {
            if (dy == 0 && s == -1) continue;
            int yy = yc + s * dy;
            if (ix < 0) {
                draw_hline(window, xc - ox, xc + ox, yy, color);
            } else {
                draw_hline(window, xc - ox, xc - ix - 1, yy, color);
                draw_hline(window, xc + ix + 1, xc + ox, yy, color);
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/* Ellipses                                                            */
/* ------------------------------------------------------------------ */

static void ellipse_plot4(window_t* window, int xc, int yc, int x, int y, uint32_t color) {
    set_pixel(window, xc + x, yc + y, color);
    set_pixel(window, xc - x, yc + y, color);
    set_pixel(window, xc + x, yc - y, color);
    set_pixel(window, xc - x, yc - y, color);
}

/* Midpoint Ellipse Algorithm */
void draw_ellipse(window_t* window, int xc, int yc, int rx, int ry, uint32_t color) {
    if (rx < 0 || ry < 0) return;
    long long rx2 = (long long)rx * rx, ry2 = (long long)ry * ry;
    long long x = 0, y = ry;
    long long px = 0, py = 2 * rx2 * y;
    long long p = ry2 - rx2 * ry + rx2 / 4;

    /* Region 1 */
    while (px < py) {
        ellipse_plot4(window, xc, yc, (int)x, (int)y, color);
        x++;
        px += 2 * ry2;
        if (p < 0) {
            p += ry2 + px;
        } else {
            y--;
            py -= 2 * rx2;
            p += ry2 + px - py;
        }
    }

    /* Region 2 */
    p = (ry2 * (2 * x + 1) * (2 * x + 1)) / 4 + rx2 * (y - 1) * (y - 1) - rx2 * ry2;
    while (y >= 0) {
        ellipse_plot4(window, xc, yc, (int)x, (int)y, color);
        y--;
        py -= 2 * rx2;
        if (p > 0) {
            p += rx2 - py;
        } else {
            x++;
            px += 2 * ry2;
            p += rx2 - py + px;
        }
    }
}

void fill_ellipse(window_t* window, int xc, int yc, int rx, int ry, uint32_t color) {
    if (rx < 0 || ry < 0) return;
    long long rx2 = (long long)rx * rx, ry2 = (long long)ry * ry;
    long long limit = rx2 * ry2;
    int x = rx;

    for (int dy = 0; dy <= ry; dy++) {
        while (x > 0 && (long long)x * x * ry2 + (long long)dy * dy * rx2 > limit)
            x--;
        draw_hline(window, xc - x, xc + x, yc + dy, color);
        if (dy != 0)
            draw_hline(window, xc - x, xc + x, yc - dy, color);
    }
}

/* ------------------------------------------------------------------ */
/* Triangles & polygons                                                */
/* ------------------------------------------------------------------ */

void draw_triangle(window_t* window, int x0, int y0, int x1, int y1,
                   int x2, int y2, uint32_t color) {
    draw_line(window, x0, y0, x1, y1, color);
    draw_line(window, x1, y1, x2, y2, color);
    draw_line(window, x2, y2, x0, y0, color);
}

void fill_triangle(window_t* window, int x0, int y0, int x1, int y1,
                   int x2, int y2, uint32_t color) {
    /* Sort vertices by y (y0 <= y1 <= y2) */
    if (y0 > y1) { GFX_SWAP(y0, y1); GFX_SWAP(x0, x1); }
    if (y1 > y2) { GFX_SWAP(y1, y2); GFX_SWAP(x1, x2); }
    if (y0 > y1) { GFX_SWAP(y0, y1); GFX_SWAP(x0, x1); }

    if (y0 == y2) { /* degenerate: all on one row */
        int minx = GFX_MIN(x0, GFX_MIN(x1, x2));
        int maxx = GFX_MAX(x0, GFX_MAX(x1, x2));
        draw_hline(window, minx, maxx, y0, color);
        return;
    }

    int total_h = y2 - y0;
    for (int y = y0; y <= y2; y++) {
        int second_half = (y > y1) || (y1 == y0);
        int seg_h = second_half ? (y2 - y1) : (y1 - y0);
        if (seg_h == 0) seg_h = 1;

        int xa = x0 + (x2 - x0) * (y - y0) / total_h;
        int xb = second_half
               ? x1 + (x2 - x1) * (y - y1) / seg_h
               : x0 + (x1 - x0) * (y - y0) / seg_h;

        draw_hline(window, xa, xb, y, color);
    }
}

/* Polygon outline; pts = {x0,y0, x1,y1, ...} */
void draw_polygon(window_t* window, const int* pts, int count, uint32_t color) {
    if (count < 2) return;
    for (int i = 0; i < count; i++) {
        int j = (i + 1) % count;
        draw_line(window, pts[2 * i], pts[2 * i + 1], pts[2 * j], pts[2 * j + 1], color);
    }
}

/* Even-odd scanline fill for arbitrary polygons (max 64 edge crossings/row) */
void fill_polygon(window_t* window, const int* pts, int count, uint32_t color) {
    if (count < 3) return;

    int miny = pts[1], maxy = pts[1];
    for (int i = 1; i < count; i++) {
        miny = GFX_MIN(miny, pts[2 * i + 1]);
        maxy = GFX_MAX(maxy, pts[2 * i + 1]);
    }
    if (miny < 0) miny = 0;
    if (maxy >= (int)window->bounds.height) maxy = window->bounds.height - 1;

    for (int y = miny; y <= maxy; y++) {
        int nodes[64], n = 0;
        for (int i = 0, j = count - 1; i < count; j = i++) {
            int xi = pts[2 * i], yi = pts[2 * i + 1];
            int xj = pts[2 * j], yj = pts[2 * j + 1];
            if ((yi < y && yj >= y) || (yj < y && yi >= y)) {
                if (n < 64)
                    nodes[n++] = xi + (y - yi) * (xj - xi) / (yj - yi);
            }
        }
        /* insertion sort */
        for (int a = 1; a < n; a++) {
            int v = nodes[a], b = a - 1;
            while (b >= 0 && nodes[b] > v) { nodes[b + 1] = nodes[b]; b--; }
            nodes[b + 1] = v;
        }
        for (int a = 0; a + 1 < n; a += 2)
            draw_hline(window, nodes[a], nodes[a + 1], y, color);
    }
}

/* ------------------------------------------------------------------ */
/* Gradients                                                           */
/* ------------------------------------------------------------------ */

static uint32_t lerp_color(uint32_t c0, uint32_t c1, int t, int max) {
    if (max <= 0) return c0;
    int r = (int)((c0 >> 16) & 0xFF) + ((int)((c1 >> 16) & 0xFF) - (int)((c0 >> 16) & 0xFF)) * t / max;
    int g = (int)((c0 >> 8)  & 0xFF) + ((int)((c1 >> 8)  & 0xFF) - (int)((c0 >> 8)  & 0xFF)) * t / max;
    int b = (int)(c0 & 0xFF)         + ((int)(c1 & 0xFF)         - (int)(c0 & 0xFF))         * t / max;
    return 0xFF000000 | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

void fill_gradient_v(window_t* window, int x, int y, int width, int height,
                     uint32_t top, uint32_t bottom) {
    for (int j = 0; j < height; j++)
        draw_hline(window, x, x + width - 1, y + j, lerp_color(top, bottom, j, height - 1));
}

void fill_gradient_h(window_t* window, int x, int y, int width, int height,
                     uint32_t left, uint32_t right) {
    for (int i = 0; i < width; i++)
        draw_vline(window, x + i, y, y + height - 1, lerp_color(left, right, i, width - 1));
}

/* ------------------------------------------------------------------ */
/* Bitmaps / blitting                                                  */
/* ------------------------------------------------------------------ */

/* Copy a w*h ARGB bitmap to the window at (x,y) */
void draw_bitmap(window_t* window, int x, int y, const uint32_t* bitmap, int w, int h) {
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            set_pixel(window, x + i, y + j, bitmap[j * w + i]);
}

/* Same, but blends using each pixel's alpha and skips fully transparent ones */
void draw_bitmap_alpha(window_t* window, int x, int y, const uint32_t* bitmap, int w, int h) {
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            set_pixel_alpha(window, x + i, y + j, bitmap[j * w + i]);
}

/* Copy a region from one window to another */
void blit(window_t* dst, int dx, int dy, window_t* src, int sx, int sy, int w, int h) {
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            set_pixel(dst, dx + i, dy + j, get_pixel(src, sx + i, sy + j));
}

/* ------------------------------------------------------------------ */
/* Flood fill (iterative scanline, uses caller-provided stack)         */
/* ------------------------------------------------------------------ */

/* stack must hold at least 2 * stack_capacity ints. Returns 0 on success,
   -1 if the stack overflowed (fill is then partial). */
int flood_fill(window_t* window, int x, int y, uint32_t new_color,
               int* stack, int stack_capacity) {
    uint32_t target = get_pixel(window, x, y);
    if (target == new_color) return 0;
    if ((uint32_t)x >= window->bounds.width || (uint32_t)y >= window->bounds.height) return 0;

    int sp = 0;
    stack[sp * 2] = x; stack[sp * 2 + 1] = y; sp++;

    while (sp > 0) {
        sp--;
        int cx = stack[sp * 2], cy = stack[sp * 2 + 1];
        if (get_pixel(window, cx, cy) != target) continue;

        int l = cx, r = cx;
        while (l > 0 && get_pixel(window, l - 1, cy) == target) l--;
        while (r < (int)window->bounds.width - 1 && get_pixel(window, r + 1, cy) == target) r++;

        draw_hline(window, l, r, cy, new_color);

        for (int dy = -1; dy <= 1; dy += 2) {
            int ny = cy + dy;
            if (ny < 0 || ny >= (int)window->bounds.height) continue;
            int in_span = 0;
            for (int i = l; i <= r; i++) {
                if (get_pixel(window, i, ny) == target) {
                    if (!in_span) {
                        if (sp >= stack_capacity) return -1;
                        stack[sp * 2] = i; stack[sp * 2 + 1] = ny; sp++;
                        in_span = 1;
                    }
                } else {
                    in_span = 0;
                }
            }
        }
    }
    return 0;
}