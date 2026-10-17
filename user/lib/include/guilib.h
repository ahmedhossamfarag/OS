#include "graphicslib.h"
#include "dslib.h"

typedef struct Border {
    uint32_t thickness = 0;
    Color color = {};
    uint32_t radius = 0;
} Border;

typedef struct Padding {
    uint32_t horizontal = 0;
    uint32_t vertical = 0;
} Padding;

typedef struct Constraints {
    uint32_t minWidth = 0;
    uint32_t minHeight = 0;
    uint32_t maxWidth = 0xFFFFFFFF;
    uint32_t maxHeight = 0xFFFFFFFF;
} Constraints;

typedef struct LayoutProperties {
    bool flexGrow = false;
    uint32_t gridRowSpan = 1;
    uint32_t gridColumnSpan = 1;
} LayoutProperties;

typedef struct WidgetProperties {
    Point location = {};
    Size preferredSize = {};
    Padding padding = {};
    Border border = {};
    Constraints constraints = {};
    Color backgroundColor = {};
    LayoutProperties layoutProperties = {};
} WidgetProperties;

class Widget {
    public:
        Widget();
        Widget(WidgetProperties properties);

        Rect getBounds();
        Rect getInnerBounds();
        Rect getRelativeInnerBounds();
        LayoutProperties &getLayoutProperties();
        virtual uint32_t getWidth();
        virtual uint32_t getHeight();
        void setPaintWidth(uint32_t width);
        void setPaintHeight(uint32_t height);
        uint32_t getPaintWidth();
        uint32_t getPaintHeight();
        void setLocation(Point location);
        Point getLocation();
        Widget* getParent();
        void setParent(Widget* parent);
        virtual void paint();
        virtual void copyBitmap(Graphics &g);
        virtual void destroy();

    protected:
        Widget* parent;
        Point location;
        Size preferredSize;
        Size paintSize;
        Padding padding;
        Border border;
        Constraints constraints;
        Color backgroundColor;
        LayoutProperties layoutProperties;
        Bitmap bitmap;
        Graphics graphics;

        void paintBorder();
        void paintBackground();
};

class Layout {
    public:
        virtual uint32_t getComputedWidth(Array<Widget*> &widgets, uint32_t height) = 0;
        virtual uint32_t getComputedHeight(Array<Widget*> &widgets, uint32_t width) = 0;
        virtual void performLayout(Array<Widget*> &widgets, Rect &bounds) = 0;
};

class DefaultLayout : public Layout {
    public:
        uint32_t getComputedWidth(Array<Widget*> &widgets, uint32_t height) override;
        uint32_t getComputedHeight(Array<Widget*> &widgets, uint32_t width) override;
        void performLayout(Array<Widget*> &widgets, Rect &bounds) override;
};

typedef enum FlexDirection {
    FLEX_DIRECTION_ROW,
    FLEX_DIRECTION_COLUMN,
    FLEX_DIRECTION_ROW_REVERSE,
    FLEX_DIRECTION_COLUMN_REVERSE
} FlexDirection;

typedef enum Alignment {
    ALIGNMENT_LEFT = 0b1,
    ALIGNMENT_CENTER = 0b10,
    ALIGNMENT_RIGHT = 0b100,
    ALIGNMENT_TOP = 0b1000,
    ALIGNMENT_MIDDLE = 0b10000,
    ALIGNMENT_BOTTOM = 0b100000,
    ALIGNMENT_STRETCH = 0b1000000
} Alignment;

class FlexLayout : public Layout {
    public:
        uint32_t getComputedWidth(Array<Widget*> &widgets, uint32_t height) override;
        uint32_t getComputedHeight(Array<Widget*> &widgets, uint32_t width) override;
        void performLayout(Array<Widget*> &widgets, Rect &bounds) override;

        FlexDirection direction = FLEX_DIRECTION_ROW;
        uint32_t spacing = 0;
        uint32_t alignment = ALIGNMENT_LEFT | ALIGNMENT_TOP;
};

class GridLayout : public Layout {
    public:
        uint32_t getComputedWidth(Array<Widget*> &widgets, uint32_t height) override;
        uint32_t getComputedHeight(Array<Widget*> &widgets, uint32_t width) override;
        void performLayout(Array<Widget*> &widgets, Rect &bounds) override;

        uint32_t columns = 1;
        uint32_t rows = 1;
        uint32_t spacing = 0;
        uint32_t alignment = ALIGNMENT_LEFT | ALIGNMENT_TOP;
};

class FlowLayout : public Layout {
    public:
        uint32_t getComputedWidth(Array<Widget*> &widgets, uint32_t height) override;
        uint32_t getComputedHeight(Array<Widget*> &widgets, uint32_t width) override;
        void performLayout(Array<Widget*> &widgets, Rect &bounds) override;

        uint32_t columns;
        uint32_t spacing = 0;
        uint32_t alignment = ALIGNMENT_LEFT | ALIGNMENT_TOP;
};

class Container : public Widget {
    public:
        Container();
        Container(WidgetProperties properties);

        void setLayout(Layout* layout);
        void addChild(Widget* widget);
        void removeChild(Widget* widget);
        uint32_t getWidth() override;
        uint32_t getHeight() override;
        void paint() override;
        void performLayout();
        void destroy() override;

    protected:
        Array<Widget*> children;
        Layout* layout;
};

class Window : public Container {
    public:
        Window();
        Window(WidgetProperties properties);

        void setFullscreen(bool fullscreen);
        void registerWindow();
        void unregisterWindow();

    protected:
        void* handle;
        bool fullscreen;
};