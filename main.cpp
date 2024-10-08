#include "lib/windows_audio.hpp"

#include <iostream>

int main( void ) {

    com_init();

    WindowsAudioEnumerator enum_test( WindowsAudioEnumerator::data_input );

    // List devices
    std::vector<std::wstring> device_names = enum_test.list_names();

    for ( const std::wstring& el : device_names )
        std::wcout << el << std::endl;

    WindowsAudioDevice device_test = enum_test[device_names.front()];

    std::wcout << "First device is: " << device_test.get_name() << std::endl;

    WindowsAudioClient client_test = device_test.activate_client();

    std::cout << "End of program!\n";

    com_uninit();
}
