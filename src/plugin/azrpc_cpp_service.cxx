#include "azrpc_cpp_service.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include "strutil.hpp"
#include "cpp_helpers.hpp"

namespace azrpc {
namespace plugin {
namespace cpp {

using namespace google::protobuf;
using namespace google::protobuf::compiler::cpp;

ServiceGenerator::ServiceGenerator(const ServiceDescriptor* descriptor,
                                   const string& dllexport_decl)
  : descriptor_(descriptor) {
  vars_["classname"] = descriptor_->name();
  vars_["full_name"] = descriptor_->full_name();
  if (dllexport_decl.empty()) {
    vars_["dllexport"] = "";
  } else {
    vars_["dllexport"] = dllexport_decl + " ";
  }
}

ServiceGenerator::~ServiceGenerator() {}

void ServiceGenerator::GenerateDeclarations(io::Printer* printer) {
  // Forward-declare the stub type.
  printer->Print(vars_,
    "class $classname$_Stub;\n"
    "\n");

  GenerateInterface(printer);
  GenerateStubDefinition(printer);
}

void ServiceGenerator::GenerateInterface(io::Printer* printer) {
  printer->Print(vars_,
    "class $dllexport$$classname$ : public azrpc::IService {\n"
    " protected:\n"
    "  // This class should be treated as an abstract interface.\n"
    "  inline $classname$() {};\n"
    " public:\n"
    "  virtual ~$classname$();\n");
  printer->Indent();

  printer->Print(vars_,
    "\n"
    "typedef $classname$_Stub Stub;\n"
    "\n"
    "static const ::google::protobuf::ServiceDescriptor* descriptor();\n"
    "\n");

  GenerateMethodSignatures(VIRTUAL, printer, false);

  printer->Print(
    "\n"
    "// implements Service ----------------------------------------------\n"
    "\n"
    "const ::google::protobuf::ServiceDescriptor* GetDescriptor();\n"
    "void callMethod(const ::google::protobuf::MethodDescriptor* method,\n"
    "                const std::shared_ptr<google::protobuf::Message>& request,\n"
    "                const std::shared_ptr<azrpc::Reply>& reply);\n");

  printer->Outdent();
  printer->Print(vars_,
    "\n"
    " private:\n"
    "  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS($classname$);\n"
    "};\n"
    "\n");
}

void ServiceGenerator::GenerateStubDefinition(io::Printer* printer) {
  printer->Print(vars_,
    "class $dllexport$$classname$_Stub {\n"
    " public:\n");

  printer->Indent();

  printer->Print(vars_,
    "$classname$_Stub(const std::shared_ptr<azrpc::IClientChannel>& channel); \n"
    "$classname$_Stub(const std::shared_ptr<azrpc::IClientChannel>& channel, \n"
    "                 const std::string& service_name); \n"
    "~$classname$_Stub();\n"
    "\n"
    "inline std::shared_ptr<azrpc::IClientChannel>& channel() { return channel_; }\n"
    "\n"
    "\n");

  GenerateMethodSignatures(NON_VIRTUAL, printer, true);

  printer->Outdent();
  printer->Print(vars_,
    " private:\n"
    "  std::shared_ptr<azrpc::IClientChannel> channel_;\n"
    "  std::string service_name_;\n"
    "  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS($classname$_Stub);\n"
    "};\n"
    "\n");
}

void ServiceGenerator::GenerateMethodSignatures(
    VirtualOrNon virtual_or_non, io::Printer* printer, bool stub) {
  for (int i = 0; i < descriptor_->method_count(); i++) {
    const MethodDescriptor* method = descriptor_->method(i);
    map<string, string> sub_vars;
    sub_vars["name"] = method->name();
    sub_vars["input_type"] = ClassName(method->input_type(), true);
    sub_vars["output_type"] = ClassName(method->output_type(), true);
    sub_vars["virtual"] = virtual_or_non == VIRTUAL ? "virtual " : "";

    if (stub) {
      printer->Print(sub_vars,
                     "$virtual$void $name$(const $input_type$* argument,\n"
                     "                     const std::shared_ptr<azrpc::IAzRpcCallback>& callback,\n"
                     "                     int timeout = 0);\n");
    } else {
      printer->Print(
          sub_vars,
          "$virtual$void $name$(const std::shared_ptr<google::protobuf::Message>& argument,\n"
          "                     const std::shared_ptr<azrpc::Reply>& reply);\n");
    }
  }
}

// ===================================================================

void ServiceGenerator::GenerateDescriptorInitializer(
    io::Printer* printer, int index) {
  map<string, string> vars;
  vars["classname"] = descriptor_->name();
  vars["index"] = SimpleItoa(index);

  printer->Print(vars,
    "$classname$_descriptor_ = file->service($index$);\n");
}

// ===================================================================

void ServiceGenerator::GenerateImplementation(io::Printer* printer) {
  printer->Print(vars_,
    "$classname$::~$classname$() {}\n"
    "\n"
    "const ::google::protobuf::ServiceDescriptor* $classname$::descriptor() {\n"
    "  protobuf_AssignDescriptorsOnce();\n"
    "  return $classname$_descriptor_;\n"
    "}\n"
    "\n"
    "const ::google::protobuf::ServiceDescriptor* $classname$::GetDescriptor() {\n"
    "  protobuf_AssignDescriptorsOnce();\n"
    "  return $classname$_descriptor_;\n"
    "}\n"
    "\n");

  // Generate methods of the interface.
  GenerateNotImplementedMethods(printer);
  GenerateCallMethod(printer);

  // Generate stub implementation.
  printer->Print(vars_,
    "$classname$_Stub::$classname$_Stub(const std::shared_ptr<azrpc::IClientChannel>& channel,\n"
    "                                   const ::std::string& service_name)\n"
    "  : channel_(channel), service_name_(service_name)\n"
    "{}\n"
    "$classname$_Stub::$classname$_Stub(const std::shared_ptr<azrpc::IClientChannel>& channel)\n"
    "  : channel_(channel), service_name_($classname$::descriptor()->name())\n"
    "{}\n"
    "$classname$_Stub::~$classname$_Stub() {\n"
    "}\n"
    "\n");

  GenerateStubMethods(printer);
}

void ServiceGenerator::GenerateNotImplementedMethods(io::Printer* printer) {
  for (int i = 0; i < descriptor_->method_count(); i++) {
    const MethodDescriptor* method = descriptor_->method(i);
    map<string, string> sub_vars;
    sub_vars["classname"] = descriptor_->name();
    sub_vars["name"] = method->name();
    sub_vars["index"] = SimpleItoa(i);
    sub_vars["input_type"] = ClassName(method->input_type(), true);
    sub_vars["output_type"] = ClassName(method->output_type(), true);

    printer->Print(sub_vars,
      "void $classname$::$name$(const std::shared_ptr<google::protobuf::Message>& argument,\n"
      "                         const std::shared_ptr<azrpc::Reply>& reply) {\n"
      "  reply->sendError(azrpc::NO_IMPLEMENTED,\n"
      "              \"Method $name$() not implemented.\");\n"
      "}\n"
      "\n");
  }
}

void ServiceGenerator::GenerateCallMethod(io::Printer* printer) {
  printer->Print(vars_,
    "void $classname$::callMethod(const ::google::protobuf::MethodDescriptor* method,\n"
    "                             const std::shared_ptr<google::protobuf::Message>& argument,\n"
    "                             const std::shared_ptr<azrpc::Reply>& reply) {\n"
    "  GOOGLE_DCHECK_EQ(method->service(), $classname$_descriptor_);\n"
    "  switch(method->index()) {\n");

  for (int i = 0; i < descriptor_->method_count(); i++) {
    const MethodDescriptor* method = descriptor_->method(i);
    map<string, string> sub_vars;
    sub_vars["name"] = method->name();
    sub_vars["index"] = SimpleItoa(i);
    sub_vars["input_type"] = ClassName(method->input_type(), true);
    sub_vars["output_type"] = ClassName(method->output_type(), true);

    // Note:  down_cast does not work here because it only works on pointers,
    //   not references.
    printer->Print(sub_vars,
      "    case $index$:\n"
      "      $name$(\n"
      "          std::static_pointer_cast<$input_type$>(argument),\n"
      "          reply);\n"
      "      break;\n");
  }

  printer->Print(vars_,
    "    default:\n"
    "      GOOGLE_LOG(FATAL) << \"Bad method index; this should never happen.\";\n"
    "      break;\n"
    "  }\n"
    "}\n"
    "\n");
}

void ServiceGenerator::GenerateStubMethods(io::Printer* printer) {
  for (int i = 0; i < descriptor_->method_count(); i++) {
    const MethodDescriptor* method = descriptor_->method(i);
    map<string, string> sub_vars;
    sub_vars["classname"] = descriptor_->name();
    sub_vars["name"] = method->name();
    sub_vars["index"] = SimpleItoa(i);
    sub_vars["input_type"] = ClassName(method->input_type(), true);
    sub_vars["output_type"] = ClassName(method->output_type(), true);

    printer->Print(sub_vars,
      "void $classname$_Stub::$name$(const $input_type$* argument,\n"
      "                              const std::shared_ptr<azrpc::IAzRpcCallback>& callback,\n"
      "                              int timeout) {\n"
      "  channel_->callMethod(\n"
      "                        $classname$::descriptor()->method($index$),\n"
      "                        argument, callback, timeout);\n"
      "}\n");
  }
}

}  // namespace cpp
}  // namespace plugin
}  // namespace azrpc

