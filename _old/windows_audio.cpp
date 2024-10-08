#include <stdexcept>
#include <vector>
#include <string>

#include <cstddef>

#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h> // For access to: PKEY_Device_FriendlyName
#include <Audioclient.h>

static bool com_initialized = false;


// TODO: Continue frtom client initialize


// dw_co_init can be "COINIT_APARTMENTTHREADED" or "COINIT_MULTITHREADED"
void initialize_windows_audio( DWORD dw_co_init = COINIT_APARTMENTTHREADED ) {
    HRESULT hr;
    if ( !com_initialized ) {
        hr = CoInitializeEx(nullptr, dw_co_init);
        if ( !SUCCEEDED(hr) )
            throw std::exception("[WindowsAudioEnumerator::WindowsAudioEnumerator] Could not initialize the windows COM library");
        com_initialized = true;
    }
}






void uninitialize_windows_audio() {
    CoUninitialize();
}






class WindowsAudioDevice {
    private:
        IMMDevice* device = nullptr;

        void deallocate() {
            if ( device != nullptr ) {
                device->Release();
                device = nullptr;
            }
        }

    public:
        WindowsAudioDevice(IMMDevice* audio_device) : device(audio_device) { }

        ~WindowsAudioDevice() {
            deallocate();
        }

        std::wstring get_name() {
            HRESULT         hr;
            IPropertyStore* props = nullptr;
            PROPVARIANT     dev_name;
            std::wstring    name;

            hr = device->OpenPropertyStore(STGM_READ, &props);
            if ( !SUCCEEDED(hr) )
                throw std::exception("[WindowsAudioDevice::get_name] Failed to open device's PropertyStore!");

            PropVariantInit(&dev_name);
            hr = props->GetValue(PKEY_Device_FriendlyName, &dev_name);
            if ( !SUCCEEDED(hr) )
                throw std::exception("[WindowsAudioDevice::get_name] Failed to retrieve device's Name!");

            // Name is stored using utf-16
            name = dev_name.pwszVal;

            PropVariantClear(&dev_name);
            props->Release();

            return name;
        }

        IMMDevice** ptrptr() {
            // Deallocates to avoid memory leaks...
            deallocate();
            return &device;
        }

        void activate_audio_client( IAudioClient** client ) {
            HRESULT hr;
            // For other devices see https://learn.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdevice-activate
            // For other CoRegisterClassObjects enumerations see https://learn.microsoft.com/en-us/windows/win32/api/wtypesbase/ne-wtypesbase-clsctx
            hr = device->Activate(IID_IAudioClient,
                                  CLSCTX_ALL,
                                  nullptr,
                                  (void**) client);
            if ( !SUCCEEDED(hr) )
                throw std::exception("[WindowsAudioDevice::activate_audio_client] Could not activate an audio client!");
        }
};



class WindowsAudioClient {
    private:
        IAudioClient* client = nullptr;
        WAVEFORMATEX* format = nullptr;

        void release_format() {
            if ( format != nullptr ) {
                free(format);
            }
            format = nullptr;
        }

        void refresh_format() {
            HRESULT hr;
            release_format();
            hr = client->GetMixFormat(&format);
            if ( !SUCCEEDED(hr) )
                throw std::exception("[WindowsAudioClient::refresh_format] Could not get mix format!");
        }

        void refresh_format_if_none() {
            if ( format == nullptr )
                refresh_format();
        }

        void deallocate() {
            // TODO: HRESULT
            if (client != nullptr) {
                client->Release();
            }
        }


    public:
        WindowsAudioClient(WindowsAudioDevice device) {
            HRESULT hr;
            device.activate_audio_client(&client);
        }

        size_t get_frame_size( ) {
            refresh_format_if_none();
            return (size_t) format->nBlockAlign;
        }

        size_t get_n_channels( ) {
            refresh_format_if_none();
            return (size_t) format->nChannels;
        }

        size_t get_bits_per_sample( ) {
            refresh_format_if_none();
            return (size_t) format->wBitsPerSample;
        }

        size_t get_sample_rate( ) {
            refresh_format_if_none();
            return (size_t) format->nSamplesPerSec;
        }
};



// To avoid forgetting to release stuff....
class WindowsAudioEnumerator {
    private:
        IMMDeviceEnumerator* enumerator = nullptr;
        IMMDeviceCollection* devices    = nullptr;
        EDataFlow            data_flow; // data_flow can be { eRender (audio output, such as speaker); eCapture (audio input, such as mic); eAll}

        void release_devices() {
            if ( devices == nullptr )
                return;
            devices->Release();
            devices = nullptr;
        }

        // devices_state can be bitwise set with: {DEVICE_STATE_ACTIVE, DEVICE_STATE_DISABLED, DEVICE_STATE_NOTPRESENT, DEVICE_STATE_UNPLUGGED}
        void refresh_devices(DWORD device_state = DEVICE_STATE_ACTIVE) {
            HRESULT hr;

            release_devices();

            hr = enumerator->EnumAudioEndpoints(data_flow,
                                                device_state,
                                                &devices);
            if ( !SUCCEEDED(hr) ) {
                release_devices(); // incase devices set by command...
                throw std::exception("[WindowsAudioEnumerator::refresh_devices] Could not enumerate audio devices...");
            }
        }

        void refresh_devices_if_none() {
            if ( devices == nullptr )
                refresh_devices();
        }

        void deallocate() {
            if ( enumerator != nullptr ) {
                enumerator->Release();
                enumerator = nullptr;
            }
        }

    public:
        static EDataFlow data_input() {
            return eCapture;
        }

        static EDataFlow data_output() {
            return eRender;
        }

        static EDataFlow data_input_output() {
            return eAll;
        }

        WindowsAudioEnumerator (EDataFlow data_flow) : data_flow(data_flow) {
            HRESULT hr;

            initialize_windows_audio();

            hr = CoCreateInstance(CLSID_MMDeviceEnumerator,
                                  nullptr,
                                  CLSCTX_ALL,
                                  IID_IMMDeviceEnumerator,
                                  (void**)&enumerator);
            if ( !SUCCEEDED(hr) )
                throw std::exception("[WindowsAudioEnumerator::WindowsAudioEnumerator] Could not create a COM object for the Windows Audio...");
        }

        ~WindowsAudioEnumerator() {
            release_devices();
            deallocate();
        }

        // Destroys enumerator, such to avoid memory leaks upon future allocations...
        IMMDeviceEnumerator** ptrptr() {
            deallocate();
            return &enumerator;
        }

        size_t count_devices() {
            HRESULT hr;
            UINT    device_count;

            refresh_devices_if_none();

            hr = devices->GetCount(&device_count);
            if ( !SUCCEEDED(hr) )
                throw std::exception("[WindowsAudioEnumerator::count_devices] Could not GetCount!");

            return (size_t) device_count;
        }

        std::vector<std::wstring> list_names() {
            HRESULT hr;
            UINT    device_count = (UINT) count_devices(); // runs refresh_devices_if_none
            std::vector<std::wstring> names;

            for ( UINT i = 0; i < device_count; i++ ) {
                WindowsAudioDevice device(nullptr);
                // IMMDevice*      device = nullptr;
                // IPropertyStore* props  = nullptr;
                // PROPVARIANT     dev_name;

                hr = devices->Item(i, device.ptrptr() );
                if ( !SUCCEEDED(hr) )
                    throw std::exception("[WindowsAudioEnumerator::list_names] Could not get an Item from devices!");

                names.push_back(device.get_name());

                // hr = devices->Item(i, &device);
                // if ( !SUCCEEDED(hr) )
                //     throw std::exception("[WindowsAudioEnumerator::list_names] Could not get an Item from devices!");

                // hr = device->OpenPropertyStore(STGM_READ, &props);
                // if ( !SUCCEEDED(hr) )
                //     throw std::exception("[WindowsAudioEnumerator::list_names] Could not open a property store!");

                // PropVariantInit(&dev_name);

                // hr = props->GetValue(PKEY_Device_FriendlyName, &dev_name);
                // if ( !SUCCEEDED(hr) )
                //     throw std::exception("[WindowsAudioEnumerator::list_names] Failed to initialize the dev_name prop!");

                // names.push_back(std::wstring(dev_name.pwszVal));

                // PropVariantClear(&dev_name);
                // props->Release(); // Assume no checks needed....
                // device->Release();
            }
        }

        WindowsAudioDevice operator[] (const std::wstring& device_name) {
            HRESULT    hr;
            UINT       device_count = (UINT) count_devices(); // runs refresh_devices_if_none
            WindowsAudioDevice target_dev(nullptr);
            // IMMDevice* target_dev   = nullptr;

            for ( UINT i = 0; i < device_count; i++ ) {
                WindowsAudioDevice dev_i(nullptr);

                hr = devices->Item(i, dev_i.ptrptr());
                if ( !SUCCEEDED(hr) )
                    throw std::exception("[WindowsAudioEnumerator::list_names] Could not get an Item from devices!");

                if ( dev_i.get_name() == device_name ) {
                    hr = devices->Item(i, target_dev.ptrptr() );
                    if ( !SUCCEEDED(hr) )
                        throw std::exception("[WindowsAudioEnumerator::list_names] Could not get the targeted Item from devices!");
                }
                // IMMDevice*      device = nullptr;
                // IPropertyStore* props  = nullptr;
                // PROPVARIANT     dev_name;

                // hr = devices->Item(i, &device);
                // if ( !SUCCEEDED(hr) )
                //     throw std::exception("[WindowsAudioEnumerator::list_names] Could not get an Item from devices!");

                // hr = device->OpenPropertyStore(STGM_READ, &props);
                // if ( !SUCCEEDED(hr) )
                //     throw std::exception("[WindowsAudioEnumerator::list_names] Could not open a property store!");

                // PropVariantInit(&dev_name);

                // hr = props->GetValue(PKEY_Device_FriendlyName, &dev_name);
                // if ( !SUCCEEDED(hr) )
                //     throw std::exception("[WindowsAudioEnumerator::list_names] Failed to initialize the dev_name prop!");

                // if ( device_name == dev_name.pwszVal ) {
                //     hr = devices->Item(i, &target_dev);
                //     if ( !SUCCEEDED(hr) )
                //         throw std::exception("[WindowsAudioEnumerator::list_names] Failed to initialize the target Item!");
                // }

                // PropVariantClear(&dev_name);
                // props->Release(); // Assume no checks needed....
                // device->Release();
            }

            return target_dev;
        }

        // Options for ERole = eConsole, eMultimedia, eCommunications (also available is ERole_enum_count)
        WindowsAudioDevice get_default_device ( ERole audio_role = eMultimedia ) {
            HRESULT            hr;
            WindowsAudioDevice default_dev(nullptr);

            // refresh_devices_if_none();

            hr = enumerator->GetDefaultAudioEndpoint( data_flow, audio_role, default_dev.ptrptr() );
            if ( !SUCCEEDED(hr) )
                throw std::exception("[WindowsAudioEnumerator::get_default_device] Could not get the default endpoint!");

        }

};
