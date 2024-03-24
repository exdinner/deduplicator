#ifndef DEDUPLICATOR_CONTEXT_HPP_
#define DEDUPLICATOR_CONTEXT_HPP_

#include <filesystem>
#include <string>
#include <vector>

#include "sqlitemm/db.hpp"

#include "dedup/file_status.hpp"

namespace dedup {

class Context {
public:
  Context() = default;
  Context(const Context&) = delete;
  Context(Context&&) noexcept = default;
  Context& operator=(const Context&) = delete;
  Context& operator=(Context&&) noexcept = default;

  virtual ~Context() = default;

  // get info of `file` in database
  [[nodiscard]] static FileStatus query(const std::filesystem::path& file);
  // insert or update
  static void update(const FileStatus& fs);
  // remove info about deleted files in database
  static void clean();

  // update info if `file` is not recorded in datebase
  static void update_non_existing(const std::filesystem::path& file);
  // update info if the last modified time of `file` does not match that in database
  static void update_modified(const std::filesystem::path& file);
  // update info if the SHA512 digest of `file` does not match that in database
  static void update_different(const std::filesystem::path& file);
  // query for hash of duplicated files in database
  [[nodiscard]] static std::vector<SHA512> query_dup_hashes();
  // same, but only query hashes of those under `parent_dir`
  [[nodiscard]] static std::vector<SHA512> query_dup_hashes(const std::filesystem::path& parent_dir);
  // query duplicated files of same size by hash returned by `query_dup_hashes`
  [[nodiscard]] static std::vector<std::string> query_dup_files_by_hash(const SHA512& hash);

protected:
  /* NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) */
  static sqlitemm::DB db_;
};

} // namespace dedup

#endif // DEDUPLICATOR_CONTEXT_HPP_
