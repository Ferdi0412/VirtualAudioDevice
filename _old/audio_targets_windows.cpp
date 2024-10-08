#include "audio_targets.hpp"

#include <stdio>

#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <PropIdl.h>
#include <functiondiscoverykeys_devpkey.h>

void com_assert( HRESULT com_hres, std::string err_msg ) {
    if ( com_hres == S_OK )
        return; // Successful
    // else
    std::cout << "\n" << error_msg << "\n";
    exit(1);
}

static size_t                   AudioTarget::count_outputs() {
    std::vector<std::string> names;
    HRESULT                  res;
    IMMDeviceEnumerator*     device_enum = nullptr;
    IMMDeviceCollection*     device_coll = nullptr;
    UINT                     device_count = 0;


    res = CoInitialize(nullptr);
    com_assert(res, "CoInitialize");

    res = CoCreateInstance(CLSID_MMDeviceEnumerator,
                           nullptr,
                           CLSCTX_ALL,
                           IID_IMMDeviceEnumerator,
                           (void**)&device_enum);
    com_assert(res, "CoCreateInstance");

    res = device_enum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &device_coll);
    com_assert(res, "EnumAudioEndpoints");

    res = device_coll->GetCount(&device_count);
    com_assert(res, "GetCount");

    for ( UINT i = 0; i < device_count; i++ ) {
        IMMDevice*      device = nullptr;
        IPropertyStore* props;
        PROPVARIANT     dev_name;

        res = device_coll->Item(i, &device);
        com_assert(res, "Item");

        res = device->OpenPropertyStore(STGM_READ, &props);
        com_assert(res, "OpenPropertyStore");

        res = PropVariantInit(&dev_name);
        com_assert(res, "PropVariantInit");

        res = props->GetValue(PKEY_Device_FriendlyName, &dev_name);
        com_assert(res, "GetValue");

        names.push_back(dev_name.pwszVal);

        res = PropVariantClear(&dev_name);
        com_assert(res, "PropVariantClear");

        res = props->Release();
        com_assert(res, "props->Release()");

        device->Release();
        com_assert(res, "device->Release()");
    }
}

static std::vector<std::string> AudioTarget::list_outputs()
