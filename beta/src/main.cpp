#include <stdio.h>
#include <Windows.h>
#include <initguid.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <assert.h>

#include <string>
#include <iostream>

void my_debug( bool it_worked, std::string error_msg, int exit_code = 1 ) {
    if ( it_worked )
        return;
    // else
    std::cout << error_msg << "\n";
    exit(exit_code);
}

int main (void) {
    std::cout << "Started!\n";
    // COM objects
    HRESULT hr;
    IMMDeviceEnumerator* device_enumerator = nullptr;

    // WASAPI objects
    IMMDevice*    audio_capture  = nullptr;
    IAudioClient* capture_client = nullptr;
    WAVEFORMATEX* wave_format     = nullptr;


    // Initialize the "Component Object Model" (COM) library
    hr = CoInitialize(nullptr); // CoInitializeEx(NULL, COINIT_MULTITHREADED) - Concurrent app alternative
    my_debug( hr == S_OK, "CoInitialize failed...");

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator,
                          nullptr,
                          CLSCTX_ALL,
                          IID_IMMDeviceEnumerator,
                          (void**)&device_enumerator);
    my_debug( hr == S_OK, "CoCreateInstance failed...");

    // Get default audio capture endpoint
    hr = device_enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &audio_capture);
    my_debug( hr == S_OK, "getDefaultAudioEndpoint failed...");

    // Activate audio capture device
    // --> Checkout reinterpret_cast<void**>(&capture_client);
    hr = audio_capture->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&capture_client);
    my_debug( hr == S_OK, "audio_capture->Activate() failed...");

    // Get audio format
    hr = capture_client->GetMixFormat(&wave_format);
    my_debug( hr == S_OK, "audio_capture->GetMixFormat() failed...");

    // Print details on mix format
    std::cout << "Mix format:\n";
    std::cout << "  Frame size     : " << wave_format->nBlockAlign    << "\n";
    std::cout << "  nChannels      : " << wave_format->nChannels      << "\n";
    std::cout << "  Bits per sample: " << wave_format->wBitsPerSample << "\n";
    std::cout << "  Samples per Sec: " << wave_format->nSamplesPerSec << "\n";

    // Cleanup...
    device_enumerator->Release();
    my_debug(hr == S_OK, "device_enumerator->Release() failed...");

    capture_client->Release();
    my_debug(hr == S_OK, "capture_client->Release() failed...");

    audio_capture->Release();
    my_debug(hr == S_OK, "audio_capture->Release() failed...");

    CoUninitialize();
}
