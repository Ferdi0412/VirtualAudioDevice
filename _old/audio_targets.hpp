#include <string>
#include <vector>
#include <cstddef> // Alt <stdint.h>

class AudioTarget {
    private:

    public:
        static size_t                   count_outputs();
        static std::vector<std::string> list_outputs();
};

// Class for working with the windows WASAPI for sound
// Throws std::except for many things...


class WindowsAudio {
    private:

    public:

}
