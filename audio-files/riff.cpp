#include "riff.hpp"
#include <stdexcept>
#include <algorithm> // For std::min

// --=== Constructors ===--
RiffChunk::RiffChunk(const std::string& chunk, size_t position) {
    size_t size;
    // Try to retrieve cksize from chunk, and read 8 bytes of ckid, cksize, then cksize bytes of chunk
    try {
    size   = (size_t) get_cksize(chunk, position);
    _chunk = chunk.substr(position, size + 8);
    }

    // If size could not be retrieved, just assign _chunk as-is (defer errors for now)
    catch (std::length_error) {
        _chunk = chunk.substr(position, chunk.length() - position);
    }
}



RiffChunk RiffChunk::from_string(const std::string& chunk, size_t position) {
    return RiffChunk(chunk, position);
}



RiffChunk RiffChunk::just_id_n_size(uint32_t id, uint32_t size) {
    // Assign a RiffChunk without worrying about the data...
    RiffChunk new_chunk;
    new_chunk.set_ckid(id);
    new_chunk.set_cksize(size);
    return new_chunk;
}


RiffChunk RiffChunk::just_id_n_size(const std::string& chunk, size_t position) {
    // Try to get the first 8 characters - where the ckid and cksize is stored
    if ( chunk.length() < (position + 8) )
        throw std::length_error("[RiffChunk::just_id_n_size] Not long enough to retrieve data!");

    return RiffChunk(chunk.substr(position, 8));
}

RiffChunk RiffChunk::just_id_n_size(std::istream& chunk_stream, std::streamoff off, std::ios_base::seekdir way) {
    // Buffer to store ckid and cksize
    char buffer[8];

    // Store the initial position to return to after contstructing the chunk
    std::streampos initial_pos = chunk_stream.tellg();

    // Go to start of chunk in stream...
    chunk_stream.seekg(off, way);

    // Read 8 bytes to try get the cksize
    chunk_stream.read(buffer, 8);
    if ( chunk_stream.gcount() < 8 )
        throw std::length_error("[RiffChunk::just_id_n_size] Could not read 8 bytes to construct RiffChunk!");

    // Return to initial position
    chunk_stream.seekg(initial_pos);

    // Create chunk with the "header" section
    return RiffChunk(std::string(buffer, 8));
}


// --=== STREAM CONSTRUCTORS ===--
RiffChunk RiffChunk::from_stream(std::istream& chunk_stream, std::streamoff off, std::ios_base::seekdir way) {
    // Store initial position
    std::streampos initial_pos = chunk_stream.tellg();

    // Get "header" section
    RiffChunk new_chunk = just_id_n_size(chunk_stream, off, way);

    // Move to "data" section
    chunk_stream.seekg(off + 8, way);

    // Create an n-length string to write the data to from stream
    std::string data(' ', (size_t) new_chunk.get_cksize());

    // Try to read the data from the stream
    chunk_stream.read(&data[0], data.length());
    if ( chunk_stream.gcount() == data.length() )
        new_chunk.overwrite_data(data);

    // Return to initial position
    chunk_stream.seekg(initial_pos);

    return new_chunk;
}

// --=== Check if registered CKID for file ===--
bool RiffChunk::validate_ckid() {
    switch ( get_ckid() ) {
        case CKID_RIFF:
        case CKID_PAL :
        case CKID_RDIB:
        case CKID_RMID:
        case CKID_RMMP:
        case CKID_WAVE:
            return true;

        default:
            return false;
    }
}

bool RiffChunk::validate_ckid(uint32_t ckid) {
    RiffChunk temp = just_id_n_size(ckid, 0);
    return temp.validate_ckid();
}

// bool RiffChunk::validate_ckid(const std::array<uint8_t, 4>& ckid) {
//     uint32_t id = (((uint32_t) ckid[0]) << 24)
//                 + (((uint32_t) ckid[1]) << 16)
//                 + (((uint32_t) ckid[2]) << 8)
//                 + (((uint32_t) ckid[3]));
//     return validate_ckid(id);
// }

// --=== Check that cksize matches data portion ===--
bool RiffChunk::validate_cksize() {
    return (get_cksize() + 8) == _chunk.length();
}

// --=== Getters ===--
uint32_t RiffChunk::get_ckid() {
    if ( _chunk.length() < 4 )
        throw std::length_error("[RiffChunk::get_ckid] Could not retrieve ckid as chunk data is too short!");

    // Retrieve based on contiguous storage of strings
    return *((uint32_t*) &(_chunk[0]));
}

uint32_t RiffChunk::get_ckid(const std::string& chunk, size_t position) {
    return RiffChunk(chunk.substr(position, 4)).get_ckid();
}

uint32_t RiffChunk::get_cksize() {
    if ( _chunk.length() < 8 )
        throw std::length_error("[RiffChunk::get_cksize] Could not retrieve cksize as chunk data is too short!");

    // Retrieve based on contiguous storage of strings
    return *((uint32_t*) &(_chunk[4]));
}

uint32_t RiffChunk::get_cksize(const std::string& chunk, size_t position) {
    return just_id_n_size(chunk, position).get_cksize();
}

uint32_t RiffChunk::get_cksize(std::istream& chunk_stream, std::streamoff off, std::ios_base::seekdir way) {
    return just_id_n_size(chunk_stream, off, way).get_cksize();
}

// --=== Setters ===--
void RiffChunk::set_ckid(uint32_t ckid) {
    if (_chunk.length() < 8)
        _chunk = std::string(8, (char) 0);
    *((uint32_t*) _chunk.data()) = ckid;
}

void RiffChunk::set_ckid(const std::string& ckid) {
    if ( ckid.length() != 4 )
        throw std::length_error("[RiffChunk::set_ckid] The ckid must be 4 bytes!!!");
    if ( _chunk.length() < 8 )
        _chunk = std::string(8, (char) 0);
    _chunk[0] = ckid[0];
    _chunk[1] = ckid[1];
    _chunk[2] = ckid[2];
    _chunk[3] = ckid[3];
}

void RiffChunk::set_cksize(uint32_t cksize) {
    if (_chunk.length() < 8)
        _chunk = std::string(8, (char) 0);
    *((uint32_t*) &(_chunk.data()[4])) = cksize;
}

// --=== Data handlers ===--
void RiffChunk::overwrite_data(const std::string& data) {
    if ( _chunk.length() < 8 )
        throw std::length_error("[RiffChunk::overwrite_data] Not enough info for ckid or cksize...");

    // Clear data
    _chunk = _chunk.substr(0, 8);

    // Append data
    _chunk.append(data);

    // Update cksize
    set_cksize(data.length());
}

void RiffChunk::append_data(const std::string& data) {
    if ( _chunk.length() < 8 )
        throw std::length_error("[RiffChunk::append_data] Not enough info for ckid or cksize...");

    // Calculate instead of using cksize incase it is incorrectly set...
    uint32_t size = (_chunk.length() - 8) + data.length();

    _chunk.append(data);

    set_cksize(size);
}


// --=== Get raw (serialized) chunk ===--
const std::string& RiffChunk::get_raw() {
    return _chunk;
}

std::string RiffChunk::get_data() {
    return _chunk.substr(8, _chunk.length() - 8);
}
