#include "dedup/util.hpp"

#include <cstdio>
#include <filesystem>
#include <tuple>

namespace dedup::util {

void walk_dir(const std::filesystem::path& dir, const std::function<void(const std::filesystem::path&)>& callback) {
  if (!std::filesystem::is_directory(dir)) {
    std::ignore = std::fprintf(stderr, "failed to walk directory `%s`: not a directory.\n", dir.c_str());
    return;
  }
  for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator{dir}) {
    if (entry.is_regular_file()) {
      const std::filesystem::path& file = entry.path();
      callback(file.is_absolute() ? file : std::filesystem::absolute(file).lexically_normal());
    }
  }
}

std::string quote(std::string_view str, const char& quote_char, const char& escape_char) {
  std::string quoted;
  quoted.reserve(str.length() + 8);
  quoted.push_back(quote_char);
  for (const char& c : str) {
    if (c == quote_char) {
      quoted.push_back(quote_char);
      quoted.push_back(escape_char);
      quoted.push_back(quote_char);
      quoted.push_back(quote_char);
    } else {
      quoted.push_back(c);
    }
  }
  quoted.push_back(quote_char);
  return quoted;
}

} // namespace dedup::util
