#include "dedup/file_status.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <tuple>
#include <utility>

#include "dedup/misc.hpp"

namespace dedup {

const std::uintmax_t FileStatus::MAX_BYTES2HASH{static_cast<const std::uintmax_t>(100 * 1024 * 1024)};

FileStatus::FileStatus(const std::filesystem::path& dir) : dir_(dir) {
  refresh();
}

FileStatus::FileStatus(std::filesystem::path&& dir) : dir_(std::move(dir)) {
  refresh();
}

void FileStatus::refresh() {
  if (dir_.empty()) {
    dir_ = "NO_STATUS";
    return;
  }
  if (!dir_.is_absolute()) {
    dir_ = std::filesystem::absolute(dir_).lexically_normal();
  }
  if (!std::filesystem::is_regular_file(dir_)) {
    std::ignore = std::fprintf(stderr, "failed to get info about `%s`: not a regular file\n", dir_.c_str());
    dir_ = "NO_STATUS";
    return;
  }
  size_ = std::filesystem::file_size(dir_);
  // is not really UNIX time
  time_ = std::chrono::time_point_cast<std::chrono::seconds>(std::filesystem::last_write_time(dir_))
            .time_since_epoch()
            .count();
  hash_ = sha512(dir_, MAX_BYTES2HASH);
}

[[nodiscard]] const std::filesystem::path& FileStatus::dir() const {
  return dir_;
}

[[nodiscard]] const std::uintmax_t& FileStatus::size() const {
  return size_;
}

[[nodiscard]] std::uintmax_t FileStatus::size(const std::filesystem::path& file) {
  return std::filesystem::file_size(file);
}

[[nodiscard]] const std::int64_t& FileStatus::time() const {
  return time_;
}

[[nodiscard]] std::int64_t FileStatus::time(const std::filesystem::path& file) {
  return std::chrono::time_point_cast<std::chrono::seconds>(std::filesystem::last_write_time(file))
    .time_since_epoch()
    .count();
}

[[nodiscard]] const SHA512& FileStatus::hash() const {
  return hash_;
}

[[nodiscard]] SHA512 FileStatus::hash(const std::filesystem::path& file) {
  return sha512(file, MAX_BYTES2HASH);
}

[[nodiscard]] bool FileStatus::no_status() const {
  return dir_ == "NO_STATUS";
  dir_.string();
}

FileStatus::FileStatus() {
  refresh();
}

bool operator==(const FileStatus& lv, const FileStatus& rv) {
  return &lv == &rv || (lv.size() == rv.size() && lv.hash() == rv.hash());
}

bool operator!=(const FileStatus& lv, const FileStatus& rv) {
  return !(lv == rv);
}

} // namespace dedup
