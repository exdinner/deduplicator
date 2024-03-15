#include "dedup/file_status.hpp"

#include <cstdint>
#include <filesystem>
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
    return;
  }
  size_ = std::filesystem::file_size(dir_);
  time_ = std::filesystem::last_write_time(dir_);
  hash_ = sha512(dir_, MAX_BYTES2HASH);
}

[[nodiscard]] const std::filesystem::path& FileStatus::dir() const {
  return dir_;
}

[[nodiscard]] const std::uintmax_t& FileStatus::size() const {
  return size_;
}

[[nodiscard]] const std::filesystem::file_time_type& FileStatus::time() const {
  return time_;
}

[[nodiscard]] const SHA512& FileStatus::hash() const {
  return hash_;
}

bool operator==(const FileStatus& lv, const FileStatus& rv) {
  return &lv == &rv || (lv.size() == rv.size() && lv.hash() == rv.hash());
}

bool operator!=(const FileStatus& lv, const FileStatus& rv) {
  return !(lv == rv);
}

} // namespace dedup
