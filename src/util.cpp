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

} // namespace dedup::util
