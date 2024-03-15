#ifndef DEDUPLICATOR_FILE_STATUS_HPP_
#define DEDUPLICATOR_FILE_STATUS_HPP_

#include <cstdint>
#include <filesystem>

#include "dedup/context.hpp"
#include "dedup/misc.hpp"

namespace dedup {

class FileStatus {
  friend class Context;
  // takes up to 100 MiB when hashing a file
  static const std::uintmax_t MAX_BYTES2HASH;

public:
  FileStatus(const FileStatus&) = default;
  FileStatus(FileStatus&&) noexcept = default;
  FileStatus& operator=(const FileStatus&) = default;
  FileStatus& operator=(FileStatus&&) noexcept = default;

  explicit FileStatus(const std::filesystem::path& dir);
  explicit FileStatus(std::filesystem::path&& dir);

  virtual ~FileStatus() = default;

  void refresh();

  // abs path of the file
  [[nodiscard]] const std::filesystem::path& dir() const;
  // size of the file
  [[nodiscard]] const std::uintmax_t& size() const;
  // time of last modification of the file
  [[nodiscard]] const std::filesystem::file_time_type& time() const;
  // hash of the file
  [[nodiscard]] const SHA512& hash() const;

protected:
  FileStatus() = default;

  std::filesystem::path dir_;
  std::uintmax_t size_{0};
  std::filesystem::file_time_type time_;
  SHA512 hash_{};
};

bool operator==(const FileStatus& lv, const FileStatus& rv);
bool operator!=(const FileStatus& lv, const FileStatus& rv);

} // namespace dedup

#endif // DEDUPLICATOR_FILE_STATUS_HPP_
