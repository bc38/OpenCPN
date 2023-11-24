
 /***************************************************************************
 *   Copyright (C) 2023 Alec Leamas                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 **************************************************************************/
#ifndef OPENCPN_INCLUDE_PINCODE_H_
#define OPENCPN_INCLUDE_PINCODE_H_

#include <string>
#include <cstdint>

class Pincode {
public:
  /** Create a new pincode based on a random value. */
  static Pincode Create();

  /** Return numeric value: */
  uint64_t Get() const;

  /** Return value as string. */
  std::string ToString() const;

  /** Return a hashvalue string. */
  std::string Hash() const;

private:
  uint64_t m_value;
  Pincode(uint64_t v) { m_value = v; }
};

#endif  // OPENCPN_INCLUDE_PINCODE_H_
