#ifndef DEDUPLICATOR_DEDUP_SQL_STMTS_HPP_
#define DEDUPLICATOR_DEDUP_SQL_STMTS_HPP_

#include <string_view>

namespace dedup::sql {

constexpr const std::string_view CREATE{
  "CREATE TABLE dedup(dir TEXT PRIMARY KEY, size INTEGER, time INTEGER, hash BLOB);"};
constexpr const std::string_view SELECT_BY_DIR{"SELECT * FROM dedup WHERE dir == ?;"};
constexpr const std::string_view SELECT_ALL_NAMES{"SELECT dir FROM dedup;"};
constexpr const std::string_view INSERT{"INSERT OR REPLACE INTO dedup VALUES (?, ?, ?, ?);"};
constexpr const std::string_view DELETE_BY_DIR{"DELETE FROM dedup WHERE dir == ?;"};
constexpr const std::string_view SELECT_DUP_HASH{"SELECT hash FROM dedup GROUP BY hash HAVING count(*) >= 2;"};
constexpr const std::string_view SELECT_DUP_HASH_UNDER_DIR{
  "SELECT hash FROM dedup WHERE dir LIKE ? || '%' GROUP BY hash HAVING count(*) >= 2;"};
constexpr const std::string_view SELECT_DUP_SIZE_BY_HASH{
  "SELECT dir FROM dedup WHERE size IN (SELECT size FROM dedup WHERE hash == ? GROUP BY size HAVING count(*) >= 2);"};

} // namespace dedup::sql

#endif // DEDUPLICATOR_DEDUP_SQL_STMTS_HPP_
