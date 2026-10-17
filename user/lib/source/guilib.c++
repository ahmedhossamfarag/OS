#include "guilib.h"
#include "math.h"
#include "winlib.h"

#define GUI_CONTAINER_INITIAL_CAPACITY 16


Widget::Widget(): Widget(WidgetProperties()) {}
Widget::Widget(WidgetProperties properties): 
        parent(nullptr), 
        location(properties.location), 
        preferredSize(properties.preferredSize), 
        paintSize(), 
        padding(properties.padding), 
        border(properties.border), 
        constraints(properties.constraints), 
        backgroundColor(properties.backgroundColor),
        layoutProperties(properties.layoutProperties),
        bitmap(),
        graphics(nullptr)
        {}
Rect Widget::getBounds() {
    return Rect(location, paintSize);
}
Rect Widget::getInnerBounds() {
    if (paintSize.width < (padding.horizontal * 2) || paintSize.height < (padding.vertical * 2)) {
        return Rect();
    }
    return Rect(
        location.x + padding.horizontal,
        location.y + padding.vertical,
        paintSize.width - padding.horizontal * 2,
        paintSize.height - padding.vertical * 2
    );
}
Rect Widget::getRelativeInnerBounds() {
    if (paintSize.width < (padding.horizontal * 2) || paintSize.height < (padding.vertical * 2)) {
        return Rect();
    }
    return Rect(
        padding.horizontal,
        padding.vertical,
        paintSize.width - padding.horizontal * 2,
        paintSize.height - padding.vertical * 2
    );
}
LayoutProperties &Widget::getLayoutProperties() {
    return layoutProperties;
}
uint32_t Widget::getWidth() {
    return math_min(math_max(preferredSize.width, constraints.minWidth), constraints.maxWidth);
}
uint32_t Widget::getHeight() {
    return math_min(math_max(preferredSize.height, constraints.minHeight), constraints.maxHeight);
}
void Widget::setPaintWidth(uint32_t width) {
    if (width < constraints.minWidth) {
        paintSize.width = 0;
    } else if (width > constraints.maxWidth) {
        paintSize.width = constraints.maxWidth;
    } else {
        paintSize.width = width;
    }
}
void Widget::setPaintHeight(uint32_t height) {
    if (height < constraints.minHeight) {
        paintSize.height = 0;
    } else if (height > constraints.maxHeight) {
        paintSize.height = constraints.maxHeight;
    } else {
        paintSize.height = height;
    }
}
uint32_t Widget::getPaintWidth() {
    return paintSize.width;
}
uint32_t Widget::getPaintHeight() {
    return paintSize.height;
}
void Widget::setLocation(Point location) {
    this->location = location;
}
Point Widget::getLocation() {
    return location;
}
Widget* Widget::getParent() {
    return parent;
}
void Widget::setParent(Widget* parent) {
    this->parent = parent;
}
void Widget::paintBorder() {
    if (border.thickness) {
        if (border.radius) {
            graphics.drawRoundedRect(Rect(location, paintSize), border.radius, border.color);
        } else {
            graphics.drawRectangle(Rect(location, paintSize), border.color);
        }
    }
}
void Widget::paintBackground() {
    if (backgroundColor.a) {
        if (border.radius) {
            graphics.fillRoundedRect(Rect(location, paintSize), border.radius, backgroundColor);
        } else {
            graphics.fillRect(Rect(location, paintSize), backgroundColor);
        }
    }
}
void Widget::paint() {
    bitmap = Bitmap(paintSize.width, paintSize.height);
    graphics = Graphics(&bitmap);
    paintBackground();
    paintBorder();
}
void Widget::copyBitmap(Graphics &g) {
    if (bitmap.buffer) {
        g.drawBitmapAlpha(location, &bitmap);
    }
}
void Widget::destroy() {
    if (bitmap.buffer) {
        delete[] bitmap.buffer;
        bitmap = {};
        graphics = Graphics(nullptr);
    }
}

uint32_t DefaultLayout::getComputedWidth(Array<Widget*> &widgets, uint32_t height) {
    return 0;
}
uint32_t DefaultLayout::getComputedHeight(Array<Widget*> &widgets, uint32_t width) {
    return 0;
}
void DefaultLayout::performLayout(Array<Widget*> &widgets, Rect &bounds) {
    for (Widget* widget : widgets) {
        widget->setPaintWidth(widget->getWidth());
        widget->setPaintHeight(widget->getHeight());
    }
}

uint32_t FlexLayout::getComputedWidth(Array<Widget*> &widgets, uint32_t height) {
    if (direction == FLEX_DIRECTION_ROW || direction == FLEX_DIRECTION_ROW_REVERSE) {
        uint32_t width = 0;
        for (Widget* widget : widgets) {
            width += widget->getWidth();
        }
        if (width > 0) {
            width += spacing * (widgets.count() - 1);
        }
        return width;
    } else {
        uint32_t width = 0;
        for (Widget* widget : widgets) {
            width += math_max(widget->getWidth(), width);
        }
        return width;
    }
}
uint32_t FlexLayout::getComputedHeight(Array<Widget*> &widgets, uint32_t width) {
    if (direction == FLEX_DIRECTION_COLUMN || direction == FLEX_DIRECTION_COLUMN_REVERSE) {
        uint32_t height = 0;
        for (Widget* widget : widgets) {
            height += widget->getHeight();
        }
        if (height > 0) {
            height += spacing * (widgets.count() - 1);
        }
        return height;
    } else {
        uint32_t height = 0;
        for (Widget* widget : widgets) {
            height += math_max(widget->getHeight(), height);
        }
        return height;
    }
}
void FlexLayout::performLayout(Array<Widget*> &widgets, Rect &bounds) {
    if (bounds.width == 0 || bounds.height == 0) {
        for (Widget* widget : widgets) {
            widget->setPaintWidth(0);
            widget->setPaintHeight(0);
        }
        return;
    }

    uint32_t availableWidth = bounds.width;
    uint32_t availableHeight = bounds.height;

    if (direction == FLEX_DIRECTION_ROW || direction == FLEX_DIRECTION_ROW_REVERSE) {
        uint32_t computedWidth = getComputedWidth(widgets, bounds.height);
        uint32_t growableWidth = math_max(0, bounds.width - computedWidth);
        for (int i = 0; i < widgets.count(); i++) {
            Widget* widget = widgets[i];
            if (availableWidth == 0) {
                widget->setPaintWidth(0);
                widget->setPaintHeight(0);
                continue;
            }
            if (widget->getLayoutProperties().flexGrow) {
                widget->setPaintWidth(math_min(availableWidth, widget->getWidth() + growableWidth));
            } else {
                widget->setPaintWidth(math_min(availableWidth, widget->getWidth()));
            }
            if (alignment & ALIGNMENT_STRETCH) {
                widget->setPaintHeight(bounds.height);
            } else {
                widget->setPaintHeight(math_min(bounds.height, widget->getHeight()));
            }
            if (availableWidth < widget->getPaintWidth()) {
                availableWidth = 0;
            } else {
                availableWidth -= widget->getPaintWidth();
            }
            if (i < widgets.count() - 1) {
                if (availableWidth < spacing) {
                    availableWidth = 0;
                } else {
                    availableWidth -= spacing;
                }
            }
        }
    } else if (direction == FLEX_DIRECTION_COLUMN || direction == FLEX_DIRECTION_COLUMN_REVERSE) {
        uint32_t computedHeight = getComputedHeight(widgets, bounds.width);
        uint32_t growableHeight = math_max(0, bounds.height - computedHeight);
        for (int i = 0; i < widgets.count(); i++) {
            Widget* widget = widgets[i];
            if (availableHeight == 0) {
                widget->setPaintWidth(0);
                widget->setPaintHeight(0);
                continue;
            }
            if (widget->getLayoutProperties().flexGrow) {
                widget->setPaintHeight(math_min(availableHeight, widget->getHeight() + growableHeight));
                growableHeight -= widget->getPaintHeight() - widget->getHeight();
            } else {
                widget->setPaintHeight(math_min(availableHeight, widget->getHeight()));
            }
            if (alignment & ALIGNMENT_STRETCH) {
                widget->setPaintWidth(bounds.width);
            } else {
                widget->setPaintWidth(math_min(bounds.width, widget->getWidth()));
            }
            if (availableHeight < widget->getPaintHeight()) {
                availableHeight = 0;
            } else {
                availableHeight -= widget->getPaintHeight();
            }
            if (i < widgets.count() - 1) {
                if (availableHeight < spacing) {
                    availableHeight = 0;
                } else {
                    availableHeight -= spacing;
                }
            }
    }

    if (direction == FLEX_DIRECTION_ROW_REVERSE) {
        int x = bounds.x + bounds.width;
        if (alignment & ALIGNMENT_LEFT) {
            x -= availableWidth;
        } else if (alignment & ALIGNMENT_CENTER) {
            x -= availableWidth / 2;
        }
        for (Widget* widget : widgets) {
            int y;
            if (alignment & ALIGNMENT_BOTTOM) {
                y = bounds.y + bounds.height - widget->getPaintHeight();
            } else if (alignment & ALIGNMENT_MIDDLE) {
                y = bounds.y + (bounds.height - widget->getPaintHeight()) / 2;
            } else {
                y = bounds.y;
            }
            widget->setLocation(Point(x - widget->getPaintWidth(), y));
            x -= widget->getPaintWidth() + spacing;
        }
    } else if (direction == FLEX_DIRECTION_ROW) {
        int x = bounds.x;
        if (alignment & ALIGNMENT_RIGHT) {
            x += availableWidth;
        } else if (alignment & ALIGNMENT_CENTER) {
            x += availableWidth / 2;
        }
        for (Widget* widget : widgets) {
            int y;
            if (alignment & ALIGNMENT_BOTTOM) {
                y = bounds.y + bounds.height - widget->getPaintHeight();
            } else if (alignment & ALIGNMENT_MIDDLE) {
                y = bounds.y + (bounds.height - widget->getPaintHeight()) / 2;
            } else {
                y = bounds.y;
            }
            widget->setLocation(Point(x, y));
            x += widget->getPaintWidth() + spacing;
        }
    } else if (direction == FLEX_DIRECTION_COLUMN_REVERSE) {
        int y = bounds.y + bounds.height;
        if (alignment & ALIGNMENT_TOP) {
            y -= availableHeight;
        } else if (alignment & ALIGNMENT_MIDDLE) {
            y -= availableHeight / 2;
        }
        for (Widget* widget : widgets) {
            int x;
            if (alignment & ALIGNMENT_RIGHT) {
                x = bounds.x + bounds.width - widget->getPaintWidth();
            } else if (alignment & ALIGNMENT_MIDDLE) {
                x = bounds.x + (bounds.width - widget->getPaintWidth()) / 2;
            } else {
                x = bounds.x;
            }
            widget->setLocation(Point(x, y - widget->getPaintHeight()));
            y -= widget->getPaintHeight() + spacing;
        }
    } else if (direction == FLEX_DIRECTION_COLUMN) {
        int y = bounds.y;
        if (alignment & ALIGNMENT_BOTTOM) {
            y += availableHeight;
        } else if (alignment & ALIGNMENT_MIDDLE) {
            y += availableHeight / 2;
        }
        for (Widget* widget : widgets) {
            int x;
            if (alignment & ALIGNMENT_RIGHT) {
                x = bounds.x + bounds.width - widget->getPaintWidth();
            } else if (alignment & ALIGNMENT_MIDDLE) {
                x = bounds.x + (bounds.width - widget->getPaintWidth()) / 2;
            } else {
                x = bounds.x;
            }
            widget->setLocation(Point(x, y));
            y += widget->getPaintHeight() + spacing;
        }
    }
}

}

uint32_t GridLayout::getComputedWidth(Array<Widget*> &widgets, uint32_t height) {
    return 0;
}
uint32_t GridLayout::getComputedHeight(Array<Widget*> &widgets, uint32_t width) {
    return 0;
}
void GridLayout::performLayout(Array<Widget*> &widgets, Rect &bounds) {
    uint32_t cellWidth = math_max(0,  (bounds.width - (columns - 1) * spacing) / columns);
    uint32_t cellHeight = math_max(0, (bounds.height - (rows - 1) * spacing) / rows);

    if (cellWidth == 0 || cellHeight == 0 || columns == 0 || rows == 0) {
        for (Widget* widget : widgets) {
            widget->setPaintWidth(0);
            widget->setPaintHeight(0);
        }
        return;
    }

    int x = bounds.x;
    int y = bounds.y;
    uint32_t row = 0;
    uint32_t col = 0;
    uint32_t maxRowSpan = 0;
    for (Widget* widget : widgets) {
        uint32_t colSpan = widget->getLayoutProperties().gridColumnSpan;
        uint32_t rowSpan = widget->getLayoutProperties().gridRowSpan;
        rowSpan = math_min(rowSpan, rows - row);
        colSpan = math_min(colSpan, columns - col);
        if (rowSpan < 1 || colSpan < 1) {
            widget->setPaintWidth(0);
            widget->setPaintHeight(0);
            continue;
        }
        widget->setPaintWidth(cellWidth * colSpan + spacing * (colSpan - 1));
        widget->setPaintHeight(cellHeight * rowSpan + spacing * (rowSpan - 1));
        int endX = x + cellWidth * colSpan + spacing * (colSpan - 1);
        int endY = y + cellHeight * rowSpan + spacing * (rowSpan - 1);
        int widgetX, widgetY;
        if (alignment & ALIGNMENT_RIGHT) {
            widgetX = endX - widget->getPaintWidth();
        } else if (alignment & ALIGNMENT_CENTER) {
            widgetX = x + (endX - x - widget->getPaintWidth()) / 2;
        } else {
            widgetX = x;
        }
        if (alignment & ALIGNMENT_BOTTOM) {
            widgetY = endY - widget->getPaintHeight();
        } else if (alignment & ALIGNMENT_MIDDLE) {
            widgetY = y + (endY - y - widget->getPaintHeight()) / 2;
        } else {
            widgetY = y;
        }
        widget->setLocation(Point(widgetX, widgetY));
        x = endX + spacing;
        col += colSpan;
        maxRowSpan = math_max(maxRowSpan, rowSpan);
        if (col >= columns) {
            col = 0;
            x = bounds.x;
            if (maxRowSpan > 0) {
                row += maxRowSpan;
                y += cellHeight * maxRowSpan + spacing * (maxRowSpan - 1);
                maxRowSpan = 0;
            }
        }
    }
}

uint32_t FlowLayout::getComputedWidth(Array<Widget*> &widgets, uint32_t height) {
    return 0;
}
uint32_t FlowLayout::getComputedHeight(Array<Widget*> &widgets, uint32_t width) {
    uint32_t height = 0;
    uint32_t row_height = 0;
    for (int i = 0; i < widgets.count(); i++) {
        if (i % this->columns == 0) {
            height += row_height;
            row_height = 0;
        }
        row_height = math_max(row_height, widgets[i]->getHeight());
    }
    return height;
}
void FlowLayout::performLayout(Array<Widget*> &widgets, Rect &bounds) {
    uint32_t columnWidth = math_max(0,  (bounds.width - (columns - 1) * spacing) / columns);

    if (columnWidth == 0 || columns == 0 || bounds.height == 0) {
        for (Widget* widget : widgets) {
            widget->setPaintWidth(0);
            widget->setPaintHeight(0);
        }
        return;
    }

    int x = bounds.x;
    int y = bounds.y;
    uint32_t col = 0;
    uint32_t rowHeight = 0;
    uint32_t availableHeight = bounds.height;
    for (int i = 0; i < widgets.count(); i++) {
        if (availableHeight == 0) {
            widgets[i]->setPaintWidth(0);
            widgets[i]->setPaintHeight(0);
            continue;
        }
        if (col == 0) {
            for (int j = i; j < i + columns; j++) {
                if (j < widgets.count()) {
                    rowHeight = math_max(rowHeight, widgets[j]->getHeight());
                }
            }
            rowHeight = math_min(rowHeight, availableHeight);
        }
        Widget* widget = widgets[i];
        widget->setPaintWidth(columnWidth);
        if (alignment & ALIGNMENT_STRETCH) {
            widget->setPaintHeight(rowHeight);
        } else {
            widget->setPaintHeight(math_min(rowHeight, widget->getHeight()));
        }
        int endX = x + columnWidth;
        int endY = y + rowHeight;
        int widgetX, widgetY;
        if (alignment & ALIGNMENT_RIGHT) {
            widgetX = endX - widget->getPaintWidth();
        } else if (alignment & ALIGNMENT_CENTER) {
            widgetX = x + (endX - x - widget->getPaintWidth()) / 2;
        } else {
            widgetX = x;
        }
        if (alignment & ALIGNMENT_BOTTOM) {
            widgetY = endY - widget->getPaintHeight();
        } else if (alignment & ALIGNMENT_MIDDLE) {
            widgetY = y + (endY - y - widget->getPaintHeight()) / 2;
        } else {
            widgetY = y;
        }
        widget->setLocation(Point(widgetX, widgetY));
        x += columnWidth + spacing;
        col++;
        if (col >= columns) {
            col = 0;
            x = bounds.x;
            y += rowHeight + spacing;
            if (availableHeight < rowHeight + spacing) {
                availableHeight = 0;
            } else {
                availableHeight -= rowHeight + spacing;
            }
        }
    }
}

Container::Container(): Container(WidgetProperties()) {}
Container::Container(WidgetProperties properties): Widget(properties), children(GUI_CONTAINER_INITIAL_CAPACITY), layout(nullptr) {}
void Container::setLayout(Layout* layout) {
    this->layout = layout;
}
void Container::addChild(Widget* widget) {
    if (widget->getParent()) {
        ((Container*)widget->getParent())->removeChild(widget);
    }
    children.add(widget);
    widget->setParent(this);
}
void Container::removeChild(Widget* widget) {
    children.remove(widget);
    widget->setParent(nullptr);
}
uint32_t Container::getWidth() {
    if (preferredSize.width) {
        return Widget::getWidth();
    } else {
        uint32_t computedWidth = layout->getComputedWidth(children, preferredSize.height);
        preferredSize.width = computedWidth + (padding.horizontal * 2);
        return Widget::getWidth();
    }
}
uint32_t Container::getHeight() {
    if (preferredSize.height) {
        return Widget::getHeight();
    } else {
        uint32_t computedHeight = layout->getComputedHeight(children, preferredSize.width);
        preferredSize.height = computedHeight + (padding.vertical * 2);
        return Widget::getHeight();
    }
}
void Container::paint() {
    Widget::paint();
    for (Widget* widget : children) {
        widget->copyBitmap(graphics);
    }
}
void Container::performLayout() {
    Rect bounds = getRelativeInnerBounds();
    if (layout) {
        layout->performLayout(children, bounds);
    } else {
        DefaultLayout layout;
        layout.performLayout(children, bounds);
    }
}
void Container::destroy() {
    for (Widget* widget : children) {
        widget->destroy();
        delete widget;
    }
    Widget::destroy();
}

Window::Window(): Window(WidgetProperties()) {}
Window::Window(WidgetProperties properties): Container(properties), handle(nullptr), fullscreen(false) {}
void Window::setFullscreen(bool fullscreen) {
    this->fullscreen = fullscreen;
    if (fullscreen) {
        location = Point(0, 0);
        uint32_t width, height;
        get_screen_size(&width, &height);
        paintSize.width = width;
        paintSize.height = height;
    } else {
        paintSize = preferredSize;
    }
}
void Window::registerWindow() {
    if (handle) {
        this->unregisterWindow();
    }
    if (!bitmap.buffer) {
        paint();
    }
    if (bitmap.buffer) {
        window_t window{
            .bounds = {
                .locationx = location.x,
                .locationy = location.y,
                .width = (int32_t)paintSize.width,
                .height = (int32_t)paintSize.height
            },
            .owner = 0,
            .buffer = bitmap.buffer
        };
        handle = register_window(&window);
    }
}
void Window::unregisterWindow() {
    if (handle) {
        deregister_window(handle);
        handle = nullptr;
    }
}
