#pragma once

#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <sstream>
#include <string>
#include <string_view>
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

inline void put(std::ostringstream& out, bool v, bool color_on) {
  (void)color_on;
  out << (v ? "true" : "false");
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

inline std::string format_hex(uint64_t value) {
  std::ostringstream out;
  out << "0x" << std::hex << value;
  return out.str();
}

inline std::string format_bin(uint8_t value) {
  std::string out = "0b";
  for (int bit = 7; bit >= 0; --bit) {
    out.push_back(((value >> bit) & 0x1U) != 0 ? '1' : '0');
  }
  return out;
}

inline bool ends_with_newline_after_ansi(std::string_view text) {
  size_t end = text.size();
  while (end > 0) {
    if (text[end - 1] == '\n') {
      return true;
    }

    if (text[end - 1] != 'm') {
      return false;
    }

    const size_t esc = text.rfind('\033', end - 1);
    if (esc == std::string_view::npos || esc + 1 >= end ||
        text[esc + 1] != '[') {
      return false;
    }

    for (size_t i = esc + 2; i + 1 < end; ++i) {
      const char ch = text[i];
      if ((ch < '0' || ch > '9') && ch != ';') {
        return false;
      }
    }

    end = esc;
  }

  return false;
}

inline bool starts_with_newline_after_ansi(std::string_view text) {
  size_t pos = 0;
  while (pos < text.size()) {
    if (text[pos] == '\n') {
      return true;
    }

    if (text[pos] != '\033') {
      return false;
    }
    if (pos + 1 >= text.size() || text[pos + 1] != '[') {
      return false;
    }

    size_t i = pos + 2;
    for (; i < text.size(); ++i) {
      const char ch = text[i];
      if (ch == 'm') {
        pos = i + 1;
        break;
      }
      if ((ch < '0' || ch > '9') && ch != ';') {
        return false;
      }
    }

    if (i >= text.size()) {
      return false;
    }
  }

  return false;
}

template <typename... Args>
void p(Args&&... args) {
  const bool color_on = tty();
  std::ostringstream out;
  bool first = true;
  bool at_line_start = true;
  auto one = [&](const auto& value) {
    std::ostringstream piece_stream;
    put(piece_stream, value, color_on);
    const std::string piece = piece_stream.str();
    const bool starts_with_newline = starts_with_newline_after_ansi(piece);

    if (!first && !at_line_start && !starts_with_newline) {
      out << ' ';
    }
    first = false;
    out << piece;

    if (!piece.empty()) {
      at_line_start = ends_with_newline_after_ansi(piece);
    }
  };
  (one(std::forward<Args>(args)), ...);
  out << '\n';

  const std::string text = out.str();
  std::fwrite(text.data(), 1, text.size(), stdout);
  std::fflush(stdout);
}

template <typename... Args>
void pp(Args&&... args) {
  const bool color_on = tty();
  std::ostringstream out;
  bool first = true;
  bool at_line_start = true;
  auto one = [&](const auto& value) {
    std::ostringstream piece_stream;
    put(piece_stream, value, false);
    const std::string piece = piece_stream.str();
    const bool starts_with_newline = starts_with_newline_after_ansi(piece);

    if (!first && !at_line_start && !starts_with_newline) {
      out << ' ';
    }
    first = false;
    out << piece;

    if (!piece.empty()) {
      at_line_start = ends_with_newline_after_ansi(piece);
    }
  };
  (one(std::forward<Args>(args)), ...);
  out << '\n';

  const std::string text = out.str();
  if (!color_on) {
    std::fwrite(text.data(), 1, text.size(), stdout);
    std::fflush(stdout);
    return;
  }

  std::ostringstream wrapped;
  wrapped << "\033[" << static_cast<int>(clr::magenta) << "m" << text
          << "\033[0m";
  const std::string colored = wrapped.str();
  std::fwrite(colored.data(), 1, colored.size(), stdout);
  std::fflush(stdout);
}

template <typename... Args>
void pb(Args&&... args) {
  const bool color_on = tty();
  std::ostringstream out;
  bool first = true;
  bool at_line_start = true;
  auto one = [&](const auto& value) {
    std::ostringstream piece_stream;
    put(piece_stream, value, false);
    const std::string piece = piece_stream.str();
    const bool starts_with_newline = starts_with_newline_after_ansi(piece);

    if (!first && !at_line_start && !starts_with_newline) {
      out << ' ';
    }
    first = false;
    out << piece;

    if (!piece.empty()) {
      at_line_start = ends_with_newline_after_ansi(piece);
    }
  };
  (one(std::forward<Args>(args)), ...);
  out << '\n';

  const std::string text = out.str();
  if (!color_on) {
    std::fwrite(text.data(), 1, text.size(), stdout);
    std::fflush(stdout);
    return;
  }

  std::ostringstream wrapped;
  wrapped << "\033[30;47m" << text << "\033[0m";
  const std::string colored = wrapped.str();
  std::fwrite(colored.data(), 1, colored.size(), stdout);
  std::fflush(stdout);
}

}  // namespace colorful_thread_safe_print

namespace ctsp = colorful_thread_safe_print;  // NOLINT(misc-unused-alias-decls)
