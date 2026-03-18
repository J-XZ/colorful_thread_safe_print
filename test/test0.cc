#include <gtest/gtest.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "test_print.h"

TEST(MyTest, Addition) {
    EXPECT_EQ(1 + 1, 2);
}

TEST(MyTest, ColoredTextRendersAnsiWrappedSegment) {
    const auto text = test_print::colored("hello").green().render();
    EXPECT_EQ(text, "\033[32mhello\033[0m");
}

TEST(MyTest, ColoredTextCanRenderWithoutAnsiCodes) {
    const auto text = test_print::colored("hello").green().render(false);
    EXPECT_EQ(text, "hello");
}

TEST(MyTest, PrintWritesPythonStyleSpacingAndNewline) {
    testing::internal::CaptureStdout();
    test_print::print("value", 42, 3.5);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "value 42 3.5\n");
}

TEST(MyTest, PrintSupportsMixedColoredSegments) {
    testing::internal::CaptureStdout();
    test_print::print(test_print::colored("left").red(), "plain",
                      test_print::colored(7).bright_blue());
    EXPECT_EQ(testing::internal::GetCapturedStdout(),
              "\033[31mleft\033[0m plain \033[94m7\033[0m\n");
}

TEST(MyTest, PrintIsThreadSafeAtLineGranularity) {
    testing::internal::CaptureStdout();

    std::vector<std::thread> threads;
    threads.emplace_back([] { test_print::print("alpha", 1); });
    threads.emplace_back([] { test_print::print("beta", 2); });
    threads.emplace_back(
        [] { test_print::print(test_print::colored("gamma").cyan(), 3); });

    for (auto& thread : threads) {
        thread.join();
    }

    const auto output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("alpha 1\n"), std::string::npos);
    EXPECT_NE(output.find("beta 2\n"), std::string::npos);
    EXPECT_NE(output.find("\033[36mgamma\033[0m 3\n"), std::string::npos);
}

TEST(MyTest, ConcurrentPrintKeepsLinesIntactUnderLoad) {
    constexpr int kThreadCount = 8;
    constexpr int kLinesPerThread = 40;

    testing::internal::CaptureStdout();

    std::vector<std::thread> threads;
    for (int thread_index = 0; thread_index < kThreadCount; ++thread_index) {
        threads.emplace_back([thread_index] {
            for (int line_index = 0; line_index < kLinesPerThread; ++line_index) {
                test_print::print("thread", thread_index, "line", line_index,
                                  test_print::colored("ok").green());
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    const auto output = testing::internal::GetCapturedStdout();
    std::istringstream stream(output);
    std::string line;
    int line_count = 0;
    while (std::getline(stream, line)) {
        ++line_count;
        EXPECT_NE(line.find("thread "), std::string::npos);
        EXPECT_NE(line.find(" line "), std::string::npos);
        EXPECT_NE(line.find("\033[32mok\033[0m"), std::string::npos);
    }

    EXPECT_EQ(line_count, kThreadCount * kLinesPerThread);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
