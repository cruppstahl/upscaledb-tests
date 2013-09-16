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

#include <cassert>

#include "configuration.h"
#include "datasource_numeric.h"
#include "datasource_binary.h"
#include "generator_runtime.h"

RuntimeGenerator::RuntimeGenerator(Configuration *conf)
  : Generator(conf), m_state(0), m_opcount(0), m_datasource(0), m_u01(m_rng),
    m_txn_is_active(false)
{
  if (conf->seed)
    m_rng.seed(conf->seed);

  switch (conf->key_type) {
    case Configuration::kKeyUint8:
      switch (conf->distribution) {
        case Configuration::kDistributionRandom:
          m_datasource = new NumericRandomDatasource<uint8_t>(conf->seed);
          break;
        case Configuration::kDistributionAscending:
          m_datasource = new NumericAscendingDatasource<uint8_t>();
          break;
        case Configuration::kDistributionDescending:
          m_datasource = new NumericDescendingDatasource<uint8_t>();
          break;
        case Configuration::kDistributionZipfian:
          m_datasource = new NumericZipfianDatasource<uint8_t>(conf->limit_ops,
                  conf->seed);
          break;
      }
      break;
    case Configuration::kKeyUint16:
      switch (conf->distribution) {
        case Configuration::kDistributionRandom:
          m_datasource = new NumericRandomDatasource<uint16_t>(conf->seed);
          break;
        case Configuration::kDistributionAscending:
          m_datasource = new NumericAscendingDatasource<uint16_t>();
          break;
        case Configuration::kDistributionDescending:
          m_datasource = new NumericDescendingDatasource<uint16_t>();
          break;
        case Configuration::kDistributionZipfian:
          m_datasource = new NumericZipfianDatasource<uint16_t>(conf->limit_ops,
                  conf->seed);
          break;
      }
      break;
    case Configuration::kKeyUint32:
      switch (conf->distribution) {
        case Configuration::kDistributionRandom:
          m_datasource = new NumericRandomDatasource<uint32_t>(conf->seed);
          break;
        case Configuration::kDistributionAscending:
          m_datasource = new NumericAscendingDatasource<uint32_t>();
          break;
        case Configuration::kDistributionDescending:
          m_datasource = new NumericDescendingDatasource<uint32_t>();
          break;
        case Configuration::kDistributionZipfian:
          m_datasource = new NumericZipfianDatasource<uint32_t>(conf->limit_ops,
                  conf->seed);
          break;
      }
      break;
    case Configuration::kKeyUint64:
      switch (conf->distribution) {
        case Configuration::kDistributionRandom:
          m_datasource = new NumericRandomDatasource<uint64_t>(conf->seed);
          break;
        case Configuration::kDistributionAscending:
          m_datasource = new NumericAscendingDatasource<uint64_t>();
          break;
        case Configuration::kDistributionDescending:
          m_datasource = new NumericDescendingDatasource<uint64_t>();
          break;
        case Configuration::kDistributionZipfian:
          m_datasource = new NumericZipfianDatasource<uint64_t>(conf->limit_ops,
                  conf->seed);
          break;
      }
      break;
    case Configuration::kKeyBinary:
      switch (conf->distribution) {
        case Configuration::kDistributionRandom:
          m_datasource = new BinaryRandomDatasource(conf->key_size,
                  conf->key_is_fixed_size, conf->seed);
          break;
        case Configuration::kDistributionAscending:
          m_datasource = new BinaryAscendingDatasource(conf->key_size,
                  conf->key_is_fixed_size);
          break;
        case Configuration::kDistributionDescending:
          m_datasource = new BinaryDescendingDatasource(conf->key_size,
                  conf->key_is_fixed_size);
          break;
        case Configuration::kDistributionZipfian:
          m_datasource = new BinaryZipfianDatasource(conf->key_size,
                  conf->key_is_fixed_size, conf->limit_ops,
                  conf->seed);
          break;
      }
      break;
  }
  assert(m_datasource != 0);
}

bool
RuntimeGenerator::execute(Database *db)
{
  if (m_state == kStateStopped)
    return (false);

  int cmd = get_next_command();
  switch (cmd) {
    case Generator::kCommandCreate:
      std::cout << "CREATE" << std::endl;
      break;
    case Generator::kCommandOpen:
      std::cout << "OPEN" << std::endl;
      break;
    case Generator::kCommandClose:
      std::cout << "CLOSE" << std::endl;
      break;
    case Generator::kCommandInsert:
      std::cout << "INSERT" << std::endl;
      break;
    case Generator::kCommandErase:
      std::cout << "ERASE" << std::endl;
      break;
    case Generator::kCommandFind:
      std::cout << "FIND" << std::endl;
      break;
    case Generator::kCommandBeginTransaction:
      std::cout << "TXN_BEGIN" << std::endl;
      assert(m_txn_is_active == false);
      m_txn_is_active = true;
      break;
    case Generator::kCommandAbortTransaction:
      std::cout << "TXN_ABORT" << std::endl;
      assert(m_txn_is_active == true);
      m_txn_is_active = false;
      break;
    case Generator::kCommandCommitTransaction:
      std::cout << "TXN_COMMIT" << std::endl;
      assert(m_txn_is_active == true);
      m_txn_is_active = false;
      break;
    default:
      assert(!"shouldn't be here");
  }

  m_opcount++;
  return (true);
}

void
RuntimeGenerator::open(Database *db)
{
  std::cout << "OPEN" << std::endl;
}

void
RuntimeGenerator::close(Database *db)
{
  std::cout << "CLOSE" << std::endl;
}

int
RuntimeGenerator::get_next_command()
{
  // limit reached - last command? then 'close'
  if (limit_reached()) {
    if (m_state == kStateRunning) {
      if (m_txn_is_active)
        return (Generator::kCommandCommitTransaction);
      m_state = kStateStopped;
      return (Generator::kCommandClose);
    }
  }

  // first command? then either 'create' or 'reopen', depending on flags
  if (m_opcount == 0) {
    if (m_conf->open)
      return (Generator::kCommandOpen);
    else
      return (Generator::kCommandCreate);
  }

  // begin/abort/commit transactions!
  if (m_conf->transactions_nth) {
    if (m_opcount % m_conf->transactions_nth == 0) {
      if (m_txn_is_active)
        return (Generator::kCommandCommitTransaction);
      else
        return (Generator::kCommandBeginTransaction);
    }
  }

  // perform "real" work
  if (m_conf->erase_pct || m_conf->find_pct) {
    double d = m_u01();
    if (d * 100 < m_conf->erase_pct)
      return (Generator::kCommandErase);
    if (d * 100 >= m_conf->erase_pct && d * 100 < m_conf->find_pct)
      return (Generator::kCommandFind);
  }
  return (Generator::kCommandInsert);
}

bool
RuntimeGenerator::limit_reached()
{
  // reached IOPS limit?
  if (m_opcount == m_conf->limit_ops)
    return (true);

  // reached time limit?
  if (m_conf->limit_seconds) {
    if (m_start.seconds() > m_conf->limit_seconds)
      return (true);
  }

  // check inserted bytes
  if (m_conf->limit_bytes) {
    if (m_inserted_bytes >= m_conf->limit_bytes)
      return (true);
  }

  return (false);
}
