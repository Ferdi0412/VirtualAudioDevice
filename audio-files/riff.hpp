// https://www.tactilemedia.com/info/MCI_Control_Info.html

// Also consider using basic_string<uint8_t>...

#include <stdint.h>
#include <string>
// #include <array>
#include <iostream>

class RiffChunk {
    private:
        std::string _chunk;

    public:
        // Official CKIDs
        static constexpr uint32_t CKID_RIFF = (('R') + ('I' << 8) + ('F' << 16) + ('F' << 24)), // RIFF file identifier
                                  CKID_PAL  = (('P') + ('A' << 8) + ('L' << 16) + (' ' << 24)), // Palette
                                  CKID_RDIB = (('R') + ('D' << 8) + ('I' << 16) + ('B' << 24)), // Device Independent Bitmap
                                  CKID_RMID = (('R') + ('M' << 8) + ('I' << 16) + ('D' << 24)), // MIDI
                                  CKID_RMMP = (('R') + ('M' << 8) + ('M' << 16) + ('P' << 24)), // Multimedia Movie File
                                  CKID_WAVE = (('W') + ('A' << 8) + ('V' << 16) + ('E' << 24)); // Waveform

        // Constructors from text - supports lazy-loading
        RiffChunk(const std::string& chunk = "", size_t position = 0);
        static RiffChunk from_string(const std::string& chunk, size_t position = 0);

        // Just "header" section...
        static RiffChunk just_id_n_size(uint32_t id, uint32_t size);
        static RiffChunk just_id_n_size(const std::string& chunk, size_t position = 0);
        static RiffChunk just_id_n_size(std::istream& chunk_stream, std::streamoff off = 0, std::ios_base::seekdir way = std::ios_base::cur);

        // Constructors from a stream/filestream - reset stream to current after completion...
        static RiffChunk from_stream(std::istream& chunk_stream, std::streamoff off = 0, std::ios_base::seekdir way = std::ios_base::cur);

        // Checks RIFF identifier as one of: RIFF, PAL, RDIB, RMID, RMMP, WAVE
        bool validate_ckid();
        static bool validate_ckid(uint32_t ckid);
        // static bool validate_ckid(const std::array<uint8_t, 4>& ckid);

        // Checks that the ck_size field is correct according to the length of the data
        bool validate_cksize();

        // Getters - throws std::length_error if the chunk is not long enough to contain both...
        uint32_t get_ckid();
        static uint32_t get_ckid(const std::string& chunk, size_t position = 0);
        static uint32_t get_ckid(std::istream& chunk_stream, std::streamoff off = 0, std::ios_base::seekdir way = std::ios_base::cur);
        uint32_t get_cksize();
        static uint32_t get_cksize(const std::string& chunk, size_t position = 0);
        static uint32_t get_cksize(std::istream& chunk_stream, std::streamoff off = 0, std::ios_base::seekdir way = std::ios_base::cur);

        // Setters - If chunk length < 8, will assign '\0' to all other fields...
        void set_ckid(uint32_t ckid);
        // void set_ckid(const std::array<uint8_t, 4>& ckid);
        void set_ckid(const std::string& ckid);
        void set_cksize(uint32_t cksize);

        // Data handlers - If ckid and cksize not set, will throw std::length_error
        void overwrite_data(const std::string& data);
        void append_data(const std::string& data);

        // Returns the chunk raw
        const std::string& get_raw();

        // Returns the data
        std::string get_data();
};
