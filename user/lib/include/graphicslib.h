#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color();
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    Color(uint8_t r, uint8_t g, uint8_t b);
    Color(uint32_t color);

    uint32_t toUint32();
} Color;

typedef struct Bitmap {
    uint32_t width;
    uint32_t height;
    uint32_t* buffer;

    Bitmap();
    Bitmap(uint32_t width, uint32_t height);
} Bitmap;

typedef struct Point {
    int x;
    int y;

    Point();
    Point(int x, int y);

    Point operator+(Point& p);
    Point operator-(Point& p);
    bool operator==(Point& p);
    bool operator!=(Point& p);
} Point;

typedef struct Size {
    uint32_t width;
    uint32_t height;

    Size();
    Size(uint32_t width, uint32_t height);

    Size operator+(Size& s);
    Size operator-(Size& s);
    bool operator==(Size& s);
    bool operator!=(Size& s);
} Size;

typedef struct Rect {
    int x;
    int y;
    uint32_t width;
    uint32_t height;

    Rect();
    Rect(int x, int y, int width, int height);
    Rect(Point p, Size s);
} Rect;


class Graphics {
public:
    Graphics(Bitmap* bitmap);
    void setPixel(Point p, Color color);
    Color getPixel(Point p);
    void setPixelAlpha(Point p, Color color); /* 0xAARRGGBB */
    void clear(Color color);
    void drawHline(Point p1, Point p2, Color color);
    void drawVline(Point p1, Point p2, Color color);
    void drawLine(Point p1, Point p2, Color color);
    void drawThickLine(Point p1, Point p2, int thickness, Color color);
    void drawRectangle(Rect r, Color color);
    void fillRect(Rect r, Color color);
    void fillRectAlpha(Rect r, Color color);
    void drawRectangleThick(Rect r, int thickness, Color color);
    void drawRoundedRect(Rect r, int radius, Color color);
    void fillRoundedRect(Rect r, int radius, Color color);
    void drawCircle(Point p, int radius, Color color);
    void fillCircle(Point p, int radius, Color color);
    void drawRing(Point p, int outerR, int innerR, Color color);
    void drawEllipse(Point p, int rx, int ry, Color color);
    void fillEllipse(Point p, int rx, int ry, Color color);
    void drawTriangle(Point p1, Point p2, Point p3, Color color);
    void fillTriangle(Point p1, Point p2, Point p3, Color color);
    void drawPolygon(Point* pts, int count, Color color);
    void fillPolygon(Point* pts, int count, Color color);
    void fillGradientV(Rect r, Color top, Color bottom);
    void fillGradientH(Rect r, Color left, Color right);
    void drawBitmap(Point p, Bitmap* src);
    void drawBitmapAlpha(Point p, Bitmap* src);
    void blitBitmap(Rect dstRect, Bitmap* src, Rect srcRect);
    int floodFill(Point p, Color newColor, int* stack, int stackCapacity);

private:
    Bitmap* bitmap;
};


/* Pixels */
void     set_pixel(Bitmap* bitmap, int x, int y, uint32_t color);
uint32_t get_pixel(Bitmap* bitmap, int x, int y);
void     set_pixel_alpha(Bitmap* bitmap, int x, int y, uint32_t color); /* 0xAARRGGBB */
void     clear_bitmap(Bitmap* bitmap, uint32_t color);

/* Lines */
void draw_hline(Bitmap* bitmap, int x0, int x1, int y, uint32_t color);
void draw_vline(Bitmap* bitmap, int x, int y0, int y1, uint32_t color);
void draw_line(Bitmap* bitmap, int x0, int y0, int x1, int y1, uint32_t color);
void draw_thick_line(Bitmap* bitmap, int x0, int y0, int x1, int y1, int thickness, uint32_t color);

/* Rectangles */
void draw_rectangle(Bitmap* bitmap, int x, int y, int width, int height, uint32_t color);
void fill_rect(Bitmap* bitmap, int x, int y, int width, int height, uint32_t color);
void fill_rect_alpha(Bitmap* bitmap, int x, int y, int width, int height, uint32_t color);
void draw_rectangle_thick(Bitmap* bitmap, int x, int y, int width, int height, int thickness, uint32_t color);
void draw_rounded_rect(Bitmap* bitmap, int x, int y, int width, int height, int radius, uint32_t color);
void fill_rounded_rect(Bitmap* bitmap, int x, int y, int width, int height, int radius, uint32_t color);

/* Circles & ellipses */
void draw_circle(Bitmap* bitmap, int xc, int yc, int radius, uint32_t color);
void fill_circle(Bitmap* bitmap, int xc, int yc, int radius, uint32_t color);
void draw_ring(Bitmap* bitmap, int xc, int yc, int outer_r, int inner_r, uint32_t color);
void draw_ellipse(Bitmap* bitmap, int xc, int yc, int rx, int ry, uint32_t color);
void fill_ellipse(Bitmap* bitmap, int xc, int yc, int rx, int ry, uint32_t color);

/* Triangles & polygons (pts = {x0,y0,x1,y1,...}) */
void draw_triangle(Bitmap* bitmap, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void fill_triangle(Bitmap* bitmap, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_polygon(Bitmap* bitmap, const int* pts, int count, uint32_t color);
void fill_polygon(Bitmap* bitmap, const int* pts, int count, uint32_t color);

/* Gradients */
void fill_gradient_v(Bitmap* bitmap, int x, int y, int width, int height, uint32_t top, uint32_t bottom);
void fill_gradient_h(Bitmap* bitmap, int x, int y, int width, int height, uint32_t left, uint32_t right);

/* Bitmaps */
void draw_bitmap(Bitmap* dst, int x, int y, Bitmap* src);
void draw_bitmap_alpha(Bitmap* dst, int x, int y, Bitmap* src);
void blit(Bitmap* dst, int dx, int dy, Bitmap* src, int sx, int sy, int w, int h);

/* Flood fill: stack needs 2 * stack_capacity ints; returns 0 ok, -1 on overflow */
int flood_fill(Bitmap* bitmap, int x, int y, uint32_t new_color, int* stack, int stack_capacity);

#endif