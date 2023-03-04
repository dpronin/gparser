#include <cstring>

#include <iostream>
#include <iterator>

#include "gparser.hpp"

int main(int argc, char const *argv[]) {
  if (argc < 2 || 0 == std::strcmp(argv[1], "-")) {
    gtask::parse(std::istream_iterator<char>(std::cin),
                 std::istream_iterator<char>(),
                 std::ostream_iterator<char>(std::cout));
    return 0;
  }

  for (int i = 1; i < argc; ++i) {
    gtask::parse(argv[i], std::ostream_iterator<char>(std::cout));
    std::cout << std::endl;
  }

  return 0;
}
