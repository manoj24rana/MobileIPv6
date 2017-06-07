#include "ns3/assert.h"
#include "ns3/mac48-address.h"

#include <iomanip>

#include "identifier.h"

namespace ns3 {

Identifier::Identifier()
  : m_len(0)
{
  memset (m_identifier, 0, MAX_SIZE);
}

Identifier::Identifier(const uint8_t *identifier, uint8_t len)
  : m_len(len)
{
  memcpy(m_identifier, identifier, m_len);
}

Identifier::Identifier(const char *str)
{
  m_len = strlen(str);
  memcpy(m_identifier, str, m_len);
}

Identifier::Identifier(Mac48Address addr)
 : m_len(6)
{
  addr.CopyTo(m_identifier);
}

Identifier::Identifier(const Identifier &identifier)
  : m_len(identifier.m_len)
{
  memcpy(m_identifier, identifier.m_identifier, m_len);
}

Identifier &
Identifier::operator = (const Identifier &identifier)
{
  m_len = identifier.m_len;
  memcpy(m_identifier, identifier.m_identifier, m_len);
  return *this;
}

uint8_t
Identifier::GetLength (void) const
{
  return m_len;
}

uint32_t
Identifier::CopyTo (uint8_t *buffer, uint8_t len) const
{
  NS_ASSERT (len >= m_len);
  
  memcpy (buffer, m_identifier, m_len);
  return m_len;
}

uint32_t
Identifier::CopyFrom (const uint8_t *buffer, uint8_t len)
{
  memcpy (m_identifier, buffer, len);
  m_len = len;
  
  return m_len;
}

bool Identifier::IsEmpty () const
{
  return (m_len == 0);
}

bool operator == (const Identifier &a, const Identifier &b)
{
  if(a.m_len != b.m_len)
    {
	  return false;
	}
  return memcmp (a.m_identifier, b.m_identifier, a.m_len) == 0;
}

bool operator != (const Identifier &a, const Identifier &b)
{
  return !(a == b);
}

std::ostream& operator<< (std::ostream& os, const Identifier & identifier)
{
  os.setf (std::ios::hex, std::ios::basefield);
  os.fill('0');
  for (uint8_t i = 0; i < (identifier.m_len-1); ++i)
    {
	  os << std::setw(2) << (uint32_t)identifier.m_identifier[i] << ":";
	}
  os << std::setw(2) << (uint32_t)identifier.m_identifier[identifier.m_len-1];
  os.setf (std::ios::dec, std::ios::basefield);
  os.fill(' ');
  return os;
}


#ifdef __cplusplus
extern "C"
{ /* } */
#endif

/**
 * \brief Get a hash key.
 * \param k the key
 * \param length the length of the key
 * \param level the previous hash, or an arbitrary value
 * \return hash
 * \note Adapted from Jens Jakobsen implementation (chillispot).
 */
static uint32_t lookuphash (unsigned char* k, uint32_t length, uint32_t level)
{
#define mix(a, b, c) \
  ({ \
   (a) -= (b); (a) -= (c); (a) ^= ((c) >> 13); \
   (b) -= (c); (b) -= (a); (b) ^= ((a) << 8);  \
   (c) -= (a); (c) -= (b); (c) ^= ((b) >> 13); \
   (a) -= (b); (a) -= (c); (a) ^= ((c) >> 12); \
   (b) -= (c); (b) -= (a); (b) ^= ((a) << 16); \
   (c) -= (a); (c) -= (b); (c) ^= ((b) >> 5);  \
   (a) -= (b); (a) -= (c); (a) ^= ((c) >> 3);  \
   (b) -= (c); (b) -= (a); (b) ^= ((a) << 10); \
   (c) -= (a); (c) -= (b); (c) ^= ((b) >> 15); \
   })

  typedef uint32_t  ub4;   /* unsigned 4-byte quantities */
  //typedef unsigned  char ub1;   /* unsigned 1-byte quantities */
  uint32_t a = 0;
  uint32_t b = 0;
  uint32_t c = 0;
  uint32_t len = 0;

  /* Set up the internal state */
  len = length;
  a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
  c = level;           /* the previous hash value */

  /* handle most of the key */
  while (len >= 12)
    {
      a += (k[0] + ((ub4)k[1] << 8) + ((ub4)k[2] << 16) + ((ub4)k[3] << 24));
      b += (k[4] + ((ub4)k[5] << 8) + ((ub4)k[6] << 16) + ((ub4)k[7] << 24));
      c += (k[8] + ((ub4)k[9] << 8) + ((ub4)k[10] << 16) + ((ub4)k[11] << 24));
      mix (a, b, c);
      k += 12; 
      len -= 12;
    }

  /* handle the last 11 bytes */
  c += length;
  switch (len) /* all the case statements fall through */
    {
    case 11: c += ((ub4)k[10] << 24);
    case 10: c += ((ub4)k[9] << 16);
    case 9 : c += ((ub4)k[8] << 8); /* the first byte of c is reserved for the length */
    case 8 : b += ((ub4)k[7] << 24);
    case 7 : b += ((ub4)k[6] << 16);
    case 6 : b += ((ub4)k[5] << 8);
    case 5 : b += k[4];
    case 4 : a += ((ub4)k[3] << 24);
    case 3 : a += ((ub4)k[2] << 16);
    case 2 : a += ((ub4)k[1] << 8);
    case 1 : a += k[0];
             /* case 0: nothing left to add */
    }
  mix (a, b, c);

#undef mix

  /* report the result */
  return c;
}

#ifdef __cplusplus
}
#endif

size_t IdentifierHash::operator () (Identifier const &x) const
{
  uint8_t buf[Identifier::MAX_SIZE];

  x.CopyTo (buf, Identifier::MAX_SIZE);

  return lookuphash (buf, x.GetLength(), 0);
}

} /* namespace ns3 */
