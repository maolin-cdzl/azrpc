#pragma once

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/stubs/common.h>
#include <string>

namespace google {
namespace protobuf {
class FileDescriptor;
}  // namespace protobuf
}  // namespace google

namespace azrpc {
namespace plugin {
namespace cpp {

class LIBPROTOC_EXPORT AzRpcCppGenerator :
    public ::google::protobuf::compiler::CodeGenerator {
 public:
  AzRpcCppGenerator();
  ~AzRpcCppGenerator();

  bool Generate(
      const ::google::protobuf::FileDescriptor* file,
      const ::std::string& parameter,
      ::google::protobuf::compiler::GeneratorContext* generator_context,
      std::string* error) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(AzRpcCppGenerator);
};
}  // namespace cpp
}  // namespace plugin
}  // namespace rpcz

