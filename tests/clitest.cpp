#include <CppLogger/logger.hpp>

#include <cassert>
#include <print>

namespace test {

void testBasic();

} // namespace test

int main() {
  std::println("Start main()\n");
  test::testBasic();
  std::println("\nend main()\n");
  return 0;
}

void test::testBasic() {
  constexpr std::string_view expect = "str fmt[10]";
  logger::logWarn("str fmt [{}]", 10);
}
