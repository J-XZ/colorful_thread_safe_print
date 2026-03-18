#include "test_print.h"

int main() {
    test_print::print("plain", "text", 42);

    test_print::print(test_print::colored("error").red(),
                      test_print::colored("warning").yellow(),
                      test_print::colored("success").green());

    test_print::print("mixed",
                      test_print::colored("[blue]").bright_blue(),
                      "segments",
                      test_print::colored(2026).bright_magenta());

    return 0;
}
