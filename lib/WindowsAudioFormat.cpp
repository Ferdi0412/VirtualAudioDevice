#include "windows_audio.hpp"

WindowsAudioFormat::WindowsAudioFormat( WAVEFORMATEX format ) {
    frame_size      = (size_t) format.nBlockAlign;
    n_channels      = (size_t) format.nChannels;
    bits_per_sample = (size_t) format.wBitsPerSample;
    sample_rate     = (size_t) format.nSamplesPerSec;
}
