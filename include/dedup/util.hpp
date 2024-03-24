#ifndef DEDUPLICATOR_DEDUP_UTIL_HPP_
#define DEDUPLICATOR_DEDUP_UTIL_HPP_

#include <filesystem>
#include <functional>

namespace dedup::util {

void walk_dir(const std::filesystem::path& dir, const std::function<void(const std::filesystem::path&)>& callback);

// quote a string to be reused as shell input, e.g. `foo'bar` -> `'foo'\''bar'`
std::string quote(std::string_view str, const char& quote_char = '\'', const char& escape_char = '\\');

} // namespace dedup::util

#endif // DEDUPLICATOR_DEDUP_UTIL_HPP_
