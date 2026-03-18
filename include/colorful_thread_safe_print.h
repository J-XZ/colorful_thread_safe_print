#pragma once

#include <unistd.h>

#include <cstdio>
#include <sstream>
#include <string>
#include <utility>

namespace colorful_thread_safe_print {

enum class clr {
  def = 39,
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

class seg {
 public:
  explicit seg(std::string text) : text_(std::move(text)) {}

  seg& fg(clr value) {
    color_ = value;
    return *this;
  }

  seg& black() { return fg(clr::black); }
  seg& red() { return fg(clr::red); }
  seg& green() { return fg(clr::green); }
  seg& yellow() { return fg(clr::yellow); }
  seg& blue() { return fg(clr::blue); }
  seg& magenta() { return fg(clr::magenta); }
  seg& cyan() { return fg(clr::cyan); }
  seg& white() { return fg(clr::white); }
  seg& bright_black() { return fg(clr::bright_black); }
  seg& bright_red() { return fg(clr::bright_red); }
  seg& bright_green() { return fg(clr::bright_green); }
  seg& bright_yellow() { return fg(clr::bright_yellow); }
  seg& bright_blue() { return fg(clr::bright_blue); }
  seg& bright_magenta() { return fg(clr::bright_magenta); }
  seg& bright_cyan() { return fg(clr::bright_cyan); }
  seg& bright_white() { return fg(clr::bright_white); }
  seg& reset() { return fg(clr::def); }

  std::string str(bool color_on = true) const {
    if (!color_on || color_ == clr::def) {
      return text_;
    }

    std::ostringstream out;
    out << "\033[" << static_cast<int>(color_) << "m" << text_ << "\033[0m";
    return out.str();
  }

 private:
  std::string text_;
  clr color_ = clr::def;
};

inline bool tty() { return ::isatty(fileno(stdout)) != 0; }

inline void put(std::ostringstream& out, const seg& value, bool color_on) {
  out << value.str(color_on);
}

template <typename T>
inline void put(std::ostringstream& out, const T& value, bool color_on) {
  (void)color_on;
  out << value;
}

template <typename T>
seg c(T&& value) {
  std::ostringstream out;
  out << std::forward<T>(value);
  return seg(out.str());
}

template <typename... Args>
void p(Args&&... args) {
  const bool color_on = tty();
  std::ostringstream out;
  bool first = true;
  auto one = [&](const auto& value) {
    if (!first) {
      out << ' ';
    }
    first = false;
    put(out, value, color_on);
  };
  (one(std::forward<Args>(args)), ...);
  out << '\n';

  const std::string text = out.str();
  std::printf("%s", text.c_str());
  std::fflush(stdout);
}

}  // namespace colorful_thread_safe_print

namespace ctsp = colorful_thread_safe_print;
