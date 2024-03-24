#ifndef DEDUPLICATOR_DEDUP_UTIL_HPP_
#define DEDUPLICATOR_DEDUP_UTIL_HPP_

#include <filesystem>
#include <functional>

namespace dedup::util {

void walk_dir(const std::filesystem::path& dir, const std::function<void(const std::filesystem::path&)>& callback);

// quote a string, e.g. `foo"bar` -> `"foo\"bar"`
std::string quote(std::string_view str, const char& quote = '"', const char& escape = '\\');

} // namespace dedup::util

#endif // DEDUPLICATOR_DEDUP_UTIL_HPP_
