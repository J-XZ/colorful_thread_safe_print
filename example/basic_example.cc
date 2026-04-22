#include "colorful_thread_safe_print.h"

int main() {
  ctsp::p("plain", "text", 42);

  ctsp::p(ctsp::c("error").red(), ctsp::c("warning").yellow(),
          ctsp::c("success").green(), ctsp::c("info").dark_yellow());

  ctsp::p("mixed", ctsp::c("[blue]").bright_blue(), "segments",
          ctsp::c(2026).bright_magenta());

  return 0;
}
