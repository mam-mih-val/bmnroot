#include "BmnOnlineSink.h"

#include <fairmq/runDevice.h>

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description &options) {
    // clang-format off
    options.add_options()
        ("verbose", bpo::value<Int_t>()->default_value(5), "Verbosity level")
        ("input-channel", bpo::value<std::string>()->default_value("bmn-online-data-2"), "Input channel name");
    // clang-format on
}

std::unique_ptr<fair::mq::Device> getDevice(fair::mq::ProgOptions &) { return std::make_unique<BmnOnlineSink>(); }
