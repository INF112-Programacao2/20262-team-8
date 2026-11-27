#ifndef GUI_WIDGETS_HPP
#define GUI_WIDGETS_HPP

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace ui {

struct Box {
    float x;
    float y;
    float width;
    float height;

    bool contains(float px, float py) const {
        return px >= x && px < x + width && py >= y && py < y + height;
    }

    bool contains(sf::Vector2i point) const {
        return contains(static_cast<float>(point.x), static_cast<float>(point.y));
    }
};

namespace colors {
inline const sf::Color background(22, 27, 38);
inline const sf::Color sidebar(16, 20, 29);
inline const sf::Color panel(33, 40, 56);
inline const sf::Color field(18, 23, 33);
inline const sf::Color border(62, 74, 98);
inline const sf::Color text(232, 236, 244);
inline const sf::Color muted(140, 152, 175);
inline const sf::Color accent(46, 160, 110);
inline const sf::Color selection(46, 110, 170);
inline const sf::Color danger(205, 80, 80);
inline const sf::Color warning(196, 130, 30);
inline const sf::Color successText(110, 220, 150);
inline const sf::Color errorText(255, 130, 130);
}

inline sf::String fromUtf8(const std::string& value) {
    return sf::String::fromUtf8(value.begin(), value.end());
}

inline std::string toUtf8(const sf::String& value) {
    const auto encoded = value.toUtf8();
    return std::string(encoded.begin(), encoded.end());
}

inline void drawText(sf::RenderTarget& target, const sf::Font& font,
                     const std::string& value, unsigned size, float x, float y,
                     sf::Color color = colors::text, bool bold = false) {
    sf::Text text(font, fromUtf8(value), size);
    text.setFillColor(color);
    if (bold) {
        text.setStyle(sf::Text::Bold);
    }
    text.setPosition({x, y});
    target.draw(text);
}

inline void drawBox(sf::RenderTarget& target, Box box, sf::Color fill,
                    sf::Color outline = sf::Color::Transparent, float thickness = 0) {
    sf::RectangleShape shape({box.width, box.height});
    shape.setPosition({box.x, box.y});
    shape.setFillColor(fill);
    if (thickness > 0) {
        shape.setOutlineColor(outline);
        shape.setOutlineThickness(-thickness);
    }
    target.draw(shape);
}

class TextField {
public:
    TextField(Box box, std::string placeholder, int maximumLength = 100)
        : box_(box), placeholder_(std::move(placeholder)),
          maximumLength_(maximumLength), focused_(false) {}

    bool contains(sf::Vector2i point) const {
        return box_.contains(point);
    }

    void setFocused(bool focused) {
        focused_ = focused;
    }

    bool focused() const {
        return focused_;
    }

    std::string value() const {
        return toUtf8(value_);
    }

    void setValue(const std::string& value) {
        value_ = fromUtf8(value);
    }

    void clear() {
        value_.clear();
    }

    void append(char32_t character) {
        if (character >= 32 && character != 127 &&
            static_cast<int>(value_.getSize()) < maximumLength_) {
            value_ += sf::String(character);
        }
    }

    void backspace() {
        if (!value_.isEmpty()) {
            value_.erase(value_.getSize() - 1);
        }
    }

    void draw(sf::RenderTarget& target, const sf::Font& font) const {
        drawBox(target, box_, colors::field,
                focused_ ? colors::accent : colors::border, 2);
        const std::string shown = value_.isEmpty() && !focused_
                                      ? placeholder_
                                      : value();
        drawText(target, font, shown, 15, box_.x + 10, box_.y + 8,
                 value_.isEmpty() && !focused_ ? colors::muted : colors::text);
        if (focused_) {
            sf::Text text(font, value_, 15);
            const float cursorX = box_.x + 11 + text.getLocalBounds().size.x;
            drawBox(target, {cursorX, box_.y + 9, 2, 19}, colors::text);
        }
    }

private:
    Box box_;
    std::string placeholder_;
    sf::String value_;
    int maximumLength_;
    bool focused_;
};

struct ListItem {
    int id;
    std::string label;
};

class ListView {
public:
    ListView(Box box, std::string emptyMessage)
        : box_(box), emptyMessage_(std::move(emptyMessage)), selectedId_(-1), top_(0) {}

    void setBox(Box box) {
        box_ = box;
    }

    void setItems(std::vector<ListItem> items) {
        items_ = std::move(items);
        const bool selectionExists = std::any_of(
            items_.begin(), items_.end(),
            [this](const ListItem& item) { return item.id == selectedId_; });
        if (!selectionExists) {
            selectedId_ = -1;
        }
        limitTop();
    }

    int selectedId() const {
        return selectedId_;
    }

    void handle(const sf::Event& event) {
        if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (click->button == sf::Mouse::Button::Left && box_.contains(click->position)) {
                const int index = top_ +
                    static_cast<int>((static_cast<float>(click->position.y) - box_.y) / 31.f);
                selectedId_ = index >= 0 && index < static_cast<int>(items_.size())
                                  ? items_[static_cast<std::size_t>(index)].id
                                  : -1;
            }
        } else if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
            if (box_.contains(wheel->position)) {
                top_ -= static_cast<int>(wheel->delta * 2);
                limitTop();
            }
        }
    }

    void draw(sf::RenderTarget& target, const sf::Font& font) const {
        drawBox(target, box_, colors::field, colors::border, 1);
        if (items_.empty()) {
            drawText(target, font, emptyMessage_, 14, box_.x + 12, box_.y + 12,
                     colors::muted);
            return;
        }

        const int visible = static_cast<int>(box_.height / 31.f);
        for (int row = 0; row < visible && top_ + row < static_cast<int>(items_.size()); ++row) {
            const int index = top_ + row;
            const float y = box_.y + static_cast<float>(row) * 31.f;
            const ListItem& item = items_[static_cast<std::size_t>(index)];
            const sf::Color fill = item.id == selectedId_
                                      ? colors::selection
                                      : (index % 2 == 0 ? colors::field : colors::panel);
            drawBox(target, {box_.x + 1, y, box_.width - 2, 31}, fill);
            drawText(target, font, item.label, 14, box_.x + 10, y + 6);
        }
        if (static_cast<int>(items_.size()) > visible) {
            const float thumbHeight =
                box_.height * static_cast<float>(visible) / static_cast<float>(items_.size());
            const float travel = box_.height - thumbHeight;
            const int maximumTop = static_cast<int>(items_.size()) - visible;
            const float thumbY = box_.y + (maximumTop > 0
                ? travel * static_cast<float>(top_) / static_cast<float>(maximumTop)
                : 0.f);
            drawBox(target, {box_.x + box_.width - 7, thumbY, 5, thumbHeight},
                    colors::border);
        }
    }

private:
    void limitTop() {
        const int visible = static_cast<int>(box_.height / 31.f);
        const int maximum = std::max(0, static_cast<int>(items_.size()) - visible);
        top_ = std::clamp(top_, 0, maximum);
    }

    Box box_;
    std::string emptyMessage_;
    std::vector<ListItem> items_;
    int selectedId_;
    int top_;
};

inline void drawButton(sf::RenderTarget& target, const sf::Font& font, Box box,
                       const std::string& label, sf::Color fill = colors::accent) {
    drawBox(target, box, fill);
    drawText(target, font, label, 15, box.x + 12, box.y + 10, sf::Color::White, true);
}

}  // namespace ui

#endif
