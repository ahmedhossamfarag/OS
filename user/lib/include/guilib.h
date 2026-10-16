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

typedef struct WidgetProperties {
    Point location = {};
    Size preferredSize = {};
    Padding padding = {};
    Border border = {};
    Constraints constraints = {};
    Color backgroundColor = {};
} WidgetProperties;

class Widget {
    public:
        Widget();
        Widget(WidgetProperties properties);
        
        virtual uint32_t getWidth();
        virtual uint32_t getHeight();
        void setPiantWidth(uint32_t width);
        void setPaintHeight(uint32_t height);
        void destroy();

    protected:
        Widget* parent;
        Point location;
        Size preferredSize;
        Size paintSize;
        Padding padding;
        Border border;
        Constraints constraints;
        Color backgroundColor;
        Bitmap bitmap;
        Graphics graphics;
};

class Layout {
    public:
        virtual uint32_t getComputedWidth(Array<Widget*> &widgets, uint32_t height) = 0;
        virtual uint32_t getComputedHeight(Array<Widget*> &widgets, uint32_t width) = 0;
        virtual void performLayout(Array<Widget*> &widgets, Rect &bounds) = 0;
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
        FlexLayout();
        FlexLayout(FlexDirection direction, uint32_t alignment, uint32_t spacing);
        uint32_t getComputedWidth(Array<Widget*> &widgets, uint32_t height) override;
        uint32_t getComputedHeight(Array<Widget*> &widgets, uint32_t width) override;
        void performLayout(Array<Widget*> &widgets, Rect &bounds) override;

        FlexDirection direction;
        uint32_t alignment;
        uint32_t spacing;
};

class GridLayout : public Layout {
    public:
        GridLayout();
        GridLayout(uint32_t columns, uint32_t rows);
        uint32_t getComputedWidth(Array<Widget*> &widgets, uint32_t height) override;
        uint32_t getComputedHeight(Array<Widget*> &widgets, uint32_t width) override;
        void performLayout(Array<Widget*> &widgets, Rect &bounds) override;

        uint32_t columns;
        uint32_t rows;
};

class FlowLayout : public Layout {
    public:
        FlowLayout();
        FlowLayout(uint32_t columns);
        uint32_t getComputedWidth(Array<Widget*> &widgets, uint32_t height) override;
        uint32_t getComputedHeight(Array<Widget*> &widgets, uint32_t width) override;
        void performLayout(Array<Widget*> &widgets, Rect &bounds) override;

        uint32_t columns;
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

    protected:
        Array<Widget*> children;
        Layout* layout;
};