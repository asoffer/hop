#ifndef EXAMPLES_BRAINFUCK_FILE_H
#define EXAMPLES_BRAINFUCK_FILE_H

#include <string>
#include <string_view>

namespace bf {

std::string LoadFileContentsOrDie(std::string_view path);

}  // namespace bf

#endif // EXAMPLES_BRAINFUCK_FILE_H
