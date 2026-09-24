#include "stdint.h"
#include "graphicslib.h"

/* ------------------------------------------------------------------ */
/* Palette (Ubuntu / Yaru inspired, 0xAARRGGBB)                        */
/* ------------------------------------------------------------------ */
#define C_ORANGE      0xFFE95420
#define C_AUBERGINE   0xFF772953
#define C_DARK_AUB    0xFF2C001E
#define C_TERM_BG     0xFF300A24
#define C_TITLE_TOP   0xFF3E3D39
#define C_TITLE_BOT   0xFF2C2B28
#define C_SIDEBAR     0xFF383736
#define C_SIDE_TEXT   0xFFE6E6E6
#define C_SIDE_DIM    0xFF8F8E8A
#define C_MAIN_BG     0xFFFAFAFA
#define C_CARD_BORDER 0xFFDDDDDD
#define C_GREEN       0xFF8AE234
#define C_BLUE        0xFF729FCF
#define C_CYAN        0xFF19B6EE
#define C_LEAF        0xFF3EB34F
#define C_WHITE       0xFFFFFFFF
#define C_TEXT_DARK   0xFF333333
#define C_TEXT_MUTED  0xFF777777
#define C_BORDER      0xFF1A1918

/* ------------------------------------------------------------------ */
/* 8x8 bitmap font, ASCII 32..126 (public domain "font8x8_basic")      */
/* Bit 0 of each byte = leftmost pixel.                                */
/* ------------------------------------------------------------------ */
static const uint8_t font8x8[95][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* ' ' */
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, /* ! */
    {0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00}, /* " */
    {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00}, /* # */
    {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00}, /* $ */
    {0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00}, /* % */
    {0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00}, /* & */
    {0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00}, /* ' */
    {0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00}, /* ( */
    {0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00}, /* ) */
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, /* * */
    {0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00}, /* + */
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x06}, /* , */
    {0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00}, /* - */
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00}, /* . */
    {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00}, /* / */
    {0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00}, /* 0 */
    {0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00}, /* 1 */
    {0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00}, /* 2 */
    {0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00}, /* 3 */
    {0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00}, /* 4 */
    {0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00}, /* 5 */
    {0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00}, /* 6 */
    {0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00}, /* 7 */
    {0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00}, /* 8 */
    {0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00}, /* 9 */
    {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x00}, /* : */
    {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x06}, /* ; */
    {0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00}, /* < */
    {0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00}, /* = */
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, /* > */
    {0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00}, /* ? */
    {0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00}, /* @ */
    {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00}, /* A */
    {0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00}, /* B */
    {0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00}, /* C */
    {0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00}, /* D */
    {0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00}, /* E */
    {0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00}, /* F */
    {0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00}, /* G */
    {0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00}, /* H */
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, /* I */
    {0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00}, /* J */
    {0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00}, /* K */
    {0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00}, /* L */
    {0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00}, /* M */
    {0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00}, /* N */
    {0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00}, /* O */
    {0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00}, /* P */
    {0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00}, /* Q */
    {0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00}, /* R */
    {0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00}, /* S */
    {0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, /* T */
    {0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00}, /* U */
    {0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00}, /* V */
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, /* W */
    {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00}, /* X */
    {0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00}, /* Y */
    {0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00}, /* Z */
    {0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00}, /* [ */
    {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00}, /* \ */
    {0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00}, /* ] */
    {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00}, /* ^ */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, /* _ */
    {0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00}, /* ` */
    {0x00,0x00,0x1E,0x30,0x3E,0x33,0x6E,0x00}, /* a */
    {0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0x00}, /* b */
    {0x00,0x00,0x1E,0x33,0x03,0x33,0x1E,0x00}, /* c */
    {0x38,0x30,0x30,0x3E,0x33,0x33,0x6E,0x00}, /* d */
    {0x00,0x00,0x1E,0x33,0x3F,0x03,0x1E,0x00}, /* e */
    {0x1C,0x36,0x06,0x0F,0x06,0x06,0x0F,0x00}, /* f */
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x1F}, /* g */
    {0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0x00}, /* h */
    {0x0C,0x00,0x0E,0x0C,0x0C,0x0C,0x1E,0x00}, /* i */
    {0x30,0x00,0x30,0x30,0x30,0x33,0x33,0x1E}, /* j */
    {0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0x00}, /* k */
    {0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, /* l */
    {0x00,0x00,0x33,0x7F,0x7F,0x6B,0x63,0x00}, /* m */
    {0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00}, /* n */
    {0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00}, /* o */
    {0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F}, /* p */
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78}, /* q */
    {0x00,0x00,0x3B,0x6E,0x66,0x06,0x0F,0x00}, /* r */
    {0x00,0x00,0x3E,0x03,0x1E,0x30,0x1F,0x00}, /* s */
    {0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0x00}, /* t */
    {0x00,0x00,0x33,0x33,0x33,0x33,0x6E,0x00}, /* u */
    {0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00}, /* v */
    {0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00}, /* w */
    {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00}, /* x */
    {0x00,0x00,0x33,0x33,0x33,0x3E,0x30,0x1F}, /* y */
    {0x00,0x00,0x3F,0x19,0x0C,0x26,0x3F,0x00}, /* z */
    {0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00}, /* { */
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, /* | */
    {0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00}, /* } */
    {0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00}  /* ~ */
};

/* ------------------------------------------------------------------ */
/* Text helpers                                                        */
/* ------------------------------------------------------------------ */

static void ui_char(window_t* w, int x, int y, char c, uint32_t color, int scale) {
    if (c < 32 || c > 126) return;
    const uint8_t* g = font8x8[c - 32];
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (g[row] & (1 << col)) {
                if (scale == 1) set_pixel(w, x + col, y + row, color);
                else fill_rect(w, x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

static int ui_text_width(const char* s, int scale) {
    int n = 0;
    while (*s++) n++;
    return n * 8 * scale;
}

/* Draws text and returns the x position after the last character */
static int ui_text(window_t* w, int x, int y, const char* s, uint32_t color, int scale) {
    while (*s) {
        ui_char(w, x, y, *s++, color, scale);
        x += 8 * scale;
    }
    return x;
}

static void ui_text_centered(window_t* w, int cx, int y, const char* s, uint32_t color, int scale) {
    ui_text(w, cx - ui_text_width(s, scale) / 2, y, s, color, scale);
}

static void u32_to_str(uint32_t v, char* out) {
    char tmp[11];
    int n = 0;
    if (v == 0) tmp[n++] = '0';
    while (v) { tmp[n++] = (char)('0' + v % 10); v /= 10; }
    for (int i = 0; i < n; i++) out[i] = tmp[n - 1 - i];
    out[n] = '\0';
}

/* ------------------------------------------------------------------ */
/* Title bar                                                           */
/* ------------------------------------------------------------------ */

static void draw_title_bar(window_t* w, int W, int title_h) {
    fill_gradient_v(w, 0, 0, W, title_h, C_TITLE_TOP, C_TITLE_BOT);
    draw_hline(w, 0, W - 1, title_h - 1, C_BORDER);

    /* App icon */
    fill_circle(w, 22, title_h / 2, 9, C_ORANGE);
    draw_ring(w, 22, title_h / 2, 5, 3, C_WHITE);

    /* Title */
    ui_text_centered(w, W / 2, (title_h - 16) / 2, "Welcome to MyOS", C_WHITE, 2);

    /* Window buttons (Yaru style, right side) */
    int cy = title_h / 2;
    int cx_close = W - 24, cx_max = W - 56, cx_min = W - 88;

    fill_circle(w, cx_min, cy, 9, 0xFF5A5955);
    draw_hline(w, cx_min - 4, cx_min + 4, cy + 3, C_WHITE);

    fill_circle(w, cx_max, cy, 9, 0xFF5A5955);
    draw_rectangle(w, cx_max - 4, cy - 4, 9, 9, C_WHITE);

    fill_circle(w, cx_close, cy, 9, C_ORANGE);
    draw_line(w, cx_close - 4, cy - 4, cx_close + 4, cy + 4, C_WHITE);
    draw_line(w, cx_close - 4, cy + 4, cx_close + 4, cy - 4, C_WHITE);
}

/* ------------------------------------------------------------------ */
/* Sidebar                                                             */
/* ------------------------------------------------------------------ */

static void draw_icon(window_t* w, int kind, int ix, int iy, uint32_t fg, uint32_t bg) {
    switch (kind) {
    case 0: /* Home */
        fill_triangle(w, ix + 7, iy, ix, iy + 7, ix + 14, iy + 7, fg);
        fill_rect(w, ix + 2, iy + 7, 10, 7, fg);
        fill_rect(w, ix + 6, iy + 9, 3, 5, bg);
        break;
    case 1: /* Documents */
        fill_rect(w, ix + 2, iy, 10, 14, fg);
        draw_hline(w, ix + 4, ix + 9, iy + 4, bg);
        draw_hline(w, ix + 4, ix + 9, iy + 7, bg);
        draw_hline(w, ix + 4, ix + 9, iy + 10, bg);
        break;
    case 2: /* Downloads */
        fill_rect(w, ix + 5, iy, 4, 7, fg);
        fill_triangle(w, ix, iy + 7, ix + 14, iy + 7, ix + 7, iy + 14, fg);
        break;
    case 3: /* Pictures */
        draw_rectangle(w, ix, iy + 1, 14, 12, fg);
        fill_circle(w, ix + 4, iy + 5, 2, fg);
        fill_triangle(w, ix + 3, iy + 11, ix + 8, iy + 6, ix + 12, iy + 11, fg);
        break;
    case 4: /* Terminal */
        draw_rectangle(w, ix, iy + 1, 14, 12, fg);
        draw_line(w, ix + 3, iy + 4, ix + 6, iy + 7, fg);
        draw_line(w, ix + 6, iy + 7, ix + 3, iy + 10, fg);
        draw_hline(w, ix + 8, ix + 11, iy + 10, fg);
        break;
    default: /* Settings */
        draw_ring(w, ix + 7, iy + 7, 7, 3, fg);
        break;
    }
}

static void draw_sidebar(window_t* w, int sb_w, int body_y, int body_h) {
    static const char* names[6] = {
        "Home", "Documents", "Downloads", "Pictures", "Terminal", "Settings"
    };
    int selected = 0;

    fill_rect(w, 0, body_y, sb_w, body_h, C_SIDEBAR);
    draw_vline(w, sb_w - 1, body_y, body_y + body_h - 1, C_BORDER);

    ui_text(w, 16, body_y + 14, "PLACES", C_SIDE_DIM, 1);

    for (int i = 0; i < 6; i++) {
        int ry = body_y + 34 + i * 36;
        uint32_t bg = (i == selected) ? C_ORANGE : C_SIDEBAR;
        uint32_t fg = (i == selected) ? C_WHITE  : C_SIDE_TEXT;

        if (i == selected)
            fill_rounded_rect(w, 8, ry, sb_w - 16, 30, 6, bg);

        draw_icon(w, i, 20, ry + 8, fg, bg);
        ui_text(w, 44, ry + 7, names[i], fg, 2);
    }
}

/* ------------------------------------------------------------------ */
/* Main content                                                        */
/* ------------------------------------------------------------------ */

static void draw_banner(window_t* w, int bx, int by, int bw, int bh) {
    fill_gradient_h(w, bx, by, bw, bh, C_AUBERGINE, C_DARK_AUB);

    /* Decorative shapes on the right */
    int cx = bx + bw - 90, cy = by + bh / 2;
    draw_ring(w, cx, cy, 55, 52, 0xFFA0507F);
    draw_ring(w, cx, cy, 44, 42, 0xFF8E3E6C);
    fill_circle(w, cx, cy, 30, C_ORANGE);
    fill_triangle(w, cx - 8, cy - 14, cx - 8, cy + 14, cx + 14, cy, C_WHITE);

    /* Text */
    ui_text(w, bx + 24, by + 22, "Welcome!", C_WHITE, 4);
    ui_text(w, bx + 24, by + 66, "Your desktop is ready.", 0xFFF3D9E6, 2);
    ui_text(w, bx + 24, by + 98, "Drawn 100% with the custom graphics library", 0xFFD9A8C3, 1);
}

static void draw_card(window_t* w, int x, int y, int cw, int ch, uint32_t accent,
                      const char* title, const char* line1, const char* line2) {
    fill_rect_alpha(w, x + 2, y + 4, cw, ch, 0x22000000);      /* soft shadow */
    fill_rounded_rect(w, x, y, cw, ch, 8, C_WHITE);
    draw_rounded_rect(w, x, y, cw, ch, 8, C_CARD_BORDER);

    fill_circle(w, x + 28, y + 26, 12, accent);
    draw_ring(w, x + 28, y + 26, 6, 4, C_WHITE);

    ui_text(w, x + 16, y + 48, title, C_TEXT_DARK, 2);
    ui_text(w, x + 16, y + 74, line1, C_TEXT_MUTED, 1);
    ui_text(w, x + 16, y + 86, line2, C_TEXT_MUTED, 1);
}

static int draw_prompt(window_t* w, int x, int y) {
    x = ui_text(w, x, y, "user@myos", C_GREEN, 1);
    x = ui_text(w, x, y, ":",         C_WHITE, 1);
    x = ui_text(w, x, y, "~",         C_BLUE,  1);
    x = ui_text(w, x, y, "$ ",        C_WHITE, 1);
    return x;
}

static void draw_terminal(window_t* w, int tx, int ty, int tw, int th, int W, int H) {
    int hdr = 22;
    char num[12];

    fill_rounded_rect(w, tx, ty, tw, th, 6, C_TERM_BG);
    fill_rect(w, tx, ty, tw, hdr, C_TITLE_BOT);              /* header bar */
    fill_rect(w, tx, ty + hdr - 2, tw, 2, C_TITLE_BOT);
    ui_text_centered(w, tx + tw / 2, ty + 7, "user@myos: ~", C_SIDE_TEXT, 1);
    fill_circle(w, tx + 12, ty + 11, 4, C_ORANGE);
    fill_circle(w, tx + 26, ty + 11, 4, 0xFFF5C211);
    fill_circle(w, tx + 40, ty + 11, 4, C_LEAF);

    int x0 = tx + 12, y = ty + hdr + 10, lh = 14, x;

    x = draw_prompt(w, x0, y);
    ui_text(w, x, y, "./welcome", C_WHITE, 1);
    y += lh;

    ui_text(w, x0, y, "Hello, world!", C_WHITE, 1);
    y += lh;

    x = ui_text(w, x0, y, "[  OK  ] ", C_GREEN, 1);
    ui_text(w, x, y, "graphics library loaded", C_WHITE, 1);
    y += lh;

    x = ui_text(w, x0, y, "[  OK  ] ", C_GREEN, 1);
    x = ui_text(w, x, y, "window registered: ", C_WHITE, 1);
    u32_to_str((uint32_t)W, num);  x = ui_text(w, x, y, num, C_CYAN, 1);
    x = ui_text(w, x, y, "x", C_WHITE, 1);
    u32_to_str((uint32_t)H, num);  ui_text(w, x, y, num, C_CYAN, 1);
    y += lh;

    x = draw_prompt(w, x0, y);
    fill_rect(w, x, y - 1, 8, 10, C_WHITE);                  /* block cursor */
}

static void draw_status_bar(window_t* w, int W, int y, int h) {
    fill_rect(w, 0, y, W, h, 0xFFEDEDED);
    draw_hline(w, 0, W - 1, y, C_CARD_BORDER);
    fill_circle(w, 14, y + h / 2 + 1, 4, C_LEAF);
    ui_text(w, 26, y + 8, "Ready", C_TEXT_DARK, 1);
    ui_text(w, W - 8 - ui_text_width("MyOS 1.0", 1), y + 8, "MyOS 1.0", C_TEXT_MUTED, 1);
}

/* ------------------------------------------------------------------ */
/* Whole window                                                        */
/* ------------------------------------------------------------------ */

void draw_welcome_window(window_t* w) {
    int W = (int)w->bounds.width;
    int H = (int)w->bounds.height;

    const int TITLE_H   = 38;
    const int STATUS_H  = 24;
    const int SIDEBAR_W = 190;

    int body_y = TITLE_H;
    int body_h = H - TITLE_H - STATUS_H;
    int main_x = SIDEBAR_W;
    int main_w = W - SIDEBAR_W;

    clear_window(w, C_MAIN_BG);

    draw_title_bar(w, W, TITLE_H);
    draw_sidebar(w, SIDEBAR_W, body_y, body_h);

    /* Welcome banner */
    int bx = main_x + 20, by = body_y + 20, bw = main_w - 40, bh = 130;
    draw_banner(w, bx, by, bw, bh);

    /* Feature cards */
    int gap = 16;
    int cw = (bw - 2 * gap) / 3, ch = 110;
    int cy = by + bh + 20;
    draw_card(w, bx,                 cy, cw, ch, C_ORANGE, "Shapes", "Lines, circles,",   "ellipses, polygons");
    draw_card(w, bx + (cw + gap),    cy, cw, ch, C_LEAF,   "Text",   "8x8 bitmap font,",  "any scale");
    draw_card(w, bx + 2 * (cw + gap), cy, cw, ch, C_CYAN,  "Colors", "Gradients & alpha", "blending");

    /* Terminal */
    int ty = cy + ch + 20;
    int th = (H - STATUS_H - 16) - ty;
    draw_terminal(w, bx, ty, bw, th, W, H);

    draw_status_bar(w, W, H - STATUS_H, STATUS_H);

    /* Window border */
    draw_rectangle(w, 0, 0, W, H, C_BORDER);
}

/* ------------------------------------------------------------------ */
/* Entry point                                                         */
/* ------------------------------------------------------------------ */

int main() {
    int32_t  x = 10;
    int32_t  y = 20;
    uint32_t width = 800;
    uint32_t height = 600;
    uint32_t bgcolor = 0xffffff;

    window_t* window = create_window(x, y, width, height, bgcolor);

    draw_welcome_window(window);

    register_window(window);
    
    while (1);

    return 0;
}

