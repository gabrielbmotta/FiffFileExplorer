//
// Created by Gabriel Motta on 3/24/22.
//

#ifndef FIFFFILEEXPLORER_OUTPUT_HPP
#define FIFFFILEEXPLORER_OUTPUT_HPP

#include "tag.hpp"
#include "../core/endian.hpp"

#include <fstream>
#include <memory>

namespace Fiff {

class Output {
public:
  Output();

  void writeTag(const Tag &tag);

  void goToWritePosition(std::streampos pos);
  std::streampos currentWritePosition();

  static Output toFile(const std::string &filePath);
  static Output toFile(const std::string &filePath, Endian fileEndian);

  RelativeEndian m_relativeEndian;
  void setEndianess(Endian endianness);
  Endian getEndianess() const;
protected:

  std::unique_ptr<std::ostream> m_ostream;
};
}//namespace

#endif //FIFFFILEEXPLORER_OUTPUT_HPP