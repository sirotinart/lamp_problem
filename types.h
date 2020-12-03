#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#include <QByteArray>
#include <QColor>

namespace Ivideon {

#pragma pack(push, 1)

struct CommandHeader {
  uint8_t type;
  uint16_t length;
};

#pragma pack(pop)

enum CommandType {
  ON = 0x12,
  OFF = 0x13,
  COLOR = 0x20
};

struct Command {
  CommandHeader header;
  QByteArray data;
};

}

#endif // TYPES_H
