/******************************************************************************
 * File Name    : compressionalgorithms.cpp
 * Coder        : Aziz Gökhan NARİN
 * E-Mail       : azizgokhannarin@yahoo.com
 * Explanation  : Compression Algorithms Tool
 * Versiyon     : 1.0.0
 ******************************************************************************/

#include "compressionalgorithms.h"

#include <lzma.h>
#include <locale>
#include <stdexcept>
#include <fstream>
#include <stdexcept>
#include <map>
#include <vector>
#include <cstdint>
#include <iostream>

struct LZ77Token {
    uint16_t offset;
    uint16_t length;
    uint8_t nextChar;
};

CompressionAlgorithms::CompressionAlgorithms()
{

}

CompressionAlgorithms::~CompressionAlgorithms()
{

}

bool CompressionAlgorithms::compressFile(const std::string &inputFileName, const std::string &outputFileName,
        std::function<std::vector<uint8_t>(const std::vector<uint8_t> &data)> compressAlgorithm)
{
    std::ifstream inputFile(inputFileName, std::ios::binary);

    if (!inputFile) {
        return false;
    }

    std::vector<uint8_t> inputData((std::istreambuf_iterator<char>(inputFile)),
                                   std::istreambuf_iterator<char>());
    inputFile.close();

    try {
        std::vector<uint8_t> compressedData = compressAlgorithm(inputData);
        std::ofstream outputFile(outputFileName, std::ios::binary);

        if (!outputFile) {
            return false;
        }

        outputFile.write(reinterpret_cast<char *>(compressedData.data()), compressedData.size());
        outputFile.close();
    } catch (const std::exception &e) {
        return false;
    }

    return true;
}

bool CompressionAlgorithms::decompressFile(const std::string &inputFileName, const std::string &outputFileName,
        std::function<std::vector<uint8_t>(const std::vector<uint8_t> &data)> decompressAlgorithm)
{
    std::ifstream inputFile(inputFileName, std::ios::binary);

    if (!inputFile) {
        return false;
    }

    std::vector<uint8_t> compressedData((std::istreambuf_iterator<char>(inputFile)),
                                        std::istreambuf_iterator<char>());
    inputFile.close();

    try {
        std::vector<uint8_t> rawData = decompressAlgorithm(compressedData);
        std::ofstream outputFile(outputFileName, std::ios::binary);

        if (!outputFile) {
            return false;
        }

        outputFile.write(reinterpret_cast<char *>(rawData.data()), rawData.size());
        outputFile.close();
    } catch (const std::exception &e) {
        return false;
    }

    return true;
}

std::vector<uint8_t> CompressionAlgorithms::compressWithLZMA2(const std::vector<uint8_t> &data)
{
    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_ret ret = lzma_easy_encoder(&strm, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
    if (ret != LZMA_OK) {
        throw std::runtime_error("Compression could not start!");
    }

    strm.next_in = data.data();
    strm.avail_in = data.size();

    std::vector<uint8_t> compressedData;
    compressedData.resize(data.size() + data.size() / 3 + 128);

    strm.next_out = compressedData.data();
    strm.avail_out = compressedData.size();

    ret = lzma_code(&strm, LZMA_FINISH);
    if (ret != LZMA_STREAM_END) {
        lzma_end(&strm);
        throw std::runtime_error("Compression is failed!");
    }

    compressedData.resize(compressedData.size() - strm.avail_out);
    lzma_end(&strm);
    return compressedData;
}

std::vector<uint8_t> CompressionAlgorithms::decompressWithLZMA2(const std::vector<uint8_t>
        &compressedData)
{
    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_ret ret = lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED);

    if (ret != LZMA_OK) {
        throw std::runtime_error("Decompression could not start!");
    }

    strm.next_in = compressedData.data();
    strm.avail_in = compressedData.size();

    std::vector<uint8_t> rawData;
    rawData.resize(compressedData.size() * 3);

    strm.next_out = rawData.data();
    strm.avail_out = rawData.size();

    ret = lzma_code(&strm, LZMA_FINISH);
    if (ret != LZMA_STREAM_END) {
        lzma_end(&strm);
        throw std::runtime_error("Decompression is failed!");
    }

    rawData.resize(rawData.size() - strm.avail_out);
    lzma_end(&strm);
    return rawData;
}

std::vector<uint8_t> CompressionAlgorithms::compressWithLZ77(const std::vector<uint8_t> &data)
{
    const size_t WINDOW_SIZE = 4096;
    const size_t LOOKAHEAD_BUFFER_SIZE = 18;

    size_t dataSize = data.size();
    size_t pos = 0;

    std::vector<uint8_t> compressedData;
    compressedData.reserve(dataSize);

    while (pos < dataSize) {
        uint16_t bestOffset = 0;
        uint16_t bestLength = 0;

        size_t startWindow = (pos >= WINDOW_SIZE) ? pos - WINDOW_SIZE : 0;
        size_t endWindow = pos;

        for (size_t i = startWindow; i < endWindow; ++i) {
            size_t matchLength = 0;
            while (matchLength < LOOKAHEAD_BUFFER_SIZE &&
                   (pos + matchLength) < dataSize &&
                   (i + matchLength) < dataSize &&
                   data[i + matchLength] == data[pos + matchLength]) {
                matchLength++;
            }

            if (matchLength > bestLength) {
                bestLength = static_cast<uint16_t>(matchLength);
                bestOffset = static_cast<uint16_t>(pos - i);
            }
        }

        uint8_t nextChar = (pos + bestLength < dataSize)
                           ? data[pos + bestLength]
                           : 0;

        LZ77Token token = { bestOffset, bestLength, nextChar };
        compressedData.push_back(static_cast<uint8_t>(token.offset >> 8));
        compressedData.push_back(static_cast<uint8_t>(token.offset & 0xFF));
        compressedData.push_back(static_cast<uint8_t>(token.length));
        compressedData.push_back(token.nextChar);
        pos += bestLength + 1;
    }

    return compressedData;
}

std::vector<uint8_t> CompressionAlgorithms::decompressWithLZ77(const std::vector<uint8_t> &compressedData)
{
    size_t pos = 0;
    size_t dataSize = compressedData.size();
    std::vector<uint8_t> data;
    data.reserve(dataSize * 2);

    while (pos + 4 <= dataSize) {
        uint16_t offset = (static_cast<uint16_t>(compressedData[pos]) << 8)
                          | static_cast<uint16_t>(compressedData[pos + 1]);
        uint16_t length = compressedData[pos + 2];
        uint8_t nextChar = compressedData[pos + 3];
        pos += 4;

        if (offset > data.size()) {
            offset = static_cast<uint16_t>(data.size());
        }

        size_t start = data.size() - offset;

        for (uint16_t i = 0; i < length; ++i) {
            if ((start + i) < data.size()) {
                data.push_back(data[start + i]);
            } else {
                break;
            }
        }

        data.push_back(nextChar);
    }

    return data;
}

std::vector<uint8_t> CompressionAlgorithms::compressWithLZ78(const std::vector<uint8_t> &data)
{
    std::map<std::vector<uint8_t>, uint16_t> dictionary;
    uint16_t dictSize = 1;

    std::vector<uint8_t> compressedData;
    compressedData.reserve(data.size() * 3);

    std::vector<uint8_t> w;
    for (size_t i = 0; i < data.size(); ++i) {
        uint8_t c = data[i];
        std::vector<uint8_t> wc = w;
        wc.push_back(c);

        auto it = dictionary.find(wc);
        if (it != dictionary.end()) {
            w = wc;
        } else {
            uint16_t index = 0;
            if (!w.empty()) {
                index = dictionary[w];
            }

            compressedData.push_back(static_cast<uint8_t>(index >> 8));
            compressedData.push_back(static_cast<uint8_t>(index & 0xFF));
            compressedData.push_back(c);

            dictionary[wc] = dictSize++;
            w.clear();
        }
    }

    if (!w.empty()) {
        uint16_t index = dictionary[w];
        compressedData.push_back(static_cast<uint8_t>(index >> 8));
        compressedData.push_back(static_cast<uint8_t>(index & 0xFF));
        compressedData.push_back(0);
    }

    return compressedData;
}

std::vector<uint8_t> CompressionAlgorithms::decompressWithLZ78(const std::vector<uint8_t> &compressedData)
{
    std::vector<std::vector<uint8_t>> dictionary;
    dictionary.push_back(std::vector<uint8_t>());

    std::vector<uint8_t> decompressedData;
    size_t position = 0;

    while (position + 3 <= compressedData.size()) {
        uint16_t index = (static_cast<uint16_t>(compressedData[position]) << 8)
                         | static_cast<uint16_t>(compressedData[position + 1]);
        position += 2;

        uint8_t c = compressedData[position];
        position++;

        std::vector<uint8_t> entry;
        if (index < dictionary.size()) {
            entry = dictionary[index];
        } else {
            break;
        }

        if (c != 0) {
            entry.push_back(c);
        }

        decompressedData.insert(decompressedData.end(), entry.begin(), entry.end());
        dictionary.push_back(entry);
    }

    return decompressedData;
}

std::vector<uint8_t> CompressionAlgorithms::compressWithLZW(const std::vector<uint8_t> &data)
{
    std::map<std::vector<uint8_t>, uint16_t> dictionary;
    uint16_t dictSize = 256;

    for (uint16_t i = 0; i < 256; ++i) {
        dictionary[ {static_cast<uint8_t>(i)}] = i;
    }

    std::vector<uint8_t> compressedData;
    compressedData.reserve(data.size() * 2);

    if (data.empty()) {
        return compressedData;
    }

    std::vector<uint8_t> w;
    w.push_back(data[0]);

    for (size_t i = 1; i < data.size(); ++i) {
        uint8_t k = data[i];
        std::vector<uint8_t> wk = w;
        wk.push_back(k);

        auto it = dictionary.find(wk);

        if (it != dictionary.end()) {
            w = wk;
        } else {
            uint16_t code = dictionary[w];
            compressedData.push_back(static_cast<uint8_t>(code >> 8));
            compressedData.push_back(static_cast<uint8_t>(code & 0xFF));

            dictionary[wk] = dictSize++;

            w.clear();
            w.push_back(k);
        }
    }

    if (!w.empty()) {
        uint16_t code = dictionary[w];
        compressedData.push_back(static_cast<uint8_t>(code >> 8));
        compressedData.push_back(static_cast<uint8_t>(code & 0xFF));
    }

    return compressedData;
}

std::vector<uint8_t> CompressionAlgorithms::decompressWithLZW(const std::vector<uint8_t> &compressedData)
{
    std::vector<std::vector<uint8_t>> dictionary(256);
    for (uint16_t i = 0; i < 256; ++i) {
        dictionary[i] = { static_cast<uint8_t>(i) };
    }

    std::vector<uint8_t> data;
    data.reserve(compressedData.size() * 3);

    size_t pos = 0;
    size_t dataSize = compressedData.size();

    if (dataSize < 2) {
        return data;
    }

    uint16_t code = (static_cast<uint16_t>(compressedData[pos]) << 8)
                    | static_cast<uint16_t>(compressedData[pos + 1]);
    pos += 2;

    if (code >= dictionary.size()) {
        std::cerr << "Failed: Invalid code (First code)." << std::endl;
        return data;
    }

    std::vector<uint8_t> w = dictionary[code];
    data.insert(data.end(), w.begin(), w.end());

    while (pos + 2 <= dataSize) {
        code = (static_cast<uint16_t>(compressedData[pos]) << 8)
               | static_cast<uint16_t>(compressedData[pos + 1]);
        pos += 2;

        std::vector<uint8_t> entry;
        if (code < dictionary.size()) {
            entry = dictionary[code];
        } else if (code == dictionary.size()) {
            entry = w;
            entry.push_back(w[0]);
        } else {
            std::cerr << "Failed: Invalid code." << std::endl;
            break;
        }

        data.insert(data.end(), entry.begin(), entry.end());

        std::vector<uint8_t> newEntry = w;
        newEntry.push_back(entry[0]);
        dictionary.push_back(newEntry);
        w = entry;
    }

    return data;
}

bool CompressionAlgorithms::compressFileWithLZMA2(const std::string &inputFileName, const std::string &outputFileName)
{
    return compressFile(inputFileName, outputFileName, &CompressionAlgorithms::compressWithLZMA2);
}

bool CompressionAlgorithms::decompressFileWithLZMA2(const std::string &inputFileName, const std::string &outputFileName)
{
    return decompressFile(inputFileName, outputFileName, &CompressionAlgorithms::decompressWithLZMA2);
}

bool CompressionAlgorithms::compressFileWithLZ77(const std::string &inputFileName, const std::string &outputFileName)
{
    return compressFile(inputFileName, outputFileName, &CompressionAlgorithms::compressWithLZ77);
}

bool CompressionAlgorithms::decompressFileWithLZ77(const std::string &inputFileName, const std::string &outputFileName)
{
    return decompressFile(inputFileName, outputFileName, &CompressionAlgorithms::decompressWithLZ77);
}

bool CompressionAlgorithms::compressFileWithLZ78(const std::string &inputFileName, const std::string &outputFileName)
{
    return compressFile(inputFileName, outputFileName, &CompressionAlgorithms::compressWithLZ78);
}

bool CompressionAlgorithms::decompressFileWithLZ78(const std::string &inputFileName, const std::string &outputFileName)
{
    return decompressFile(inputFileName, outputFileName, &CompressionAlgorithms::decompressWithLZ78);
}

bool CompressionAlgorithms::compressFileWithLZW(const std::string &inputFileName, const std::string &outputFileName)
{
    return compressFile(inputFileName, outputFileName, &CompressionAlgorithms::compressWithLZW);
}

bool CompressionAlgorithms::decompressFileWithLZW(const std::string &inputFileName, const std::string &outputFileName)
{
    return decompressFile(inputFileName, outputFileName, &CompressionAlgorithms::decompressWithLZW);
}
