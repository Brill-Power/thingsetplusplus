#include <fstream>
#include <iostream>
#include <thingset++/StreamingThingSetBinaryDecoder.hpp>

using namespace ThingSet;
using namespace std;

#define FILE_CHUNK_SIZE 251

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
    EepromHeader _header;

public:
    FileStreamingThingSetBinaryDecoder(char *path) : StreamingThingSetBinaryDecoder(1), _file(), _offset(0)
    {
        _file.open(path, ios::in | ios::binary);
        _file.read(reinterpret_cast<char *>(&_header), sizeof(_header));
        read(0, FILE_CHUNK_SIZE * 2);
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
        _file.read(reinterpret_cast<char *>(&_buffer[pos]), chunk);
        _offset += chunk;
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
    return 0;
}