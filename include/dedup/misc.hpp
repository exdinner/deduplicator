#ifndef DEDUPLICATOR_DEDUP_MISC_HPP_
#define DEDUPLICATOR_DEDUP_MISC_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace dedup {

using SHA512 = std::array<std::uint8_t, 64>;

SHA512 sha512(const std::filesystem::path& file);
SHA512 sha512(const std::filesystem::path& file, const std::uintmax_t& max_bytes);
SHA512 sha512(const std::vector<std::uint8_t>& data);

std::string data2hexstr(const std::uint8_t* p_data, const std::size_t& data_len);
std::string data2hexstr(const std::vector<std::uint8_t>& data);

template <std::size_t data_len>
std::string data2hexstr(const std::array<std::uint8_t, data_len>& data) {
  return data2hexstr(data.data(), data_len);
}

std::vector<std::uint8_t> hexstr2data(const std::string& hexstr);
std::vector<std::uint8_t> hexstr2data(const char* hexstr, const std::size_t& len);

void exit_safe(const int& status);

std::optional<std::string> getenv_safe(const std::string& name);

std::optional<std::filesystem::path> get_home_dir();
std::optional<std::filesystem::path> get_home_dir(const std::uint32_t& uid);
std::optional<std::filesystem::path> get_home_dir(const std::string& username);

} // namespace dedup

#endif // DEDUPLICATOR_DEDUP_MISC_HPP_
