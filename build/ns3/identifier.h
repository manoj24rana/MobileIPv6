#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <stdint.h>
#include <string.h>

#include <ostream>

#include "ns3/attribute-helper.h"

namespace ns3
{

class Mac48Address;

/**
 * \class Identifier
 * \brief Identifier.
 */
class Identifier
{
public:
  enum MaxSize_e {
    MAX_SIZE = 255
  };
  
  Identifier();
  Identifier(const uint8_t *buffer, uint8_t len);
  Identifier(const char *str);
  Identifier(Mac48Address addr);
  Identifier(const Identifier & identifier);
  Identifier &operator = (const Identifier &identifier);
  
  uint8_t GetLength (void) const;
  
  uint32_t CopyTo (uint8_t *buffer, uint8_t len) const;
  
  uint32_t CopyFrom (const uint8_t *buffer, uint8_t len);
  
  bool IsEmpty() const;

protected:

private:
  friend bool operator == (const Identifier &a, const Identifier &b);
  friend bool operator != (const Identifier &a, const Identifier &b);
  friend std::ostream& operator<< (std::ostream& os, const Identifier & identifier);
  
  uint8_t m_len;
  uint8_t m_identifier[MAX_SIZE];
};

ATTRIBUTE_HELPER_HEADER (Identifier);

bool operator == (const Identifier &a, const Identifier &b);
bool operator != (const Identifier &a, const Identifier &b);
std::ostream& operator<< (std::ostream& os, const Identifier & identifier);

/**
 * \class IdentifierHash
 * \brief Hash function class for Identifier.
 */
class IdentifierHash : public std::unary_function<Identifier, size_t>
{
public:
  /**
   * \brief Unary operator to hash Identifier.
   * \param x Identifier to hash
   */
  size_t operator () (Identifier const &x) const;
};

} /* namespace ns3 */

#endif /* MOBILITY_HEADER_H */
