#pragma once

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace test_print {

int add(int a, int b);

enum class color_code {
    default_color = 39,
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
    blue = 34,
    magenta = 35,
    cyan = 36,
    white = 37,
    bright_black = 90,
    bright_red = 91,
    bright_green = 92,
    bright_yellow = 93,
    bright_blue = 94,
    bright_magenta = 95,
    bright_cyan = 96,
    bright_white = 97,
};

class styled_text {
public:
    explicit styled_text(std::string text) : text_(std::move(text)) {}

    styled_text& color(color_code value) {
        color_ = value;
        return *this;
    }

    styled_text& black() { return color(color_code::black); }
    styled_text& red() { return color(color_code::red); }
    styled_text& green() { return color(color_code::green); }
    styled_text& yellow() { return color(color_code::yellow); }
    styled_text& blue() { return color(color_code::blue); }
    styled_text& magenta() { return color(color_code::magenta); }
    styled_text& cyan() { return color(color_code::cyan); }
    styled_text& white() { return color(color_code::white); }
    styled_text& bright_black() { return color(color_code::bright_black); }
    styled_text& bright_red() { return color(color_code::bright_red); }
    styled_text& bright_green() { return color(color_code::bright_green); }
    styled_text& bright_yellow() { return color(color_code::bright_yellow); }
    styled_text& bright_blue() { return color(color_code::bright_blue); }
    styled_text& bright_magenta() { return color(color_code::bright_magenta); }
    styled_text& bright_cyan() { return color(color_code::bright_cyan); }
    styled_text& bright_white() { return color(color_code::bright_white); }
    styled_text& reset() { return color(color_code::default_color); }

    std::string render(bool enable_color = true) const {
        if (!enable_color || color_ == color_code::default_color) {
            return text_;
        }

        std::ostringstream out;
        out << "\033[" << static_cast<int>(color_) << "m";
        out << text_;
        out << "\033[0m";
        return out.str();
    }

private:
    std::string text_;
    color_code color_ = color_code::default_color;
};

inline bool stdout_supports_color() {
    return ::isatty(fileno(stdout)) != 0;
}

inline void append_print_arg(std::ostringstream& out, const styled_text& value,
                             bool enable_color) {
    out << value.render(enable_color);
}

template <typename T>
inline void append_print_arg(std::ostringstream& out, const T& value,
                             bool enable_color) {
    (void)enable_color;
    out << value;
}

template <typename T>
styled_text colored(T&& value) {
    std::ostringstream out;
    out << std::forward<T>(value);
    return styled_text(out.str());
}

template <typename... Args>
void print(Args&&... args) {
    const bool enable_color = stdout_supports_color();
    std::ostringstream out;
    bool first = true;
    auto append_one = [&](const auto& value) {
        if (!first) {
            out << ' ';
        }
        first = false;
        append_print_arg(out, value, enable_color);
    };
    (append_one(std::forward<Args>(args)), ...);
    out << '\n';

    const std::string rendered = out.str();
    std::printf("%s", rendered.c_str());
    std::fflush(stdout);
}

}  // namespace test_print
