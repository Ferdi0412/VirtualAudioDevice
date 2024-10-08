# VirtualAudioDevice
I am exploring the options with regards to accessing audio inputs and outputs for WWindows devices. My eventual goal is to be able to get to a point where I can have a program that can produce reliable text from speech, even in noisy environments and it would be great if it could recognize several people from audio. For example, my grandmother is hard of hearing and struggles when several people talk, or there is considerable background noise. She will often reply with "sure" as she is tired of asking people to repeat themselves. I have looked at various voice-to-text applications but the expect the target to be close to the microphone, and are reliable when one person is speaking.

## Building

```bash
cmake -S (project-dir) -B (build-dir)

cmake --build (build-dir)
```

## Running

```bash
start /b /w (path/to/project.exe)
```

## Features
In order for my ultimate goal to be achievable I at least some of the following will be required:

1. Audio splitting/demixing - separate various audio sources to simplify the noise cancellation.
2. Audio rerouting - such that the audio splitting can be run on various audio inputs (even outputs to allow for text on movies/songs/similar)
3. Voice recognition (more on the categorizing the voice as belonging to a person) - It is better in my current opinion that the same person is recognized as different people (eg. if they have differing pitch/tone through their speach) rather than several people recognized as one
4. Voice to text (with various languages - maybe this could be based on the "phoenetic symbols" - assuming words are pronounced phoenetically - this falls apart with Norwegian dialects...)
5. Some interface to display the text from a given voice, maybe even several - so that they can select those they whish to listen to...

# Windows Audio

From [chatGPT](https://chatgpt.com/c/67042790-97e8-800f-834b-61f5b6c42d99)...

Useful docs on [WASAPI](https://www.thewelltemperedcomputer.com/KB/WASAPI.htm).

A tutorial on [WASAPI](https://medium.com/@shahidahmadkhan86/sound-in-windows-the-wasapi-in-c-23024cdac7c6).

## COM Coding Practices
https://learn.microsoft.com/en-us/windows/win32/learnwin32/com-coding-practices

## Capture Audio with WASAPI

```C++
// Initialize the COM library
CoInitialize(nullptr);

// Get the default audio capture device
IMMDeviceEnumerator *deviceEnumerator = nullptr;
CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator));

IMMDevice *audioDevice = nullptr;
deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &audioDevice);

// Initialize audio client
IAudioClient *audioClient = nullptr;
audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&audioClient));

WAVEFORMATEX *waveFormat = nullptr;
audioClient->GetMixFormat(&waveFormat);
audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 0, 0, waveFormat, nullptr);

// Create capture client
IAudioCaptureClient *captureClient = nullptr;
audioClient->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(&captureClient));

```

## Playback Audio

```C++
IMMDevice *outputDevice = nullptr;
deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &outputDevice);

IAudioClient *renderClient = nullptr;
outputDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&renderClient));

renderClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 0, 0, waveFormat, nullptr);

IAudioRenderClient *audioRenderClient = nullptr;
renderClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audioRenderClient));

```

## Forward Audio

```C++
BYTE *data;
UINT32 packetLength = 0;
while (true) {
    captureClient->GetNextPacketSize(&packetLength);

    if (packetLength > 0) {
        UINT32 numFramesAvailable;
        captureClient->GetBuffer(&data, &numFramesAvailable, nullptr, nullptr, nullptr);

        audioRenderClient->GetBuffer(numFramesAvailable, &data);
        audioRenderClient->ReleaseBuffer(numFramesAvailable, 0);
    }
}

```

## Further suggestions:
**PortAudio** or **RtAudio**
