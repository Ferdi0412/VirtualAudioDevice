#include "windows_audio.hpp"

void WindowsAudioClient::deallocate( void ) {
    if ( client == nullptr )
        return;

    // if ( started )
    //     stop();

    client->Release();
    client = nullptr;
}

// REFIID WindowsAudioClient::iid = IID_IAudioClient;

WindowsAudioClient::WindowsAudioClient( IAudioClient* client ): client(client) {
    // Nothing else to init...
}

WindowsAudioClient::~WindowsAudioClient( void ) {
    deallocate();
}

// TODO: start, stop, initialize, get_service, etc...

IAudioClient** WindowsAudioClient::ptrptr( void ) {
    return &client;
}

IUnknown** WindowsAudioClient::iptrptr( void ) {
    return (IUnknown**) ptrptr();
}

