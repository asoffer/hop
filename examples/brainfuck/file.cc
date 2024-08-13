#include "examples/brainfuck/file.h"

#include <optional>
#include <span>
#include <string>
#include <string_view>

#include "nth/debug/debug.h"
#include "nth/io/file.h"
#include "nth/io/file_path.h"

namespace bf {

std::string LoadFileContentsOrDie(std::string_view path) {
  std::optional file_path = nth::file_path::try_construct(path);
  if (not file_path) { std::abort(); }
  std::optional file = nth::file::read_only(*file_path);
  if (not file) { std::abort(); }
  std::string contents(file->size(), '\0');
  std::span<char const> data = file->read_into(contents);
  if (not file->close()) { std::abort(); }
  NTH_REQUIRE((debug), data.size() == contents.size());
  return contents;
}

}  // namespace bf
