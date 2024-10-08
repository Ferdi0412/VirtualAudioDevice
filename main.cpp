#include "lib/windows_audio.hpp"

#include <iostream>

int main( void ) {

    com_init();

    WindowsAudioEnumerator enum_test( WindowsAudioEnumerator::data_input );

    std::vector<std::wstring> device_names = enum_test.list_names();

    for ( const std::wstring& el : device_names )
        std::wcout << el << std::endl;

    com_uninit();
}
