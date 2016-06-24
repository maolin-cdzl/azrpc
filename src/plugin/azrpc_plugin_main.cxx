#include <google/protobuf/compiler/plugin.h>
#include "azrpc_cpp_generator.hpp"

int main(int argc, char* argv[]) {
  azrpc::plugin::cpp::AzRpcCppGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}

