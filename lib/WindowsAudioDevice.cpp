#include "windows_audio.hpp"


void WindowsAudioDevice::deallocate(void) {
    if ( device != nullptr ) {
        device->Release();
        device = nullptr;
    }
}

WindowsAudioDevice::WindowsAudioDevice( IMMDevice* audio_device ): device( audio_device ) {
    // Nothing else for the constructor
}

WindowsAudioDevice::~WindowsAudioDevice(void) {
    deallocate();
}

std::wstring WindowsAudioDevice::get_name(void) {
    HRESULT         hr;
    PROPVARIANT     dev_name;
    IPropertyStore* props = nullptr;
    std::wstring    name;

    if ( device == nullptr )
        throw std::runtime_error("[WindowsAudioDevice::get_name] The device has not been initialized...");

    hr = device->OpenPropertyStore(STGM_READ, &props);
    if ( !SUCCEEDED(hr) )
        throw std::runtime_error("[WindowsAudioDevice::get_name] Failed to open device PropertyStore!");

    PropVariantInit(&dev_name);

    hr = props->GetValue(PKEY_Device_FriendlyName, &dev_name);
    if ( !SUCCEEDED(hr) )
        throw std::runtime_error("[WindowsAudioDevice::get_name] Failed to get name value!");

    // Retrieve name, which is stored using utf-16
    name = dev_name.pwszVal;

    PropVariantClear(&dev_name);
    props->Release();

    return name;
}

IMMDevice** WindowsAudioDevice::ptrptr(void) {
    deallocate(); // To avoid memory leaks on multiple assignment...
    return &device;
}

void WindowsAudioDevice::activate( IUnknown** ptrptr, REFIID iid ) {
    HRESULT hr;

    if ( device == nullptr )
        throw std::runtime_error("[WindowsAudioDevice::activate] The device is not assigned!");

    hr = device->Activate(iid,
                            CLSCTX_ALL,
                            nullptr,
                            (void**) ptrptr);
    if ( !SUCCEEDED(hr) )
        throw std::runtime_error("[WindowsAudioDevice::activate] Could not activate!");
}

WindowsAudioClient WindowsAudioDevice::activate_client(void) {
    WindowsAudioClient new_client(nullptr);
    activate(new_client.iptrptr());
    return new_client;
}
