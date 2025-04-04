#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <devguid.h>
#include <usbiodef.h>
#include <devpkey.h>
#include <stdio.h>

#pragma comment(lib, "setupapi.lib")

void PrintDeviceProperty(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA* deviceInfoData, DWORD property, const char* label) {
    char propertyBuffer[1024];
    DWORD requiredSize = 0;
    DWORD propertyType = 0;

    if (SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, deviceInfoData, property, &propertyType,
                                           (PBYTE)propertyBuffer, sizeof(propertyBuffer), &requiredSize)) {
        printf("  %s: %s\n", label, propertyBuffer);
    } else {
        printf("  %s: [Unavailable]\n", label);
    }
}

void EnumerateUSBDevices() {
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_USB, NULL, NULL, DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        printf("Failed to retrieve the USB device list.\n");
        return;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD index = 0;

    printf("Connected USB Devices:\n");

    while (SetupDiEnumDeviceInfo(deviceInfoSet, index, &deviceInfoData)) {
        printf("\nDevice %d:\n", index + 1);

        PrintDeviceProperty(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC, "Device Description");
        PrintDeviceProperty(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, "Hardware ID");
        PrintDeviceProperty(deviceInfoSet, &deviceInfoData, SPDRP_MFG, "Manufacturer");
        PrintDeviceProperty(deviceInfoSet, &deviceInfoData, SPDRP_FRIENDLYNAME, "Friendly Name");
        PrintDeviceProperty(deviceInfoSet, &deviceInfoData, SPDRP_LOCATION_INFORMATION, "Location");

        index++;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}

int main() {
    EnumerateUSBDevices();
    return 0;
}
