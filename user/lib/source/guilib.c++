#include "guilib.h"
#include "math.h"

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
        bitmap(),
        graphics(nullptr)
        {}
uint32_t Widget::getWidth() {
    return math_min(math_max(preferredSize.width, constraints.minWidth), constraints.maxWidth);
}
uint32_t Widget::getHeight() {
    return math_min(math_max(preferredSize.height, constraints.minHeight), constraints.maxHeight);
}
void Widget::setPiantWidth(uint32_t width) {
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

FlexLayout::FlexLayout(): direction(FLEX_DIRECTION_ROW), alignment(ALIGNMENT_LEFT | ALIGNMENT_TOP), spacing(0) {}
FlexLayout::FlexLayout(FlexDirection direction, uint32_t alignment, uint32_t spacing): direction(direction), alignment(alignment), spacing(spacing) {}
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
    if (direction == FLEX_DIRECTION_ROW) {
        // TODO
    } else if (direction == FLEX_DIRECTION_ROW_REVERSE) {
        // TODO
    } else if (direction == FLEX_DIRECTION_COLUMN) {
        // TODO
    } else if (direction == FLEX_DIRECTION_COLUMN_REVERSE) {
        // TODO
    }
}

GridLayout::GridLayout(): columns(0), rows(0) {}
GridLayout::GridLayout(uint32_t columns, uint32_t rows): columns(columns), rows(rows) {}
uint32_t GridLayout::getComputedWidth(Array<Widget*> &widgets, uint32_t height) {
    return 0;
}
uint32_t GridLayout::getComputedHeight(Array<Widget*> &widgets, uint32_t width) {
    return 0;
}
void GridLayout::performLayout(Array<Widget*> &widgets, Rect &bounds) {
   // TODO
}

FlowLayout::FlowLayout(): columns(0) {}
FlowLayout::FlowLayout(uint32_t columns): columns(columns) {}
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
    // TODO
}

Container::Container(): Container(WidgetProperties()) {}
Container::Container(WidgetProperties properties): Widget(properties), children(GUI_CONTAINER_INITIAL_CAPACITY), layout(nullptr) {}
void Container::setLayout(Layout* layout) {
    this->layout = layout;
}
void Container::addChild(Widget* widget) {
    children.add(widget);
}
void Container::removeChild(Widget* widget) {
    children.remove(widget);
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
