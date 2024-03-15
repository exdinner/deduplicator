#include "dedup/context.hpp"

#include <cstdio>
#include <filesystem>
#include <optional>
#include <tuple>

#include "sqlitemm/db.hpp"

#include "dedup/misc.hpp"

namespace dedup {

/* NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) */
sqlitemm::DB Context::db{[]() -> std::filesystem::path {
  // `~/.config/deduplicator/db`
  std::filesystem::path home_dir{[]() -> std::filesystem::path {
    std::optional<std::filesystem::path> dir = get_home_dir();
    if (dir.has_value()) {
      return dir.value();
    }
    dir = getenv_safe("HOME");
    if (dir.has_value()) {
      return dir.value();
    }
    std::ignore = std::fprintf(stderr, "Failed to get user directory.\nExiting because data file could not be read.");
    exit_safe(-1);
    return {};
  }()};
  std::filesystem::path db_dir = home_dir / ".config/deduplicator";
  if (!std::filesystem::exists(db_dir)) {
    std::filesystem::create_directories(db_dir);
    if (!std::filesystem::is_directory(db_dir)) {
      std::ignore = std::fprintf(
        stderr, "Failed to create directory `%s`.\nExiting because data file could not be read.", db_dir.c_str());
      exit_safe(-1);
    }
  }
  return db_dir / "db";
}()};

} // namespace dedup
