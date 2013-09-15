/**
 * Copyright (C) 2005-2013 Christoph Rupp (chris@crupp.de).
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * See files COPYING.* for License information.
 */


#ifndef DATASOURCE_BINARY_H__
#define DATASOURCE_BINARY_H__

#include <limits>
#include <boost/random.hpp>
#include <boost/random/uniform_01.hpp>

//
// abstract base class for a data source - generates test data
//
class BinaryRandomDatasource : public Datasource
{
  public:
    BinaryRandomDatasource(int size, bool fixed_size, unsigned int seed = 0)
      : m_size(size), m_fixed_size(fixed_size) {
      if (seed)
        m_rng.seed(seed);
      for (int i = 0; i < sizeof(m_data); i++)
        m_data[i] = (unsigned char)i;
    }

    // returns the next piece of data
    virtual void get_next(std::vector<unsigned char> &vec) {
      int size = m_size;
      if (m_fixed_size == false)
        size = (m_rng() % m_size) + 1;
      vec.resize(size);

      for (int i = 0; i < size; i++)
        vec[i] = m_data[m_rng() % sizeof(m_data)];
    }

  private:
    boost::mt19937 m_rng;
    unsigned char m_data[256];
    int m_size;
    bool m_fixed_size;
};

class BinaryAscendingDatasource : public Datasource
{
  public:
    BinaryAscendingDatasource(int size, bool fixed_size)
      : m_size(size), m_fixed_size(fixed_size) {
      if (fixed_size) {
        m_data.resize(size);
        for (int i = 0; i < size; i++)
          m_data[i] = 0;
      }
      else {
        m_data.resize(1);
        m_data[0] = 0;
      }
    }

    // returns the next piece of data; overflows are ignored
    virtual void get_next(std::vector<unsigned char> &vec) {
      vec = m_data;

      size_t size = m_data.size();
      if (m_fixed_size || m_data.size() == m_size) {
        for (size_t s = size - 1; s >= 0; s--) {
          m_data[s]++;
          // if we have an overflow: continue with the next digit
          if (m_data[s] != 0)
            break;
        }
        // no fixed size? cut off trailing zeroes
        if (!m_fixed_size) {
          for (size_t s = size - 1; s >= 0; s--) {
            if (!m_data[s])
              m_data.resize(m_data.size() - 1);
            else
              break;
          }
        }
      }
      else {
        if (m_data.size() < m_size) {
          m_data.resize(m_data.size() + 1);
          m_data[m_data.size() - 1] = 0;
        }
      }

      vec = m_data;
    }

  private:
    unsigned char m_value;
    int m_size;
    std::vector<unsigned char> m_data;
    bool m_fixed_size;
};

class BinaryDescendingDatasource : public Datasource
{
  public:
    BinaryDescendingDatasource(int size, bool fixed_size)
      : m_size(size), m_fixed_size(fixed_size) {
      if (fixed_size) {
        m_data.resize(size);
        for (int i = 0; i < size; i++)
          m_data[i] = 0xff;
      }
      else {
        m_data.resize(1);
        m_data[0] = 0xff;
      }
    }

    // returns the next piece of data; overflows are ignored
    virtual void get_next(std::vector<unsigned char> &vec) {
      vec = m_data;

      size_t size = m_data.size();
      if (m_fixed_size || m_data.size() == m_size) {
        for (size_t s = size - 1; s >= 0; s--) {
          m_data[s]--;
          // if we have an overflow: continue with the next digit
          if (m_data[s] != 0xff)
            break;
        }
        // no fixed size? cut off trailing zeroes
        if (!m_fixed_size) {
          for (size_t s = size - 1; s >= 0; s--) {
            if (m_data[s] == 0xff)
              m_data.resize(m_data.size() - 1);
            else
              break;
          }
        }
      }
      else {
        if (m_data.size() < m_size) {
          m_data.resize(m_data.size() + 1);
          m_data[m_data.size() - 1] = 0xff;
        }
      }

      vec = m_data;
    }

  private:
    unsigned char m_value;
    int m_size;
    std::vector<unsigned char> m_data;
    bool m_fixed_size;
};

// Zipfian distribution is based on
// http://www.cse.usf.edu/~christen/tools/toolpage.html
class BinaryZipfianDatasource : public Datasource
{
  // vorberechnen eines datenstroms, der groß genug ist um daraus die
  // ganzen werte abzuleiten (N * size)
  // dann eine NumericZipfianDatasource erzeugen und in diesem binary
  // array entsprechend die daten rauskopieren
  public:
    BinaryZipfianDatasource(unsigned n, int size, bool fixed_size,
            long seed = 0, double alpha = 0.8)
      : m_size(size), m_fixed_size(fixed_size), m_zipf(n, seed, alpha) {
      if (seed)
        m_rng.seed(seed);
      m_data.resize(n * size);
      for (unsigned i = 0; i < (n * size); i++)
        m_data[i] = m_rng() % 0xff;
    }

    // returns the next piece of data
    virtual void get_next(std::vector<unsigned char> &vec) {
      int size = m_size;
      if (!m_fixed_size)
        size = (m_rng() % m_size) + 1;

      vec.resize(size);

      int pos = m_zipf.get_next_number(); 
      for (int i = 0; i < size; i++)
        vec[i] = m_data[pos + i];
    }

  private:
    boost::mt19937 m_rng;
    int m_size;
    bool m_fixed_size;
    NumericZipfianDatasource<int> m_zipf;
    std::vector<unsigned char> m_data;
};

#endif /* DATASOURCE_BINARY_H__ */

