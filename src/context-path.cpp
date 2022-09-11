// context-path.cpp
// clang, ISO C++20
// adds options allowing the addition and removal of a folder from PATH
// to the context menu

#include <atlbase.h>
#include <atlconv.h>
#include <windows.h>

#include <iostream>
// linker -> manifest file -> UAC Execution level -> administrator could be used
// to force running as admin but this would run every time we use it from the
// context menu as well note that it won't add the context menu entry unless exe
// is run as administrator

HKEY hkey;

void logPause(std::string_view msg) {
  std::cout << msg << std::endl;
  system("pause > nul");
}

// https://docs.microsoft.com/en-us/windows/win32/api/winreg/

bool createKey(HKEY&& hkey, const LPCWSTR& lpSubKey, HKEY& phkResult) {
  return RegCreateKeyW(hkey, lpSubKey, &phkResult) == ERROR_SUCCESS;
};

bool openKey(HKEY&& hkey, const LPCWSTR& lpSubKey, REGSAM&& samDesired,
             HKEY& phkResult) {
  return RegOpenKeyEx(hkey, lpSubKey, 0, samDesired, &phkResult) ==
         ERROR_SUCCESS;
}

bool setValue(HKEY&& hkey, const LPCSTR& lpSubKey, DWORD&& dwType,
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

int main(int argc, char* argv[]) {
  // indexing argv[1] when it doesn't exist should cause an access violation
  // error
  if (argc > 1) {
    ShowWindow(::GetConsoleWindow(), SW_HIDE);
    openKey(HKEY_CURRENT_USER, L"Environment", KEY_ALL_ACCESS, hkey);

    // TODO add getPath function
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
      int&& startPos = valStr.find(argv[1]);
      if (argc == 3) {
        if (startPos != std::string::npos) {
          //  include semicolon in the part to be removed
          valStr.erase(startPos, valStr.length() + 1);
        }
      } else {
        if (startPos == std::string::npos) valStr += argv[1] + std::string(";");
        //  make it set the value of a certain lpValue, (not lpKey!)
        //  you can specify a specific key with a handle obtained from
        //  RegOpenKeyEx() which takes a pointer to a variable that later
        //  receives a handle to the opened key
      };
      LSTATUS RegSetValueExASuccess =
          RegSetValueExA(hkey, "Path", 0, REG_SZ, (const BYTE*)valStr.c_str(),
                         valStr.size() + 1) == ERROR_SUCCESS;

      RegCloseKey(HKEY_CURRENT_USER);
      if (!RegSetValueExASuccess) {
        logPause("could not setValue");
      }

    } else {
      std::cout << GetLastError();
      logPause("could not getValue");
    };
    return 0;
  }

  std::cout
      << "make sure you're running this with administrative priveleges or it "
         "WILL NOT WORK"
      << std::endl;

  std::string data = std::move(argv[0]);
  data.append(" \"%1\"");

  // prefix both with the same thing so they end up grouped together
  std::string dirPathAdd = "Folder\\shell\\CP Add Folder to PATH\\command";
  std::string dirPathRemove =
      "Folder\\shell\\CP Remove Folder from PATH\\command";
  // std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  // dir of type LPCWSTR assigned to (converter.from_bytes(dirPath) cast to a
  // wide string) converted to a const wchar_t*
  // LPCWSTR dir = ((std::wstring)converter.from_bytes(dirPath)).c_str();

  LPCWSTR dirAdd = std::wstring(dirPathAdd.begin(), dirPathAdd.end()).c_str();
  LPCWSTR dirRemove =
      std::wstring(dirPathRemove.begin(), dirPathRemove.end()).c_str();

  LPSTR lpData = _strdup(data.c_str());
  LPSTR lpDataR = _strdup((data + " 1").c_str());

  if (!createKey(HKEY_CLASSES_ROOT, dirAdd, hkey)) {
    logPause("Error during RegOpenKeyW(): ");
    return 1;
  };

  if (!openKey(HKEY_CLASSES_ROOT, dirAdd, KEY_ALL_ACCESS, hkey)) {
    logPause("Error during RegOpenKeyEx()");
    return 1;
  }

  if (!setValue(HKEY_CLASSES_ROOT,
                "Folder\\shell\\CP Add Folder to PATH\\command", REG_SZ,
                lpData)) {
    logPause("Error during RegSetValueA()");
    return 1;
  }

  if (RegCloseKey(hkey) != ERROR_SUCCESS) {
    logPause("Error during RegCloseKey()");
    return 1;
  }

  // could use a for loop too

  if (!createKey(HKEY_CLASSES_ROOT, dirRemove, hkey)) {
    logPause("Error during RegOpenKeyW(): ");
    return 1;
  };

  if (!openKey(HKEY_CLASSES_ROOT, dirRemove, KEY_ALL_ACCESS, hkey)) {
    logPause("Error during RegOpenKeyEx()");
    return 1;
  }

  if (!setValue(HKEY_CLASSES_ROOT,
                "Folder\\shell\\CP Remove Folder from PATH\\command", REG_SZ,
                lpDataR)) {
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
