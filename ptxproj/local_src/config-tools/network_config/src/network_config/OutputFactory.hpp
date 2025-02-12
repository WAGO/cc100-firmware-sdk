// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <memory>
#include "Output.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace network_config {

class OutputFactory {
 public:
  OutputFactory() = default;
  virtual ~OutputFactory() = default;

  OutputFactory(const OutputFactory&) = delete;
  OutputFactory& operator=(const OutputFactory&) = delete;
  OutputFactory(OutputFactory&& other) = delete;
  OutputFactory& operator=(OutputFactory&&) = delete;

  static ::std::unique_ptr<Output> Create(const po::variables_map &vm);
};

}
