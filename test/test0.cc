#include <gtest/gtest.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "colorful_thread_safe_print.h"

TEST(MyTest, Addition) { EXPECT_EQ(1 + 1, 2); }

TEST(MyTest, ColoredTextRendersAnsiWrappedSegment) {
  const auto text = ctsp::c("hello").green().str();
  EXPECT_EQ(text, "\033[32mhello\033[0m");
}

TEST(MyTest, ColoredTextCanRenderWithoutAnsiCodes) {
  const auto text = ctsp::c("hello").green().str(false);
  EXPECT_EQ(text, "hello");
}

TEST(MyTest, PrintWritesPythonStyleSpacingAndNewline) {
  testing::internal::CaptureStdout();
  ctsp::p("value", 42, 3.5);
  EXPECT_EQ(testing::internal::GetCapturedStdout(), "value 42 3.5\n");
}

TEST(MyTest, PrintSupportsMixedColoredSegments) {
  testing::internal::CaptureStdout();
  ctsp::p(ctsp::c("left").red(), "plain", ctsp::c(7).bright_blue());
  EXPECT_EQ(testing::internal::GetCapturedStdout(), "left plain 7\n");
}

TEST(MyTest, PrintDoesNotPrefixSpaceAfterEmbeddedNewline) {
  testing::internal::CaptureStdout();
  ctsp::p(ctsp::c("Node count:").blue(), 1, ";", ctsp::c("Nodes:").blue(),
          "127.0.0.1:50051\n", ctsp::c("current node:").blue(), "127.0.0.1",
          ":", 50051);
  EXPECT_EQ(testing::internal::GetCapturedStdout(),
            "Node count: 1 ; Nodes: 127.0.0.1:50051\n"
            "current node: 127.0.0.1 : 50051\n");
}

TEST(MyTest, PrintIsThreadSafeAtLineGranularity) {
  testing::internal::CaptureStdout();

  std::vector<std::thread> threads;
  threads.emplace_back([] { ctsp::p("alpha", 1); });
  threads.emplace_back([] { ctsp::p("beta", 2); });
  threads.emplace_back([] { ctsp::p(ctsp::c("gamma").cyan(), 3); });

  for (auto& thread : threads) {
    thread.join();
  }

  const auto output = testing::internal::GetCapturedStdout();
  EXPECT_NE(output.find("alpha 1\n"), std::string::npos);
  EXPECT_NE(output.find("beta 2\n"), std::string::npos);
  EXPECT_NE(output.find("gamma 3\n"), std::string::npos);
}

TEST(MyTest, PrintContainN) {
  testing::internal::CaptureStdout();
  ctsp::p("Line 1", "\n", "Line 2", "\n", "Line 3");
  EXPECT_EQ(testing::internal::GetCapturedStdout(),
            "Line 1\nLine 2\nLine 3\n");
}

TEST(MyTest, ConcurrentPrintKeepsLinesIntactUnderLoad) {
  constexpr int kThreadCount = 8;
  constexpr int kLinesPerThread = 40;

  testing::internal::CaptureStdout();

  std::vector<std::thread> threads;
  for (int thread_index = 0; thread_index < kThreadCount; ++thread_index) {
    threads.emplace_back([thread_index] {
      for (int line_index = 0; line_index < kLinesPerThread; ++line_index) {
        ctsp::p("thread", thread_index, "line", line_index,
                ctsp::c("ok").green());
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
    EXPECT_NE(line.find("ok"), std::string::npos);
  }

  EXPECT_EQ(line_count, kThreadCount * kLinesPerThread);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
