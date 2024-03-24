#include "dedup/context.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <optional>
#include <sqlitemm/value.hpp>
#include <string>
#include <tuple>
#include <vector>

#include "sqlitemm/db.hpp"
#include "sqlitemm/stmt.hpp"

#include "dedup/file_status.hpp"
#include "dedup/misc.hpp"
#include "dedup/sql_stmts.hpp"

namespace dedup {

SHA512 blob2sha512(const sqlitemm::Value::Blob& blob) {
  SHA512 sha;
  if (blob.size() != sha.size()) {
    std::ignore = std::fprintf(stderr, "hash size mismatch, truncating\n");
  }
  std::copy_n(blob.begin(), sha.size(), sha.begin());
  return sha;
}

[[nodiscard]] FileStatus Context::query(const std::filesystem::path& file) {
  FileStatus fs;
  db_.prepare(sql::SELECT_BY_DIR)
    .bind(1, sqlitemm::Value::of_text(file.is_absolute() ? file : std::filesystem::absolute(file).lexically_normal()))
    .each_row([&fs](const std::vector<sqlitemm::Value>& row) -> void {
    fs.dir_ = row[0].as<sqlitemm::Value::Text>();
    fs.size_ = row[1].as<sqlitemm::Value::Integer>();
    fs.time_ = row[2].as<sqlitemm::Value::Integer>();
    fs.hash_ = blob2sha512(row[3].as<sqlitemm::Value::Blob>());
  });
  return fs;
}

void Context::update(const FileStatus& fs) {
  db_.prepare(sql::INSERT)
    .bind(1, sqlitemm::Value::of_text(fs.dir_))
    .bind(2, sqlitemm::Value::of_integer(static_cast<std::int64_t>(fs.size_)))
    .bind(3, sqlitemm::Value::of_integer(fs.time_))
    .bind(4, sqlitemm::Value::of_blob({fs.hash_.begin(), fs.hash_.end()}))
    .each_row();
}

void Context::clean() {
  db_.exec(sql::SELECT_ALL_NAMES, [](const std::vector<sqlitemm::Value>& row) -> void {
    const std::string& file = row[0].as<sqlitemm::Value::Text>();
    if (!std::filesystem::is_regular_file(file)) {
      db_.prepare(sql::DELETE_BY_DIR).bind(1, sqlitemm::Value::of_text(file.c_str())).each_row();
    }
  });
}

void Context::update_non_existing(const std::filesystem::path& file) {
  FileStatus status = query(file);
  if (status.no_status()) {
    update(FileStatus{file});
  }
}

void Context::update_modified(const std::filesystem::path& file) {
  FileStatus status = query(file);
  if (status.no_status()) {
    update(FileStatus{file});
    return;
  }
  if (status.time() != FileStatus::time(file)) {
    update(FileStatus{file});
    return;
  }
}

void Context::update_different(const std::filesystem::path& file) {
  FileStatus status = query(file);
  if (status.no_status()) {
    update(FileStatus{file});
    return;
  }
  FileStatus new_status{file};
  if (status.hash() != new_status.hash()) {
    update(new_status);
    return;
  }
}

[[nodiscard]] std::vector<SHA512> Context::query_dup_hashes() {
  std::vector<SHA512> dup_hashes;
  db_.exec(sql::SELECT_DUP_HASH, [&dup_hashes](const std::vector<sqlitemm::Value>& row) -> void {
    dup_hashes.emplace_back(blob2sha512(row[0].as<sqlitemm::Value::Blob>()));
  });
  return dup_hashes;
}

[[nodiscard]] std::vector<SHA512> Context::query_dup_hashes(const std::filesystem::path& parent_dir) {
  std::vector<SHA512> dup_hashes;
  db_.prepare(sql::SELECT_DUP_HASH_UNDER_DIR)
    .bind(1,
          sqlitemm::Value::of_text(parent_dir.is_absolute()
                                     ? parent_dir.c_str()
                                     : std::filesystem::absolute(parent_dir).lexically_normal().c_str()))
    .each_row([&dup_hashes](const std::vector<sqlitemm::Value>& row) -> void {
    dup_hashes.emplace_back(blob2sha512(row[0].as<sqlitemm::Value::Blob>()));
  });
  return dup_hashes;
}

[[nodiscard]] std::vector<std::string> Context::query_dup_files_by_hash(const SHA512& hash) {
  std::vector<std::string> dup_files;
  db_.prepare(sql::SELECT_DUP_SIZE_BY_HASH)
    .bind(1, sqlitemm::Value::of_blob(hash))
    .each_row([&dup_files](const std::vector<sqlitemm::Value>& row) -> void {
    dup_files.emplace_back(row[0].as<sqlitemm::Value::Text>());
  });
  return dup_files;
}

/* NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) */
sqlitemm::DB Context::db_{[]() -> sqlitemm::DB {
  // `~/.config/deduplicator/db`
  std::optional<std::filesystem::path> home_dir_opt = get_home_dir();
  if (!home_dir_opt.has_value()) {
    home_dir_opt = getenv_safe("HOME");
  }
  if (!home_dir_opt.has_value()) {
    std::ignore = std::fprintf(stderr, "Failed to get user directory.\nExiting because data file could not be read.");
    std::terminate();
  }
  std::filesystem::path db_dir = home_dir_opt.value() / ".config/deduplicator";
  if (!std::filesystem::exists(db_dir)) {
    std::filesystem::create_directories(db_dir);
    if (!std::filesystem::is_directory(db_dir)) {
      std::ignore = std::fprintf(
        stderr, "Failed to create directory `%s`.\nExiting because data file could not be read.", db_dir.c_str());
      exit_safe(-1);
    }
  }
  sqlitemm::DB db{db_dir / "db"};
  std::vector<std::string> table_names = db.table_names();
  bool table_exists = std::any_of(table_names.begin(), table_names.end(), [](const std::string& name) -> bool {
    return name == "dedup";
  });
  if (!table_exists) {
    db.exec(sql::CREATE);
  }
  return db;
}()};

} // namespace dedup
