#include "dedup/misc.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <mutex>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "openssl/crypto.h"
#include "openssl/evp.h"
#include "pwd.h"
#include "unistd.h"

#define BUF_SIZE 1024

namespace dedup {

SHA512 sha512(const std::filesystem::path& file) {
  if (!std::filesystem::is_regular_file(file)) {
    std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: not a regular file\n", file.c_str());
    return {};
  }
  SHA512 hash;
  std::ifstream f{file, std::ios::in | std::ios::binary};
  if (f.bad()) {
    std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: failed to open.\n", file.c_str());
    return {};
  }
  char buf[BUF_SIZE];
  EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
  if (EVP_DigestInit_ex2(md_ctx, EVP_sha512(), nullptr) == 0) {
    std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: OpenSSL EVP_DigestInit_ex2 failed.\n", file.c_str());
    EVP_MD_CTX_free(md_ctx);
    return {};
  }
  while (f) {
    f.read(buf, sizeof(buf));
    if (EVP_DigestUpdate(md_ctx, buf, f.gcount()) == 0) {
      std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: OpenSSL EVP_DigestUpdate failed.\n", file.c_str());
      EVP_MD_CTX_free(md_ctx);
      return {};
    }
  }
  if (EVP_DigestFinal_ex(md_ctx, hash.data(), nullptr) == 0) {
    std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: OpenSSL EVP_DigestFinal_ex failed.\n", file.c_str());
    EVP_MD_CTX_free(md_ctx);
    return {};
  }
  EVP_MD_CTX_free(md_ctx);
  return hash;
}

SHA512 sha512(const std::filesystem::path& file, const std::uintmax_t& max_bytes) {
  if (!std::filesystem::is_regular_file(file)) {
    std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: not a regular file\n", file.c_str());
    return {};
  }
  std::uintmax_t bytes_count{0};
  SHA512 hash;
  char buf[BUF_SIZE];
  std::ifstream f{file, std::ios::in | std::ios::binary};
  if (f.bad()) {
    std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: failed to open.\n", file.c_str());
    return {};
  }
  EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
  if (EVP_DigestInit_ex2(md_ctx, EVP_sha512(), nullptr) == 0) {
    std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: OpenSSL EVP_DigestInit_ex2 failed.\n", file.c_str());
    EVP_MD_CTX_free(md_ctx);
    return {};
  }
  while (f && max_bytes > bytes_count) {
    if (bytes_count + sizeof(buf) <= max_bytes) {
      f.read(buf, sizeof(buf));
    } else {
      f.read(buf, static_cast<std::streamsize>(max_bytes - bytes_count));
    }
    bytes_count += f.gcount();
    if (EVP_DigestUpdate(md_ctx, buf, f.gcount()) == 0) {
      std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: OpenSSL EVP_DigestUpdate failed.\n", file.c_str());
      EVP_MD_CTX_free(md_ctx);
      return {};
    }
  }
  if (EVP_DigestFinal_ex(md_ctx, hash.data(), nullptr) == 0) {
    std::ignore = std::fprintf(stderr, "Failed to hash file `%s`: OpenSSL EVP_DigestFinal_ex failed.\n", file.c_str());
    EVP_MD_CTX_free(md_ctx);
    return {};
  }
  EVP_MD_CTX_free(md_ctx);
  return hash;
}

SHA512 sha512(const std::vector<std::uint8_t>& data) {
  SHA512 hash;
  EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
  if (EVP_DigestInit_ex2(md_ctx, EVP_sha512(), nullptr) == 0) {
    std::ignore = std::fprintf(stderr, "Failed to hash data (%p): OpenSSL EVP_DigestInit_ex2 failed.\n", &data);
    EVP_MD_CTX_free(md_ctx);
    return {};
  }
  if (EVP_DigestUpdate(md_ctx, data.data(), data.size()) == 0) {
    std::ignore = std::fprintf(stderr, "Failed to hash data (%p): OpenSSL EVP_DigestUpdate failed.\n", &data);
    EVP_MD_CTX_free(md_ctx);
    return {};
  }
  if (EVP_DigestFinal_ex(md_ctx, hash.data(), nullptr) == 0) {
    std::ignore = std::fprintf(stderr, "Failed to hash data (%p): OpenSSL EVP_DigestFinal_ex failed.\n", &data);
    EVP_MD_CTX_free(md_ctx);
    return {};
  }
  EVP_MD_CTX_free(md_ctx);
  return hash;
}

std::string data2hexstr(const std::vector<std::uint8_t>& data) {
  return data2hexstr(data.data(), data.size());
}

std::string data2hexstr(const std::uint8_t* p_data, const std::size_t& data_len) {
  char buf[data_len * 2 + 4];
  std::size_t result_len{0};
  if (OPENSSL_buf2hexstr_ex(buf, sizeof(buf), &result_len, p_data, data_len, '\0') == 0) {
    std::ignore = std::fprintf(stderr, "OpenSSL OPENSSL_buf2hexstr_ex failed.\n");
    return {};
  }
  return {buf, buf + result_len};
}

std::vector<std::uint8_t> hexstr2data(const std::string& hexstr) {
  return hexstr2data(hexstr.data(), hexstr.length());
}

std::vector<std::uint8_t> hexstr2data(const char* hexstr, const std::size_t& len) {
  std::uint8_t buf[len / 2 + 2];
  std::size_t result_len{0};
  if (OPENSSL_hexstr2buf_ex(buf, sizeof(buf), &result_len, hexstr, '\0') == 0) {
    std::ignore = std::fprintf(stderr, "OpenSSL OPENSSL_buf2hexstr_ex failed.\n");
    return {};
  }
  return {buf, buf + result_len};
}

void exit_safe(const int& status) {
  static std::mutex exit_mutex;
  std::unique_lock<std::mutex> getenv_lock(exit_mutex);
  /* NOLINTNEXTLINE(concurrency-mt-unsafe) */
  std::exit(status);
}

std::optional<std::string> getenv_safe(const std::string& name) {
  static std::mutex getenv_mutex;
  std::unique_lock<std::mutex> getenv_lock(getenv_mutex);
  /* NOLINTNEXTLINE(concurrency-mt-unsafe) */
  char* value = std::getenv(name.c_str());
  if (value == nullptr) {
    return std::nullopt;
  }
  return value;
}

std::optional<std::filesystem::path> get_home_dir() {
  return get_home_dir(getuid());
}

std::optional<std::filesystem::path> get_home_dir(const std::uint32_t& uid) {
  std::size_t pwd_buf_len = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (pwd_buf_len == -1) {
    pwd_buf_len = 4096;
  }
  char pwd_buf[pwd_buf_len];
  passwd pwd_result{};
  passwd* p_pwd_result = nullptr;
  int e = getpwuid_r(uid, &pwd_result, pwd_buf, pwd_buf_len, &p_pwd_result);
  if (e != 0 || p_pwd_result == nullptr) {
    return std::nullopt;
  }
  return std::filesystem::path{pwd_result.pw_dir};
}

std::optional<std::filesystem::path> get_home_dir(const std::string& username) {
  std::size_t pwd_buf_len = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (pwd_buf_len == -1) {
    pwd_buf_len = 4096;
  }
  char pwd_buf[pwd_buf_len];
  passwd pwd_result{};
  passwd* p_pwd_result = nullptr;
  int e = getpwnam_r(username.c_str(), &pwd_result, pwd_buf, pwd_buf_len, &p_pwd_result);
  if (e != 0 || p_pwd_result == nullptr) {
    return std::nullopt;
  }
  return std::filesystem::path{pwd_result.pw_dir};
}

} // namespace dedup
