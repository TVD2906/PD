#include <iostream>
#include <windows.h>
#include <tchar.h>

using namespace std;

#define STRING_SIZE 256
#define IMAGE_PATH_SIZE 1024

void QueryRegistrySubKeys(HKEY hKey) {
    DWORD index = 0;
    WCHAR subKeyName[STRING_SIZE];
    DWORD subKeyNameSize;

    wcout << L"Subkeys:" << endl;
    
    while (true) {
        subKeyNameSize = STRING_SIZE;
        if (RegEnumKeyEx(hKey, index, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
            break;
        }

        wcout << L" - " << subKeyName << endl;
        
        HKEY hSubKey;
        if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
            WCHAR imagePath[IMAGE_PATH_SIZE];
            DWORD imagePathSize = sizeof(imagePath);
            
            if (RegQueryValueEx(hSubKey, L"ImagePath", NULL, NULL, (LPBYTE)imagePath, &imagePathSize) == ERROR_SUCCESS) {
                wcout << L"    ImagePath: " << imagePath << endl;
            } else {
                wcout << L"    ImagePath: (not found)" << endl;
            }
            
            DWORD valueIndex = 0;
            WCHAR valueName[STRING_SIZE];
            DWORD valueNameSize;
            BYTE valueData[IMAGE_PATH_SIZE];
            DWORD valueDataSize;
            DWORD valueType;
            
            wcout << L"    Values:" << endl;
            while (true) {
                valueNameSize = STRING_SIZE;
                valueDataSize = IMAGE_PATH_SIZE;
                if (RegEnumValue(hSubKey, valueIndex, valueName, &valueNameSize, NULL, &valueType, valueData, &valueDataSize) != ERROR_SUCCESS) {
                    break;
                }
                wcout << L"    - " << valueName << L" (Type: " << valueType << L")" << endl;
            
                if (valueType == REG_SZ || valueType == REG_EXPAND_SZ) {
                    wcout << L"      Data: " << (WCHAR*)valueData << endl;
                }
            
                valueIndex++;
            }

            WCHAR defaultValue[IMAGE_PATH_SIZE];
            DWORD defaultValueSize = sizeof(defaultValue);
            if (RegQueryValueEx(hSubKey, NULL, NULL, NULL, (LPBYTE)defaultValue, &defaultValueSize) == ERROR_SUCCESS) {
                wcout << L"    Default Value: " << defaultValue << endl;
            }
            
            RegCloseKey(hSubKey);
        }

        index++;
    }
}

int main() {
    HKEY hKey;
    LPCWSTR registryPath = L"SYSTEM\\CurrentControlSet\\Services";

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, registryPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        QueryRegistrySubKeys(hKey);
        RegCloseKey(hKey);
    } else {
        cerr << "Failed to open registry key." << endl;
    }

    return 0;
}