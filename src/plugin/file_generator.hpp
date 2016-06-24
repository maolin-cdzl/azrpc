#pragma once

#include <string>
#include <vector>

#include "azrpc_cpp_service.hpp"

namespace azrpc {
namespace plugin {
namespace cpp {
class ServiceGenerator;
}  // namespace cpp
}  // namespace plugin
}  // namespace azrpc 

namespace google {
namespace protobuf {
class FileDescriptor;
class ServiceDescriptor;

namespace io {
class Printer;
}
}
}

namespace azrpc {
namespace plugin {
namespace cpp {

class FileGenerator {
  public:
    FileGenerator(const google::protobuf::FileDescriptor* file,
                  const std::string& dllexport_decl);

    ~FileGenerator();

    void GenerateHeader(google::protobuf::io::Printer* printer);

    void GenerateSource(google::protobuf::io::Printer* printer);

  private:
    void GenerateNamespaceOpeners(google::protobuf::io::Printer* printer);

    void GenerateNamespaceClosers(google::protobuf::io::Printer* printer);

    void GenerateBuildDescriptors(google::protobuf::io::Printer* printer);

    std::vector<std::string> package_parts_;
    std::vector<ServiceGenerator*> service_generators_;
    const ::google::protobuf::FileDescriptor* file_;
    std::string dllexport_decl_;
};

}  // namespace cpp
}  // namespace plugin
}  // namespace azrpc

