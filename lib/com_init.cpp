#include "windows_audio.hpp"

static bool com_initialized = false;

void com_init( DWORD dw_co_init ) {
    HRESULT hr;

    if ( com_initialized )
        throw std::runtime_error("[com_init] COM is already initialized!");

    hr = CoInitializeEx(nullptr, dw_co_init);
    if ( !SUCCEEDED(hr) )
        throw std::runtime_error("[com_init] COM failed to initialize!");

    com_initialized = true;
}

void com_uninit( void ) {
    CoUninitialize();
    com_initialized = false;
}

bool com_is_init( void ) {
    return com_initialized;
}

void com_init_if_uninit( void ) {
    if ( !com_is_init() )
        com_init();
}
