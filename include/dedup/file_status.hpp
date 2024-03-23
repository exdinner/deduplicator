#ifndef DEDUPLICATOR_FILE_STATUS_HPP_
#define DEDUPLICATOR_FILE_STATUS_HPP_

#include <cstdint>
#include <filesystem>

#include "dedup/misc.hpp"

namespace dedup {

class FileStatus {
  friend class Context; // include/dedup/context.hpp
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
  [[nodiscard]] static std::uintmax_t size(const std::filesystem::path& file);
  // time of last modification of the file
  [[nodiscard]] const std::int64_t& time() const;
  [[nodiscard]] static std::int64_t time(const std::filesystem::path& file);
  // hash of the file
  [[nodiscard]] const SHA512& hash() const;
  [[nodiscard]] static SHA512 hash(const std::filesystem::path& file);
  // if it is invalid
  [[nodiscard]] bool no_status() const;

protected:
  FileStatus();

  std::filesystem::path dir_;
  std::uintmax_t size_{0};
  std::int64_t time_{0};
  SHA512 hash_{};
};

bool operator==(const FileStatus& lv, const FileStatus& rv);
bool operator!=(const FileStatus& lv, const FileStatus& rv);

} // namespace dedup

#endif // DEDUPLICATOR_FILE_STATUS_HPP_
