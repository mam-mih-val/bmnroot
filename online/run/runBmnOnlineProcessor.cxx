#include "BmnOnlineProcessor.h"

#include <fairmq/runDevice.h>

namespace bpo = boost::program_options;

void addCustomOptions(bpo::options_description &options) {
    // clang-format off
    options.add_options()
        ("verbose", bpo::value<Int_t>()->default_value(10), "Verbosity level")
        ("input-channel", bpo::value<std::string>()->default_value("bmn-online-data-1"), "Input channel name")
        ("output-channel", bpo::value<std::string>()->default_value("bmn-online-data-2"), "Output channel name");
    // clang-format on
}

std::unique_ptr<fair::mq::Device> getDevice(fair::mq::ProgOptions &) { return std::make_unique<BmnOnlineProcessor>(); }
