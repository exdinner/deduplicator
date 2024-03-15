#ifndef DEDUPLICATOR_CONTEXT_HPP_
#define DEDUPLICATOR_CONTEXT_HPP_

#include "sqlitemm/db.hpp"

namespace dedup {

class Context {
public:
  Context() = default;
  Context(const Context&) = default;
  Context(Context&&) noexcept = default;
  Context& operator=(const Context&) = default;
  Context& operator=(Context&&) noexcept = default;

  virtual ~Context() = default;

protected:
  /* NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) */
  static sqlitemm::DB db;
};

} // namespace dedup

#endif // DEDUPLICATOR_CONTEXT_HPP_
