#include <cstdio>
#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

#include "dedup/context.hpp"
#include "dedup/file_status.hpp"
#include "dedup/misc.hpp"
#include "dedup/util.hpp"

void print_help(const char* command) {
  std::ignore = std::fprintf(stderr,
                             "Usage: %s <dir>\n"
                             "\n"
                             "  scan duplicated files under <dir>.\n",
                             command);
}

/* NOLINTNEXTLINE(misc-unused-parameters) */
int main(int argc, const char* argv[]) {
  if (argc != 2) {
    print_help(argv[0]);
    return 1;
  }
  std::filesystem::path dir{argv[1]};
  if (!std::filesystem::is_directory(dir)) {
    std::ignore = std::fprintf(stderr, "`%s` is not a directory.\n", argv[1]);
    print_help(argv[1]);
    return 1;
  }
  if (dir.is_relative()) {
    dir = std::filesystem::absolute(dir).lexically_normal();
  }
  dedup::util::walk_dir(dir, [](const std::filesystem::path& file) -> void {
    std::ignore = std::fprintf(stderr, "%s\n", file.c_str());
    dedup::Context::update_modified(file);
  });
  std::ignore = std::fprintf(stderr, "\n");
  std::vector<dedup::SHA512> dup_hashes = dedup::Context::query_dup_hashes(dir);
  for (const dedup::SHA512& dup_hash : dup_hashes) {
    std::vector<std::string> dup_files = dedup::Context::query_dup_files_by_hash(dup_hash);
    std::printf("# ========== duplicated ==========\n");
    for (const std::string& dup_file : dup_files) {
      std::printf("#rm %s\n", dedup::util::quote(dup_file).c_str());
    }
    std::printf("# ================================\n\n");
  }
  return 0;
}
