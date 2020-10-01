#include "include/flutter_secure_storage_windows/flutter_secure_storage_windows_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

using namespace flutter;
#include <iostream>
#include <fstream>
using namespace std;
#include <filesystem>
#include <shlobj_core.h>

namespace {

class FlutterSecureStorageWindowsPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  FlutterSecureStorageWindowsPlugin();

  virtual ~FlutterSecureStorageWindowsPlugin();

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

// static
void FlutterSecureStorageWindowsPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "plugins.it_nomads.com/flutter_secure_storage",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<FlutterSecureStorageWindowsPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

FlutterSecureStorageWindowsPlugin::FlutterSecureStorageWindowsPlugin() {}

FlutterSecureStorageWindowsPlugin::~FlutterSecureStorageWindowsPlugin() {}

std::filesystem::path getSettingsPath() {
    std::filesystem::path path;
    PWSTR path_tmp;

    auto get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);

    if (get_folder_path_ret != S_OK) {
        CoTaskMemFree(path_tmp);
        return path;
    }

    // Convert the Windows path type to a C++ path
    path = path_tmp;
    CoTaskMemFree(path_tmp);

    path /= "Test"; // Should be replaced
    path /= "settings";

    return path;
}

void FlutterSecureStorageWindowsPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  // Replace "getPlatformVersion" check with your plugin's method.
  // See:
  // https://github.com/flutter/engine/tree/master/shell/platform/common/cpp/client_wrapper/include/flutter
  // and
  // https://github.com/flutter/engine/tree/master/shell/platform/glfw/client_wrapper/include/flutter
  // for the relevant Flutter APIs.
  if (method_call.method_name().compare("write") == 0) {
      const auto* arguments = std::get_if<EncodableMap>(method_call.arguments());
      auto kind_iter = arguments->find(EncodableValue(std::string("key")));
      if (kind_iter == arguments->end()) {
          result->Error("Argument error", "The required key 'key' is not set in the map.");
          return;
      }
      const auto& key = std::get<std::string>(kind_iter->second);
      kind_iter = arguments->find(EncodableValue(std::string("value")));
      if (kind_iter == arguments->end()) {
          result->Error("Argument error", "The required key 'value' is not set in the map.");
          return;
      }
      const auto& value = std::get<std::string>(kind_iter->second);

      /*
      DATA_BLOB DataIn;
      DATA_BLOB DataOut;
      BYTE* pbDataInput = (BYTE*)value.c_str();
      DWORD cbDataInput = (DWORD)strlen((char*)pbDataInput) + 1;

      DataIn.pbData = pbDataInput;
      DataIn.cbData = cbDataInput;

      CRYPTPROTECT_PROMPTSTRUCT PromptStruct;
      ZeroMemory(&PromptStruct, sizeof(PromptStruct));
      PromptStruct.cbSize = sizeof(PromptStruct);
      PromptStruct.dwPromptFlags = CRYPTPROTECT_PROMPT_ON_PROTECT;
      PromptStruct.szPrompt = L"This is a user prompt.";

      if (!CryptProtectData(
          &DataIn,
          L"This is the description string.", // A description string to be included with the encrypted data. 
          NULL,                               // Optional entropy not used.
          NULL,                               // Reserved.
          &PromptStruct,                               // Pass NULL for the prompt structure.
          0,
          &DataOut)) {
          LPSTR messageBuffer = nullptr;
          size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
              NULL, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

          std::string message(messageBuffer, size);

          //Free the buffer.
          //LocalFree(messageBuffer);
          result->Error("Encryption error using CryptProtectData", messageBuffer);
          return;
      }*/

      std::filesystem::path path = getSettingsPath();
      if (path.empty()) {
          result->Error("Failed to resolve settings directory.");
          return;
      }

      std::filesystem::create_directories(path);

      ofstream myfile(path / key);
      if (myfile.is_open()) {
          myfile << /*DataOut.pbData*/value << endl;
          myfile.close();
          result->Success();
          return;
      }

      result->Error("Write failed.");
  } else if (method_call.method_name().compare("read") == 0) {
      const auto* arguments = std::get_if<EncodableMap>(method_call.arguments());
      auto kind_iter = arguments->find(EncodableValue(std::string("key")));
      if (kind_iter == arguments->end()) {
          result->Error("Argument error", "The required key 'key' is not set in the map.");
          return;
      }
      const auto& key = std::get<std::string>(kind_iter->second);

      std::filesystem::path path = getSettingsPath();
      if (path.empty()) {
          result->Error("Failed to resolve settings directory.");
          return;
      }

      ifstream myfile(path / key);
      if (myfile.is_open()) {
          string line;
          getline(myfile, line);
          myfile.close();

          /*
          DATA_BLOB encryptedData;
          DATA_BLOB decryptedData;
          BYTE* pbDataInput = (BYTE*)line.c_str();
          DWORD cbDataInput = (DWORD)strlen((char*)pbDataInput) + 1;
          encryptedData.pbData = pbDataInput;
          encryptedData.cbData = cbDataInput;
          LPWSTR pDescrOut = NULL;

          if (CryptUnprotectData(
              &encryptedData,
              &pDescrOut,
              NULL,        // Optional entropy
              NULL,        // Reserved
              NULL,        // Optional PromptStruct
              0,
              &decryptedData)) {
              // TODO optional check if pDescrOut is equal to key. it should be.
              LocalFree(pDescrOut);
              result->Success(flutter::EncodableValue(decryptedData.pbData));
              // FIXME what is with decryptedData.pbData where are those data freed?
          } else {
              result->Error("Decryption error");
          }/*/
          result->Success(flutter::EncodableValue(line));
          //*/
      }

      result->Success();
  } else if (method_call.method_name().compare("delete") == 0) {
      const auto* arguments = std::get_if<EncodableMap>(method_call.arguments());
      auto kind_iter = arguments->find(EncodableValue(std::string("key")));
      if (kind_iter == arguments->end()) {
          result->Error("Argument error", "The required key 'key' is not set in the map.");
          return;
      }
      const auto& key = std::get<std::string>(kind_iter->second);

      std::filesystem::path path = getSettingsPath();
      if (path.empty()) {
          result->Error("Failed to resolve settings directory.");
          return;
      }

      std::filesystem::remove(path / key);
      result->Success();
  } else if (method_call.method_name().compare("readAll") == 0) {
  } else if (method_call.method_name().compare("deleteAll") == 0) {
      std::filesystem::path path = getSettingsPath();
      if (path.empty()) {
          result->Error("Failed to resolve settings directory.");
          return;
      }

      std::filesystem::remove_all(path);
      result->Success();
  } else if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else {
    result->NotImplemented();
  }
}

}  // namespace

void FlutterSecureStorageWindowsPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  FlutterSecureStorageWindowsPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
