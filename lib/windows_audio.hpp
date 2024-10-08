#pragma once

#include <stdexcept>
#include <vector>
#include <string>

#include <cstddef>

// extern "C" {
// For IID_IMMDeviceEnumerator, CLSID_MMDeviceEnumerator
#include <initguid.h>

#include <mmdeviceapi.h>
#include <Audioclient.h>

// For access to: PKEY_Device_FriendlyName - must be last of includes...
#include <Functiondiscoverykeys_devpkey.h>
// #include <functiondiscoverykeys.h>


/* === HELPERS === */
void com_init( DWORD dw_co_init = COINIT_APARTMENTTHREADED );

void com_uninit();

bool com_is_init();

void com_init_if_uninit();



/* === DEVICE CLASS === */
class WindowsAudioDevice {
    private:
        IMMDevice* device = nullptr;

        void deallocate( );

    public:
        WindowsAudioDevice( IMMDevice* audio_device );

        ~WindowsAudioDevice( ); // deallocate()

        // Returns "friendly" name of device
        std::wstring get_name( );

        IMMDevice** ptrptr( ); // deallocate(); return &device;

        // IWindowsClient new_audio_client( );
}; // DONE



/* === ENUMERATOR CLASS === */
class WindowsAudioEnumerator {
    private:
        IMMDeviceEnumerator* enumerator = nullptr;
        IMMDeviceCollection* devices    = nullptr;
        EDataFlow            data_flow;

        void release_devices();

        // devices_state can be bitwise set with: {DEVICE_STATE_ACTIVE, DEVICE_STATE_DISABLED, DEVICE_STATE_NOTPRESENT, DEVICE_STATE_UNPLUGGED}
        void refresh_devices(DWORD device_state = DEVICE_STATE_ACTIVE);

        void refresh_devices_if_none(DWORD device_state = DEVICE_STATE_ACTIVE);

        void deallocate();

        void allocate();

    public:
        static const EDataFlow data_input;
        static const EDataFlow data_output;
        static const EDataFlow data_input_output;

        static const ERole     role_multimedia;
        static const ERole     role_console;
        static const ERole     role_communications;

        WindowsAudioEnumerator ( EDataFlow data_flow );

        ~WindowsAudioEnumerator(); // release_devices(); deallocate();

        // IMMDeviceEnumerator** ptrptr();

        size_t count_devices();

        std::vector<std::wstring> list_names();

        WindowsAudioDevice operator[] ( const std::wstring& device_name );

        WindowsAudioDevice get_default_device( ERole audio_role = eMultimedia );
};



/* === FORMAT WRAPPER === */
class WindowsAudioFormat {
    public:
        size_t frame_size,
               n_channels,
               bits_per_sample,
               sample_rate;

        WindowsAudioFormat( WAVEFORMATEX format );
};



// class IWindowsClient {
//     public:
//         IWindowsClient() {}
    // private:
    //     IAudioClient* client = nullptr;

    //     void deallocate();

    // public:
    //     IWindowsClient( WindowsAudioDevice device );

    //     // Returns size of each frame
    //     WindowsAudioFormat get_format();
// };


