#include "jasmin/serialization.h"

namespace jasmin::internal {

char* CopyRawInstructions(std::span<Value const> raw_instructions,
                          std::string& output) {
  output.reserve(output.size() + raw_instructions.size() * ValueSize);
  char* p = output.data() + output.size();

  for (Value const& value : raw_instructions) {
    char buffer[ValueSize];
    Value::Store(value, buffer, ValueSize);
    output.append(std::begin(buffer), std::end(buffer));
  }
  return p;
}

}  // namespace jasmin::internal
