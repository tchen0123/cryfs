#include "CryConfigLoader.h"
#include "CryConfigFile.h"
#include <boost/filesystem.hpp>
#include <messmer/cpp-utils/random/Random.h>
#include <messmer/cpp-utils/logging/logging.h>

namespace bf = boost::filesystem;
using cpputils::unique_ref;
using cpputils::make_unique_ref;
using cpputils::Console;
using cpputils::IOStreamConsole;
using cpputils::Random;
using cpputils::RandomGenerator;
using boost::optional;
using boost::none;
using std::vector;
using std::string;
using std::function;
using namespace cpputils::logging;

namespace cryfs {

CryConfigLoader::CryConfigLoader(unique_ref<Console> console, RandomGenerator &keyGenerator, function<string()> askPassword)
    : _creator(std::move(console), keyGenerator), _askPassword(askPassword) {
}

optional<CryConfigFile> CryConfigLoader::_loadConfig(const bf::path &filename) {
  string password = _askPassword();
  auto config = CryConfigFile::load(filename, password);
  if (config == none) {
    LOG(ERROR) << "Could not load config file. Wrong password?";
    return none;
  }
  return std::move(*config);
}

}