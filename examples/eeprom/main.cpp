#include <fstream>
#include <iostream>
#include <thingset++/StreamingThingSetBinaryDecoder.hpp>

using namespace ThingSet;
using namespace std;

#define FILE_CHUNK_SIZE 251

static uint32_t crc32_ieee_update(uint32_t crc, const uint8_t *data, size_t len);

class FileStreamingThingSetBinaryDecoder : public StreamingThingSetBinaryDecoder<FILE_CHUNK_SIZE>
{
private:
    struct EepromHeader
    {
        uint16_t version;
        uint16_t data_len;
        uint32_t crc;
    } __packed;

    ifstream _file;
    off_t _offset;
    uint32_t _crc;
    EepromHeader _header;

public:
    FileStreamingThingSetBinaryDecoder(char *path) : StreamingThingSetBinaryDecoder(1), _file(), _offset(0), _crc(0)
    {
        _file.open(path, ios::in | ios::binary);
        _file.read(reinterpret_cast<char *>(&_header), sizeof(_header));
        _offset += sizeof(_header);
        read(0, FILE_CHUNK_SIZE * 2);
    }

    bool verify()
    {
        bool valid = _crc == _header.crc;
        if (valid) {
            printf("CRCs match: %x\n", _crc);
        }
        else {
            printf("CRC mismatch: expected %x, calculated %x\n", _header.crc, _crc);
        }
        return valid;
    }

protected:
    int read() override
    {
        return read(FILE_CHUNK_SIZE, FILE_CHUNK_SIZE);
    }

    int read(size_t pos, size_t maxSize)
    {
        size_t remaining = _header.data_len - (_offset - sizeof(_header));
        size_t chunk = MIN(remaining, maxSize);
        // printf("Reading chunk %zu at %lld; %zu bytes remaining; ", chunk, _offset, remaining - chunk);
        _file.read(reinterpret_cast<char *>(&_buffer[pos]), chunk);
        _offset += chunk;
        _crc = crc32_ieee_update(_crc, &_buffer[pos], chunk);
        // printf("CRC %x\n", _crc);
        return chunk;
    }
};

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        return -1;
    }

    FileStreamingThingSetBinaryDecoder decoder(argv[1]);
    decoder.decodeMap<uint16_t>([&](uint16_t &key) {
        auto type = decoder.peekType();
        printf("Key 0x%x; type %d", key, type);
        switch (type) {
            case ZCBOR_MAJOR_TYPE_LIST: {
                size_t elementCount = 0;
                if (!decoder.decodeList([&](size_t i) {
                        elementCount = i + 1;
                        return decoder.skip();
                    }))
                {
                    return false;
                }
                printf("; num elements: %zu\n", elementCount);
                return true;
            }
            case ZCBOR_MAJOR_TYPE_TSTR: {
                string str;
                if (decoder.decode(&str)) {
                    printf("; value %s\n", str.c_str());
                    return true;
                }
                return false;
            }
            case ZCBOR_MAJOR_TYPE_SIMPLE: {
                float f;
                bool b;
                if (decoder.decode(&f)) {
                    printf("; value %f\n", f);
                    return true;
                }
                else if (decoder.decode(&b)) {
                    printf("; value %s\n", b ? "true" : "false");
                    return true;
                }
                return false;
            }
            case ZCBOR_MAJOR_TYPE_PINT: {
                uint64_t t;
                if (decoder.decode(&t)) {
                    printf("; value %llu\n", t);
                    return true;
                }
                return false;
            }
            default:
                printf("\n");
                return decoder.skip();
        }
    });
    decoder.verify();
    return 0;
}

static uint32_t crc32_ieee_update(uint32_t crc, const uint8_t *data, size_t len)
{
    /* crc table generated from polynomial 0xedb88320 */
    static const uint32_t table[16] = {
        0x00000000U, 0x1db71064U, 0x3b6e20c8U, 0x26d930acU, 0x76dc4190U, 0x6b6b51f4U, 0x4db26158U, 0x5005713cU,
        0xedb88320U, 0xf00f9344U, 0xd6d6a3e8U, 0xcb61b38cU, 0x9b64c2b0U, 0x86d3d2d4U, 0xa00ae278U, 0xbdbdf21cU,
    };

    crc = ~crc;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        crc = (crc >> 4) ^ table[(crc ^ byte) & 0x0f];
        crc = (crc >> 4) ^ table[(crc ^ ((uint32_t)byte >> 4)) & 0x0f];
    }

    return (~crc);
}
