#include "fiff/input.hpp"

#include "fiff/datatypes.hpp"

namespace {

}

//==============================================================================
/**
 * Constructs a Input object.
 *
 * Prefer a constructor that also accepts a path to file.
 */
Fiff::Input::Input()
:m_relativeEndian(RelativeEndian::undetermined)
{

}

//==============================================================================
/**
 * Returns next tag in the file, and moves the read head one tag forward.
 */
Fiff::Tag Fiff::Input::getTag()
{
  Fiff::Tag tag;

  readMetaData(tag);
  readData(tag);

  return tag;
}

//==============================================================================
/**
 * Returns next tag in the file. Read head does not move.
 */
Fiff::Tag Fiff::Input::peekTag()
{
  std::streampos position = currentReadPosition();
  Fiff::Tag tag = getTag();
  goToReadPosition(position);
  return tag;
}

//==============================================================================
/**
 * Moves the read head to a position given by input parameter.
 * @param pos   Where to move the read head.
 */
void Fiff::Input::goToReadPosition(std::streampos pos)
{
  m_istream->seekg(pos);
}

//==============================================================================
/**
 * Gets the current position of the read head.
 */
std::streampos Fiff::Input::currentReadPosition() const
{
  return m_istream->tellg();
}

//==============================================================================
/**
 * Returns whether the read head is at the end of the file.
 */
bool Fiff::Input::atEnd() const
{
  return m_istream->eof();
}

//==============================================================================

Fiff::Input Fiff::Input::fromFile(const std::string &filePath)
{
  Input in;
  in.m_istream = std::make_unique<std::ifstream>(filePath, std::ios::binary);
  in.setEndianess();
  return in;
}

//==============================================================================

Fiff::Input Fiff::Input::fromFile(const std::string &filePath, Endian fileEndian)
{
  Input in;
  in.m_istream = std::make_unique<std::ifstream>(filePath, std::ios::binary);
  in.setEndianess(fileEndian);
  return in;
}

//==============================================================================
/**
 * Tries to determine the file's endianness by peeking at the next tag.
 * Call this function when read position is at start.
 *
 * This function is ideally looking for tag with id 100, which by convention
 * is at the start of a fiff file. If it does not find that when checking both
 * endian possibilities, it checks which endianness produces a tag kind in a
 * "reasonable" range, ie. not in the millions.
 */
void Fiff::Input::setEndianess()
{
  auto pos = m_istream->tellg();
  int32_t kind = 0;
  m_istream->read(reinterpret_cast<char*>(&kind), sizeof(kind));
  if(kind == 100) {
    m_relativeEndian = RelativeEndian::same_as_system;
    m_istream->seekg(pos);
    return;
  }

  int32_t swapkind = kind;
  endswap(&swapkind);
  if(swapkind == 100) {
    m_relativeEndian = RelativeEndian::different_from_system;
    m_istream->seekg(pos);
    return;
  }

  // fallback test if file does not begin with correct tag
  if(kind > 1000000 || kind < -1000000){
    m_relativeEndian = RelativeEndian::different_from_system;
  } else {
    m_relativeEndian = RelativeEndian::same_as_system;
  }
  m_istream->seekg(pos);
}

//==============================================================================
/**
 * Determines the file's endianness based on user input.
 * @param fileEndian    Endianness of the file.
 */
void Fiff::Input::setEndianess(Endian fileEndian)
{
  if (systemEndian() == fileEndian){
    m_relativeEndian = RelativeEndian::same_as_system;
  } else {
    m_relativeEndian = RelativeEndian::different_from_system;
  }
}

//==============================================================================
/**
 * Reads tag metadata and adds it to a tag, swapping endianness if needed.
 * @param tag   Tag object where the read data will be placed.
 */
void Fiff::Input::readMetaData(Fiff::Tag &tag)
{
  m_istream->read(reinterpret_cast<char*>(&tag.kind), sizeof(tag.kind));
  m_istream->read(reinterpret_cast<char*>(&tag.type), sizeof(tag.type));
  m_istream->read(reinterpret_cast<char*>(&tag.size), sizeof(tag.size));
  m_istream->read(reinterpret_cast<char*>(&tag.next), sizeof(tag.next));

  if(m_relativeEndian == RelativeEndian::different_from_system){
    endswap(&tag.kind);
    endswap(&tag.type);
    endswap(&tag.size);
    endswap(&tag.next);
  }
}

//==============================================================================
/**
 * Reads tag data and adds it to a tag, swapping endianness if needed.
 * @param tag   Tag object where the read data will be placed.
 *
 * This function assumes that the tag object has populated size and type
 * fields, used to determine how many bytes to read in and how to flip data
 * endianness if necessary.
 *
 * This function assumes the read head is at the data portion of a fiff tag
 * in the stream.
 *
 * Because some data types are structs, we need to swap bytes of each field
 * individually.
 */
void Fiff::Input::readData(Fiff::Tag &tag)
{
  //TODO: check time of switch statement vs function map vs other possible implementations
  tag.data = new char[tag.size];
  m_istream->read(reinterpret_cast<char *>(tag.data), tag.size);

  if(m_relativeEndian == RelativeEndian::different_from_system)
  {
    switch(tag.type)
    {
      case 0: //void
      {
        break;
      }
      // 1 byte
      case 1: //byte
      {
        auto *dataPtr = reinterpret_cast<int8_t*>(tag.data);
        endswap(dataPtr);
        break;
      }
      // 2 bytes
      case 2: //short
      case 7: //ushort
      case 13: //dau_pack13
      case 14: //dau_pack14
      case 16: //dau_pack16
      {
        auto *dataPtr = reinterpret_cast<int16_t*>(tag.data);
        endswap(dataPtr);
        break;
      }
      // 4 bytes
      case 3: //int32
      case 4: //float
      case 6: //julian
      case 8: //uint32
      {
        auto *dataPtr = reinterpret_cast<int32_t*>(tag.data);
        endswap(dataPtr);
        break;
      }
      // 8 bytes:
      case 5: //double
      case 9: //uint64
      case 11: //int64
      {
        auto *dataPtr = reinterpret_cast<int64_t*>(tag.data);
        endswap(dataPtr);
        break;
      }
      // array of 1 byte
      case 10: //string
      {
        auto *dataPtr = reinterpret_cast<int8_t*>(tag.data);
        for (int i = 0; i < tag.size; ++i){
          endswap(dataPtr + i);
        }
        break;
      }
      case 23: //old_pack TODO: tricky, variable length
      {
        break;
      }
      // sequential 4 bytes with added array of 1 byte
      case 30: //ch_info_rec
      {
        auto* ptr4byte = reinterpret_cast<int32_t*>(tag.data);
        for(int i = 0; i < 20; ++i){
          endswap(ptr4byte + i);
        }
        auto* ptr1byte = reinterpret_cast<int8_t*>(tag.data) + 80;
        for(int j = 0; j < 16; ++j){
          endswap(ptr1byte + j);
        }
        break;
      }
      // sequential 4 bytes
      case 31: //id_struct
      case 32: //dir_entry_struct
      case 33: //dig_point_struct
      case 34: //ch_pos_struct
      case 35: //coord_trans_struct
      {
        auto sizeInBytes = tag.size / 4;
        auto* ptr4byte = reinterpret_cast<int32_t*>(tag.data);
        for(int i = 0; i < sizeInBytes; ++i){
          endswap(ptr4byte + i);
        }
        break;
      }
    }
  }
}

