#include "guilib.h"
#include "math.h"
#include "winlib.h"

#define GUI_CONTAINER_INITIAL_CAPACITY 16


Widget::Widget(): Widget(WidgetProperties()) {}
Widget::Widget(WidgetProperties properties): 
        parent(nullptr), 
        location(properties.location), 
        paintLocation(),
        preferredSize(properties.preferredSize), 
        paintSize(), 
        padding(properties.padding), 
        border(properties.border), 
        constraints(properties.constraints), 
        backgroundColor(properties.backgroundColor),
        layoutProperties(properties.layoutProperties),
        eventHandlers(properties.eventHandlers),
        mouseOver(false),
        mouseDown(false),
        mousePressed(false),
        bitmap(),
        graphics(nullptr),
        paintState(WIDGET_PAINT_STATE_INIT)
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
LayoutProperties Widget::getLayoutProperties() {
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
    if (this->location != location){
        this->location = location;
        if (paintState != WIDGET_PAINT_STATE_INIT) {
            paintState = WIDGET_PAINT_STATE_LAYOUT_REQUIRED;
        }
    }
}
Point Widget::getLocation() {
    return location;
}
void Widget::setPreferredSize(Size preferedSize) {
    if (this->preferredSize != preferedSize) {
        this->preferredSize = preferedSize;
        if (paintState != WIDGET_PAINT_STATE_INIT) {
            paintState |= WIDGET_PAINT_STATE_LAYOUT_REQUIRED;
        }
    }
}
Size Widget::getPreferredSize() {
    return preferredSize;
}
void Widget::setBackgroundColor(Color backgroundColor) {
    if (this->backgroundColor != backgroundColor) {
        this->backgroundColor = backgroundColor;
        if (paintState != WIDGET_PAINT_STATE_INIT) {
            paintState |= WIDGET_PAINT_STATE_REPAINT_REQUIRED;
        }
    }
}
Color Widget::getBackgroundColor() {
    return backgroundColor;
}
EventHandlers Widget::getEventHandlers() {
    return eventHandlers;
}
void Widget::setEventHandlers(EventHandlers eventHandlers) {
    this->eventHandlers = eventHandlers;
}
bool Widget::isFocus() { return focus; }
void Widget::setFocus(bool focus) { this->focus = focus; }
bool Widget::containtPoint(Point p) {
    return p.x >= location.x && p.y >= location.y && 
            p.x < location.x + paintSize.width && p.y < location.y + paintSize.height;
}
Widget* Widget::getWidgetAt(Point &p) {
    if (containtPoint(p)) {
        return this;
    }
    return nullptr;
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
            graphics.drawRoundedRect(Rect(Point(0, 0), paintSize), border.radius, border.color);
        } else {
            graphics.drawRectangle(Rect(Point(0, 0), paintSize), border.color);
        }
    }
}
void Widget::paintBackground() {
    graphics.clear(Color());
    if (backgroundColor.a) {
        if (border.radius) {
            graphics.fillRoundedRect(Rect(Point(0, 0), paintSize), border.radius, backgroundColor);
        } else {
            graphics.fillRect(Rect(Point(0, 0), paintSize), backgroundColor);
        }
    }
}
void Widget::paint(BmDrawer* bmDrawer) {
    if (bitmap.width != paintSize.width || bitmap.height != paintSize.height) {
        if (bitmap.buffer) {
            delete[] bitmap.buffer;
        }
        bitmap = Bitmap(paintSize.width, paintSize.height);
    }
    graphics = Graphics(&bitmap);
    if (bitmap.buffer) {
        paintBackground();
        paintBorder();
        if (bmDrawer) {
            bmDrawer->drawBitmapAlpha(location, &bitmap);
        }
    }
    paintLocation = location;
    paintState = WIDGET_PAINT_STATE_PAINTED;
}
void Widget::repaint(BmDrawer* bmDrawer) {
    if (paintState & WIDGET_PAINT_STATE_REPAINT_REQUIRED) {
        paint(bmDrawer);
    }
}
void Widget::copyBitmap(BmDrawer* bmDrawer) {
    if (paintState & WIDGET_PAINT_STATE_REPAINT_REQUIRED) {
        repaint(bmDrawer);
        return;
    }
    if (bitmap.buffer && bmDrawer) {
        bmDrawer->drawBitmapAlpha(location, &bitmap);
    }
}
uint32_t Widget::getPaintState() {
    return paintState;
}
void Widget::updatePaintState() {
    if (paintLocation != location || paintSize.width != bitmap.width || paintSize.height != bitmap.height) {
        paintState |= WIDGET_PAINT_STATE_REPAINT_REQUIRED | WIDGET_PAINT_STATE_LAYOUT_REQUIRED;
    } else if (!(paintState & WIDGET_PAINT_STATE_REPAINT_REQUIRED)) {
        paintState = WIDGET_PAINT_STATE_PAINTED;
    }
}
Rect Widget::getRepaintBounds() {
    if (paintState & WIDGET_PAINT_STATE_REPAINT_REQUIRED) {
        Rect paintedBounds = Rect(paintLocation.x, paintLocation.y, bitmap.width, bitmap.height);
        return Rect(location, paintSize) + paintedBounds;
    } else {
        return Rect();
    }
}
void Widget::handleEvent(Event* event) {
    if (event->type == MOUSE_EVENT) {
        MouseInfo mouseInfo = event->mouse_info;
        if (!containtPoint(Point(mouseInfo.mouse_x, mouseInfo.mouse_y))) {
            mouseOver = false;
            if (eventHandlers.onMouseLeave){
                eventHandlers.onMouseLeave(this, mouseInfo);
            }
            return;
        }
        if (!mouseOver) {
            mouseOver = true;
            mouseDown = mouseInfo.left_button;
            mousePressed = false;
            if (eventHandlers.onMouseEnter) {
                eventHandlers.onMouseEnter(this, mouseInfo);
            }
            return;
        }
        if (mouseInfo.left_button != mouseDown){
            mouseDown = mouseInfo.left_button;
            if (mouseDown) {
                if (eventHandlers.onMouseDown) {
                    eventHandlers.onMouseDown(this, mouseInfo);
                }
                mousePressed = true;
            } else {
                if (eventHandlers.onMouseUp) {
                    eventHandlers.onMouseUp(this, mouseInfo);
                }
                if (mousePressed) {
                    if (eventHandlers.onClick) {
                        eventHandlers.onClick(this, mouseInfo);
                    }
                    mousePressed = false;
                }
            }
            return;
        }
        if (eventHandlers.onMouseMove) {
            eventHandlers.onMouseMove(this, mouseInfo);
        }
    } else if (event->type == KEYBOARD_EVENT) {
        KeyInfo keyInfo = event->key_info;
        if (keyInfo.key_release) {
            if (eventHandlers.onKeyUp) {
                eventHandlers.onKeyUp(this, keyInfo);
            }
        } else {
            if (eventHandlers.onKeyDown) {
                eventHandlers.onKeyDown(this, keyInfo);
            }
        }
    }
}
void Widget::handleFocusChange() {
    if (focus) {
        if (eventHandlers.onFocusIn) {
            eventHandlers.onFocusIn(this);
        }
    } else {
        if (eventHandlers.onFocusOut) {
            eventHandlers.onFocusOut(this);
        }
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
        uint32_t growableWidth = math_max(0, (int)bounds.width - (int)computedWidth);
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
        uint32_t growableHeight = math_max(0, (int)bounds.height - (int)computedHeight);
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
            } else if (alignment & ALIGNMENT_CENTER) {
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
            } else if (alignment & ALIGNMENT_CENTER) {
                x = bounds.x + (bounds.width - widget->getPaintWidth()) / 2;
            } else {
                x = bounds.x;
            }
            widget->setLocation(Point(x, y));
            y += widget->getPaintHeight() + spacing;
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
    uint32_t cellWidth = math_max(0,  ((int)bounds.width - ((int)columns - 1) * (int)spacing) / (int)columns);
    uint32_t cellHeight = math_max(0, ((int)bounds.height - ((int)rows - 1) * (int)spacing) / (int)rows);

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
                y += cellHeight * maxRowSpan + spacing * maxRowSpan;
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
    uint32_t columnWidth = math_max(0,  ((int)bounds.width - ((int)columns - 1) * (int)spacing) / (int)columns);

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
    if (paintState != WIDGET_PAINT_STATE_INIT){
        paintState |= WIDGET_PAINT_STATE_LAYOUT_REQUIRED;
    }
}
void Container::removeChild(Widget* widget) {
    children.remove(widget);
    widget->setParent(nullptr);
    if (paintState != WIDGET_PAINT_STATE_INIT){
        paintState |= WIDGET_PAINT_STATE_LAYOUT_REQUIRED;
    }
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
Widget* Container::getWidgetAt(Point &p) {
    if (Widget::containtPoint(p)) {
        Point localPoint = p - location;
        for (Widget* widget : children) {
            if (widget->containtPoint(localPoint)) {
                p.x = localPoint.x - widget->getLocation().x;
                p.y = localPoint.y - widget->getLocation().y;
                return widget;
            }
        }
        p = localPoint;
        return this;
    }
    return nullptr;
}
void Container::paint(BmDrawer* bmDrawer) {
    Widget::paint(bmDrawer);
    if (bitmap.buffer && bmDrawer) {
        performLayout();
        if (parent) {
            bmDrawer->setOrigin(location);
            bmDrawer->setClipping(getInnerBounds());
        }
        for (Widget* widget : children) {
            widget->paint(bmDrawer);
        }
        if (parent) {
            bmDrawer->clearClipping();
            bmDrawer->clearOrigin();
        }
    }
}
void Container::repaint(BmDrawer* bmDrawer) {
    if (paintState & WIDGET_PAINT_STATE_REPAINT_REQUIRED && bmDrawer) {
        Widget::paint(bmDrawer);
        if (parent) {
            bmDrawer->setOrigin(location);
            bmDrawer->setClipping(getInnerBounds());
        }
        if (bitmap.buffer) {
            for (Widget* widget : children) {
                widget->copyBitmap(bmDrawer);
            }
        }
        if (parent) {
            bmDrawer->clearClipping();
            bmDrawer->clearOrigin();
        }
        return;
    }
    if (parent) {
        bmDrawer->setOrigin(location);
        bmDrawer->setClipping(getInnerBounds());
    }
    if (bitmap.buffer) {
        for (Widget* widget : children) {
            widget->repaint(bmDrawer);
        }
    }
    if (parent) {
        bmDrawer->clearClipping();
        bmDrawer->clearOrigin();
    }
}
void Container::copyBitmap(BmDrawer* bmDrawer) {
    if (paintState & WIDGET_PAINT_STATE_REPAINT_REQUIRED) {
        repaint(bmDrawer);
        return;
    }
    Widget::copyBitmap(bmDrawer);
    if (bitmap.buffer) {
        bmDrawer->setOrigin(location);
        bmDrawer->setClipping(getInnerBounds());
        for (Widget* widget : children) {
            widget->copyBitmap(bmDrawer);
        }
        bmDrawer->clearClipping();
        bmDrawer->clearOrigin();
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
void Container::updateLayout() {
    performLayout();
    for (Widget* widget : children) {
        widget->updatePaintState();
    }
}
void Container::updatePaintState() {
    Widget::updatePaintState();
    if (!(paintState & WIDGET_PAINT_STATE_REPAINT_REQUIRED)) {
        for (Widget* widget : children) {
            if (widget->getPaintState() & WIDGET_PAINT_STATE_LAYOUT_REQUIRED) {
                paintState |= WIDGET_PAINT_STATE_REPAINT_REQUIRED;
                break;
            }
        }
    }
}
Rect Container::getRepaintBounds() {
    if (paintState & WIDGET_PAINT_STATE_REPAINT_REQUIRED) {
        return Widget::getRepaintBounds();
    } else {
        Rect bounds;
        for (int i = 0; i < children.count(); i++){
            if (children[i]->getPaintState() == WIDGET_PAINT_STATE_REPAINT_REQUIRED) {
                Rect childBounds = children[i]->getRepaintBounds();
                bounds = bounds + childBounds;
            }
        }
        return bounds + location;
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
Window::Window(WidgetProperties properties): Container(properties), handle(nullptr), fullscreen(false), bmDrawer(nullptr) {
    paintSize = properties.preferredSize;
}
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
        bitmap = Bitmap(paintSize.width, paintSize.height);
        bmDrawer = new BmDrawer(&bitmap);
        paint(bmDrawer);
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
            .buffer = bmDrawer->getBuffer()
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
void Window::handleWindowEvent(Event* event) {
    if (event->type == MOUSE_EVENT) {
        MouseInfo mouseInfo = event->mouse_info;
        Point mousePos{mouseInfo.mouse_x, mouseInfo.mouse_y};
        Widget* widget = getWidgetAt(mousePos);
        if (widget) {
            event->mouse_info.mouse_x = mousePos.x;
            event->mouse_info.mouse_y = mousePos.y;
            widget->handleEvent(event);
            handlePaintUpdate(widget);
            if (widget->isFocus()){
                if (focusedWidget != widget) {
                    if (focusedWidget) {
                        focusedWidget->setFocus(false);
                        focusedWidget->handleFocusChange();
                        handlePaintUpdate(focusedWidget);
                    }
                    focusedWidget = widget;
                    focusedWidget->setFocus(true);
                    focusedWidget->handleFocusChange();
                    handlePaintUpdate(focusedWidget);
                }
            }
        }
    } else if (event->type == KEYBOARD_EVENT) {
        if (focusedWidget) {
            focusedWidget->handleEvent(event);
            handlePaintUpdate(focusedWidget);
        }
    }
}
void Window::handlePaintUpdate(Widget* widget) {
    Widget* currentWidget = nullptr;
    Rect repaintBounds;
    if (widget->getPaintState() & WIDGET_PAINT_STATE_LAYOUT_REQUIRED) {
        Widget* parent = widget->getParent();
        if (parent) {
           ((Container*)parent)->updateLayout();
           repaintBounds = parent->getRepaintBounds();
           parent->updatePaintState();
        }
    }
    if (currentWidget == nullptr) {
        currentWidget = widget;
        repaintBounds = widget->getRepaintBounds();
    }
    if (repaintBounds.width > 0 && repaintBounds.height > 0) {
        repaint(bmDrawer);
        updateRegion(repaintBounds);
    }
}
void Window::listenToEvents() {
    eventQueue = new EventQueue(10);
    eventQueue->register_event_queue();

    while (true) {
        Event event = eventQueue->deque();
        if (event.type != NO_EVENT) {
            handleWindowEvent(&event);
        }
    }
}
void Window::updateRegion(Rect &bounds) {
    if (handle) {
        bounds_t abs_bounds;
        abs_bounds.locationx = bounds.x;
        abs_bounds.locationy = bounds.y;
        abs_bounds.width = bounds.width;
        abs_bounds.height = bounds.height;
        redraw_window_region(handle, &abs_bounds);
    }
}
void Window::destroy() {
    if (eventQueue) {
        eventQueue->deregister_event_queue();
        delete eventQueue;
        eventQueue = nullptr;
    }
    if (handle) {
        unregisterWindow();
    }
    if (bmDrawer) {
        delete bmDrawer;
        bmDrawer = nullptr;
    }
    Container::destroy();
}
