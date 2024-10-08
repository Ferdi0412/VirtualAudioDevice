#include "windows_audio.hpp"




const EDataFlow WindowsAudioEnumerator::data_input        = eCapture;
const EDataFlow WindowsAudioEnumerator::data_output       = eRender;
const EDataFlow WindowsAudioEnumerator::data_input_output = eAll;

const ERole WindowsAudioEnumerator::role_multimedia     = eMultimedia;
const ERole WindowsAudioEnumerator::role_console        = eConsole;
const ERole WindowsAudioEnumerator::role_communications = eCommunications;

void WindowsAudioEnumerator::release_devices( void ) {
    if ( devices == nullptr )
        return;
    devices->Release();
    devices = nullptr;
}

void WindowsAudioEnumerator::refresh_devices( DWORD device_state ) {
    HRESULT hr;

    if ( enumerator == nullptr )
        throw std::runtime_error("[WindowsAudioEnumerator::refresh_devices] The enumerator has to first be set!");

    release_devices();

    hr = enumerator->EnumAudioEndpoints(data_flow,
                                        device_state,
                                        &devices);
    if ( !SUCCEEDED(hr) ) {
        release_devices(); // Incase it is no longer a nullptr...
        throw std::runtime_error("[WindowsAudioEnumerator::refresh_devices] Could not enumerate devices!");
    }
}

void WindowsAudioEnumerator::refresh_devices_if_none( DWORD device_state ) {
    if ( devices == nullptr )
        refresh_devices();
}

void WindowsAudioEnumerator::deallocate( void ) {
    if ( enumerator == nullptr )
        return;
    enumerator->Release();
    enumerator = nullptr;
}

void WindowsAudioEnumerator::allocate( void ) {
    HRESULT hr;

    if ( !com_is_init() )
        throw std::runtime_error("[WindowsAudioEnumerator::allocate] Please run com_init before instantiating this class!");

    deallocate();

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator,
                          nullptr,
                          CLSCTX_ALL,
                          IID_IMMDeviceEnumerator,
                          (void**) &enumerator);
    if ( !SUCCEEDED(hr) )
        throw std::runtime_error("[WindowsAudioEnumerator::allocate] Could not create the required COM object!");
}

WindowsAudioEnumerator::WindowsAudioEnumerator( EDataFlow data_flow ): data_flow(data_flow) {
    allocate();
}

WindowsAudioEnumerator::~WindowsAudioEnumerator( void ) {
    release_devices();
    deallocate();
}

size_t WindowsAudioEnumerator::count_devices( void ) {
    HRESULT hr;
    UINT device_count;

    refresh_devices_if_none();

    hr = devices->GetCount(&device_count);
    if ( !SUCCEEDED(hr) )
        throw std::runtime_error("[WindowsAudioEnumerator::count_devices] Could not GetCount!");

    return (size_t) device_count;
}

std::vector<std::wstring> WindowsAudioEnumerator::list_names() {
    HRESULT hr;
    UINT    device_count;
    std::vector<std::wstring> names;

    device_count = (UINT) count_devices();

    for ( UINT i = 0; i < device_count; i++ ) {
        WindowsAudioDevice device(nullptr);

        hr = devices->Item(i, device.ptrptr());
        if ( !SUCCEEDED(hr) )
            throw std::runtime_error("[WindowsAudioEnumerator::list_names] Could not get an Item from devices!");

        names.push_back(device.get_name());
    }

    return names;
}

WindowsAudioDevice WindowsAudioEnumerator::operator[] ( const std::wstring& device_name ) {
    HRESULT            hr;
    UINT               device_count;
    WindowsAudioDevice target(nullptr);

    device_count = (UINT) count_devices();

    for ( UINT i = 0; i < device_count; i++ ) {
        WindowsAudioDevice device(nullptr);

        hr = devices->Item(i, device.ptrptr());
        if ( !SUCCEEDED(hr) )
            throw std::runtime_error("[WindowsAudioEnumerator::operator[]] Could not get an Item from devices!");

        if ( device.get_name() == device_name ) {
            hr = devices->Item(i, target.ptrptr() );
            if ( !SUCCEEDED(hr) )
                throw std::runtime_error("[WindowsAudioEnumerator::operator[]] Could not get target Item from devices!");
        }
    }

    return target;
}

WindowsAudioDevice WindowsAudioEnumerator::get_default_device( ERole audio_role ) {
    HRESULT            hr;
    WindowsAudioDevice device(nullptr);

    if ( enumerator == nullptr )
        throw std::runtime_error("[WindowsAudioDevice::get_default_devices] Enumerator not allocated!");

    hr = enumerator->GetDefaultAudioEndpoint( data_flow, audio_role, device.ptrptr() );
    if ( !SUCCEEDED(hr) )
        throw std::runtime_error("[WindowsAudioEnumerator::get_default_device] Could not retrieve the default!");

    return device;
}
