// context-path.cpp
// clang, ISO C++20
// adds option "add current folder to PATH" to context menu

#include <windows.h>

#include <codecvt>
#include <iostream>
#include <string_view>

using std::cin;
using std::cout;
using std::endl;

// linker -> manifest file -> UAC Execution level -> administrator could be used
// to force running as admin but this would run every time we use it from the
// context menu as well note that it won't add the context menu entry unless exe
// is run as administrator
void logPause(std::string_view msg) {
  cout << msg << endl;
  system("pause > nul");
}

// https://docs.microsoft.com/en-us/windows/win32/api/winreg/

bool createKey(HKEY&& hkey, LPCWSTR&& lpSubKey, HKEY& phkResult) {
  return RegCreateKeyW(hkey, lpSubKey, &phkResult) == ERROR_SUCCESS;
};

bool openKey(HKEY&& hkey, LPCWSTR&& lpSubKey, REGSAM&& samDesired,
             HKEY& phkResult) {
  return RegOpenKeyEx(hkey, lpSubKey, 0, samDesired, &phkResult) ==
         ERROR_SUCCESS;
}

bool setValue(HKEY&& hkey, const LPCSTR&& lpSubKey, DWORD&& dwType,
              LPSTR lpData) {
  return RegSetValueA(hkey, lpSubKey, dwType, lpData, sizeof(lpData)) ==
         ERROR_SUCCESS;
}

/*
using compatibility version right now
TODO don't use compatibility version (open subKey for reading and use
RegSetKeyValueW)

bool setValue(HKEY&& hkey, LPCWSTR dir, DWORD dwType, const std::string& data) {
return RegSetValueExW(hkey, dir, 0, dwType, (const BYTE*)data.c_str(),
data.size() + 1) == ERROR_SUCCESS;
}
*/

HKEY hkey;
std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

int main(int argc, char* argv[]) {
  // indexing argv[1] when it doesn't exist causes an access violation error
  if (argc == 2) {
    std::string path = "Environment";
    openKey(HKEY_CURRENT_USER, L"Environment", KEY_ALL_ACCESS, hkey);

    /*   If hKey specifies HKEY_PERFORMANCE_DATA and the pvData buffer
           is not large enough to contain all of the returned data,
           the function returns ERROR_MORE_DATA and
               the value returned through the pcbData parameter is undefined
                   .This is because the size of the performance data can
       change from one call to the next.In this case, you must increase the
       buffer size and call RegGetValue again passing the updated buffer size
       in the pcbData parameter.Repeat this until the function succeeds.You
       need to maintain a separate variable to keep track of the buffer size,
           because the value returned by pcbData is unpredictable.*/

    // yeah but 50 K should be enough I don't care if I'm allocating too
    // much, I can't be bothered to do all the above and if you can, you
    // should ⬇️⬇️⬇️

    char val[50000];
    DWORD dataSize = sizeof(val);
    if (ERROR_SUCCESS == RegGetValueA(HKEY_CURRENT_USER, "Environment", "Path",
                                      RRF_RT_ANY, nullptr, &val, &dataSize)) {
      std::string valStr = val;
      if (valStr.find(argv[1]) == std::string::npos) {
        valStr += argv[1] + std::string(";");
        std::cout << "\n\nnew path: " << valStr << endl;
        // make it set the value of a certain lpValue, (not lpKey!)
        // you can specify a specific key with a handle obtained from
        // RegOpenKeyEx() which takes a pointer to a variable that receives a
        // handle to the opened key
        LSTATUS RegSetValueExASuccess =
            RegSetValueExA(hkey, "Path", 0, REG_SZ, (const BYTE*)valStr.c_str(),
                           valStr.size() + 1) == ERROR_SUCCESS;

        RegCloseKey(HKEY_CURRENT_USER);
        if (!RegSetValueExASuccess) {
          logPause("could not setValue");
        }
      }

    } else {
      cout << GetLastError();
      cout << "could not getValue";
    };

    return 0;
  }

  cout << "make sure you're running this with administrative priveleges or it "
          "WILL NOT WORK"
       << endl;

  std::string data = std::move(argv[0]);
  data.append(" \"%1\"");

  std::string dirPath = "Folder\\shell\\Add Folder to PATH\\command";

  // dir of type LPCWSTR assigned to (converter.from_bytes(dirPath) cast to a
  // wide string) converted to a const wchar_t*
  // LPCWSTR dir = ((std::wstring)converter.from_bytes(dirPath)).c_str();

  LPCWSTR dir = converter.from_bytes(dirPath).c_str();

  LPSTR lpData = _strdup(data.c_str());

  if (!createKey(HKEY_CLASSES_ROOT,
                 L"Folder\\shell\\Add Folder to PATH\\command", hkey)) {
    logPause("Error during RegOpenKeyW(): ");
    return 1;
  };

  if (!openKey(HKEY_CLASSES_ROOT, L"Folder\\shell\\Add Folder to PATH\\command",
               KEY_ALL_ACCESS, hkey)) {
    logPause("Error during RegOpenKeyEx()");
    return 1;
  }

  if (!setValue(HKEY_CLASSES_ROOT, "Folder\\shell\\Add Folder to PATH\\command",
                REG_SZ, lpData)) {
    logPause("Error during RegSetValueA()");
    return 1;
  }

  if (RegCloseKey(hkey) != ERROR_SUCCESS) {
    logPause("Error during RegCloseKey()");
    return 1;
  }

  logPause("Operation successful. You may close this window.");
  return 0;
};