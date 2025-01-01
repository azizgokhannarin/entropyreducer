/******************************************************************************
 * File Name    : transformationalgorithms.h
 * Coder        : Aziz Gökhan NARİN
 * E-Mail       : azizgokhannarin@yahoo.com
 * Explanation  : Data Transformation Algorithms Tool
 * Versiyon     : 1.0.0
 ******************************************************************************/

#include "transformationalgorithms.h"

#include <bitset>
#include <iostream>
#include <fstream>

struct pair_hash {
    std::size_t operator()(const std::pair<uint8_t, uint8_t> &pair) const
    {
        return std::hash<uint8_t>()(pair.first) ^ (std::hash<uint8_t>()(pair.second) << 1);
    }
};

TransformationAlgorithms::TransformationAlgorithms()
{

}

TransformationAlgorithms::~TransformationAlgorithms()
{

}

bool TransformationAlgorithms::encodeFile(const std::string &inputFileName, const std::string &outputFileName,
        std::function<void(std::vector<uint8_t> &data)> encodeAlgorithm)
{
    std::ifstream inputFile(inputFileName, std::ios::binary);

    if (!inputFile) {
        return false;
    }

    std::vector<uint8_t> inputData((std::istreambuf_iterator<char>(inputFile)),
                                   std::istreambuf_iterator<char>());
    inputFile.close();

    try {
        encodeAlgorithm(inputData);
        std::ofstream outputFile(outputFileName, std::ios::binary);

        if (!outputFile) {
            return false;
        }

        outputFile.write(reinterpret_cast<char *>(inputData.data()), inputData.size());
        outputFile.close();
    } catch (const std::exception &e) {
        return false;
    }

    return true;
}

bool TransformationAlgorithms::decodeFile(const std::string &inputFileName, const std::string &outputFileName,
        std::function<void(std::vector<uint8_t> &data)> decodeAlgorithm)
{
    std::ifstream inputFile(inputFileName, std::ios::binary);

    if (!inputFile) {
        return false;
    }

    std::vector<uint8_t> encodedData((std::istreambuf_iterator<char>(inputFile)),
                                     std::istreambuf_iterator<char>());
    inputFile.close();

    try {
        decodeAlgorithm(encodedData);
        std::ofstream outputFile(outputFileName, std::ios::binary);

        if (!outputFile) {
            return false;
        }

        outputFile.write(reinterpret_cast<char *>(encodedData.data()), encodedData.size());
        outputFile.close();
    } catch (const std::exception &e) {
        return false;
    }

    return true;
}

void TransformationAlgorithms::encodeWithBWT(std::vector<uint8_t> &data)
{
    std::vector<uint8_t> encodedData;
    size_t len = data.size();

    if (len == 0) {
        return encodedData.swap(data);
    }

    std::vector<size_t> indices(len);

    for (size_t i = 0; i < len; ++i) {
        indices[i] = i;
    }

    auto compare = [&data, len](size_t a, size_t b) {
        for (size_t i = 0; i < len; ++i) {
            uint8_t ac = data[(a + i) % len];
            uint8_t bc = data[(b + i) % len];

            if (ac != bc) {
                return ac < bc;
            }
        }

        return false;
    };

    std::sort(indices.begin(), indices.end(), compare);

    encodedData.resize(len + sizeof(uint32_t));
    uint32_t originalIndex = 0;

    for (size_t i = 0; i < len; ++i) {
        size_t idx = (indices[i] + len - 1) % len;
        encodedData[sizeof(uint32_t) + i] = data[idx];

        if (indices[i] == 0) {
            originalIndex = static_cast<uint32_t>(i);
        }
    }

    encodedData[0] = static_cast<uint8_t>(originalIndex & 0xFF);
    encodedData[1] = static_cast<uint8_t>((originalIndex >> 8) & 0xFF);
    encodedData[2] = static_cast<uint8_t>((originalIndex >> 16) & 0xFF);
    encodedData[3] = static_cast<uint8_t>((originalIndex >> 24) & 0xFF);

    data.swap(encodedData);
}

void TransformationAlgorithms::decodeWithBWT(std::vector<uint8_t> &encodedData)
{
    std::vector<uint8_t> data;
    size_t totalLen = encodedData.size();

    if (totalLen <= sizeof(uint32_t)) {
        return;
    }

    size_t len = totalLen - sizeof(uint32_t);

    uint32_t originalIndex = static_cast<uint32_t>(encodedData[0])
                             | (static_cast<uint32_t>(encodedData[1]) << 8)
                             | (static_cast<uint32_t>(encodedData[2]) << 16)
                             | (static_cast<uint32_t>(encodedData[3]) << 24);

    const uint8_t *lastColumn = &encodedData[sizeof(uint32_t)];

    std::vector<int> count(256, 0);

    for (size_t i = 0; i < len; ++i) {
        count[lastColumn[i]]++;
    }

    std::vector<int> cumulativeCount(256, 0);
    cumulativeCount[0] = 0;

    for (int i = 1; i < 256; ++i) {
        cumulativeCount[i] = cumulativeCount[i - 1] + count[i - 1];
    }

    std::vector<int> tally(256, 0);
    std::vector<int> LF(len);

    for (size_t i = 0; i < len; ++i) {
        uint8_t ch = lastColumn[i];
        LF[i] = cumulativeCount[ch] + tally[ch];
        tally[ch]++;
    }

    data.resize(len);
    int idx = originalIndex;

    for (size_t i = 0; i < len; ++i) {
        data[len - i - 1] = lastColumn[idx];
        idx = LF[idx];
    }

    encodedData.swap(data);
}

void TransformationAlgorithms::encodeWithDelta(std::vector<uint8_t> &data)
{
    uint8_t previous = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        uint8_t current = data[i];
        data[i] = current - previous;
        previous = current;
    }
}

void TransformationAlgorithms::decodeWithDelta(std::vector<uint8_t> &encodedData)
{
    uint8_t previous = 0;

    for (size_t i = 0; i < encodedData.size(); ++i) {
        encodedData[i] = encodedData[i] + previous;
        previous = encodedData[i];
    }
}

void TransformationAlgorithms::encodeWithCube(std::vector<uint8_t> &data)
{
    size_t dataSize = data.size();
    std::vector<uint8_t> encodedData(dataSize);

    for (size_t i = 0; i + 64 <= dataSize; i += 64) {
        for (size_t j = 0; j < 64; ++j) {
            size_t targetIndex = i + ((j + 1) % 64);
            encodedData[targetIndex] = data[i + j];
        }
    }

    data = encodedData;
}

void TransformationAlgorithms::decodeWithCube(std::vector<uint8_t> &encodedData)
{
    size_t dataSize = encodedData.size();
    std::vector<uint8_t> decodedData(dataSize);

    for (size_t i = 0; i + 64 <= dataSize; i += 64) {
        for (size_t j = 0; j < 64; ++j) {
            size_t targetIndex = i + ((j + 1) % 64);
            decodedData[i + j] = encodedData[targetIndex];
        }
    }

    encodedData = decodedData;
}

void TransformationAlgorithms::encodeWithComplement(std::vector<uint8_t> &data)
{
    const size_t keySize = 1024;
    std::vector<uint8_t> key(keySize, 0);

    for (size_t i = 0; i < keySize; ++i) {
        std::unordered_map<uint8_t, size_t> frequency;

        for (size_t j = i; j < data.size(); j += keySize) {
            frequency[data[j]]++;
        }

        uint8_t mostFrequentByte = 0;
        size_t maxFrequency = 0;

        for (const auto& [byteValue, count] : frequency) {
            if (count > maxFrequency) {
                mostFrequentByte = byteValue;
                maxFrequency = count;
            }
        }

        key[i] = mostFrequentByte;
    }

    for (size_t i = 0; i < data.size(); ++i) {
        data[i] ^= key[i % keySize];
    }

    data.insert(data.end(), key.begin(), key.end());
}

void TransformationAlgorithms::decodeWithComplement(std::vector<uint8_t> &encodedData)
{
    const size_t keySize = 1024;

    if (encodedData.size() < keySize) {
        std::cerr << "Data length is less than key" << std::endl;
        return;
    }

    std::vector<uint8_t> key(encodedData.end() - keySize, encodedData.end());
    encodedData.erase(encodedData.end() - keySize, encodedData.end());

    for (size_t i = 0; i < encodedData.size(); ++i) {
        encodedData[i] ^= key[i % keySize];
    }
}

void TransformationAlgorithms::encodeWithBlockSort(std::vector<uint8_t> &data)
{
    std::vector<uint8_t> encodedData;
    const size_t blockSize = 256;
    size_t dataSize = data.size();
    size_t numFullBlocks = dataSize / blockSize;
    size_t remainingBytes = dataSize % blockSize;

    size_t dataIndex = 0;

    for (size_t blockIndex = 0; blockIndex < numFullBlocks; ++blockIndex) {
        std::vector<uint8_t> block(data.begin() + dataIndex, data.begin() + dataIndex + blockSize);
        std::vector<uint8_t> sortedBlock = block;
        std::sort(sortedBlock.begin(), sortedBlock.end());
        std::bitset<256> bitmask;
        std::vector<uint8_t> changedValues;

        for (size_t i = 0; i < blockSize; ++i) {
            if (block[i] != sortedBlock[i]) {
                bitmask.set(i);
                changedValues.push_back(block[i]);
            }
        }

        encodedData.insert(encodedData.end(), sortedBlock.begin(), sortedBlock.end());

        for (size_t i = 0; i < 32; ++i) {
            uint8_t byte = 0;

            for (size_t bit = 0; bit < 8; ++bit) {
                if (bitmask.test(i * 8 + bit)) {
                    byte |= (1 << bit);
                }
            }

            encodedData.push_back(byte);
        }

        encodedData.insert(encodedData.end(), changedValues.begin(), changedValues.end());

        dataIndex += blockSize;
    }

    if (remainingBytes > 0) {
        encodedData.insert(encodedData.end(), data.begin() + dataIndex, data.end());
    }

    data.swap(encodedData);
}

void TransformationAlgorithms::decodeWithBlockSort(std::vector<uint8_t> &encodedData)
{
    std::vector<uint8_t> decodedData;
    const size_t blockSize = 256;
    size_t dataIndex = 0;
    size_t dataSize = encodedData.size();

    while (dataIndex + blockSize + 32 <= dataSize) {
        std::vector<uint8_t> sortedBlock(encodedData.begin() + dataIndex, encodedData.begin() + dataIndex + blockSize);
        dataIndex += blockSize;
        std::bitset<256> bitmask;

        for (size_t i = 0; i < 32; ++i) {
            if (dataIndex >= dataSize) {
                std::cerr << "Data format is wrong!" << std::endl;
                return;
            }

            uint8_t byte = encodedData[dataIndex++];

            for (size_t bit = 0; bit < 8; ++bit) {
                if (byte & (1 << bit)) {
                    bitmask.set(i * 8 + bit);
                }
            }
        }

        size_t numChanges = bitmask.count();

        if (dataIndex + numChanges > dataSize) {
            std::cerr << "Data format is wrong!" << std::endl;
            return;
        }

        std::vector<uint8_t> changedValues(encodedData.begin() + dataIndex, encodedData.begin() + dataIndex + numChanges);
        dataIndex += numChanges;

        std::vector<uint8_t> originalBlock = sortedBlock;

        size_t changeIndex = 0;

        for (size_t i = 0; i < blockSize; ++i) {
            if (bitmask.test(i)) {
                originalBlock[i] = changedValues[changeIndex++];
            }
        }

        decodedData.insert(decodedData.end(), originalBlock.begin(), originalBlock.end());
    }

    if (dataIndex < dataSize) {
        decodedData.insert(decodedData.end(), encodedData.begin() + dataIndex, encodedData.end());
    }

    encodedData.swap(decodedData);
}

void TransformationAlgorithms::encodeWithPB(std::vector<uint8_t> &data)
{
    std::unordered_map<std::pair<uint8_t, uint8_t>, size_t, pair_hash> frequencies;

    size_t dataSize = data.size();
    size_t pairCount = dataSize / 2;

    for (size_t i = 0; i < pairCount * 2; i += 2) {
        uint8_t first = data[i];
        uint8_t second = data[i + 1];
        frequencies[ {first, second}]++;
    }

    std::vector<std::pair<std::pair<uint8_t, uint8_t>, size_t>> freqVec(frequencies.begin(), frequencies.end());
    std::sort(freqVec.begin(), freqVec.end(),
    [](const auto & a, const auto & b) { return a.second > b.second; });

    std::unordered_map<std::pair<uint8_t, uint8_t>, std::pair<uint8_t, uint8_t>, pair_hash> encodeMap;

    uint8_t replacementByte = 0;
    size_t selectedPairs = 0;

    for (const auto &item : freqVec) {
        if (selectedPairs >= 256) break;
        const auto &pair = item.first;
        if (pair.first != pair.second) {
            encodeMap[pair] = {replacementByte, replacementByte};
            encodeMap[ {replacementByte, replacementByte}] = pair;
            replacementByte++;
            selectedPairs++;
        }
    }

    for (size_t i = 0; i < pairCount * 2; i += 2) {
        std::pair<uint8_t, uint8_t> currentPair = {data[i], data[i + 1]};
        if (encodeMap.find(currentPair) != encodeMap.end()) {
            auto encodedPair = encodeMap[currentPair];
            data[i] = encodedPair.first;
            data[i + 1] = encodedPair.second;
        }
    }

    for (size_t i = 0; i < 256; ++i) {
        auto it = encodeMap.find({i, i});

        if (it != encodeMap.end()) {
            data.push_back(it->second.first);
            data.push_back(it->second.second);
        } else {
            data.push_back(0);
            data.push_back(0);
        }
    }
}

void TransformationAlgorithms::decodeWithPB(std::vector<uint8_t> &encodedData)
{
    if (encodedData.size() < 512) {
        std::cerr << "Encoded data is too short to contain mapping." << std::endl;
        return;
    }

    size_t dataSize = encodedData.size() - 512;
    std::vector<std::pair<uint8_t, uint8_t>> mapping(256);

    for (size_t i = 0; i < 256; ++i) {
        size_t index = dataSize + i * 2;
        mapping[i] = {encodedData[index], encodedData[index + 1]};
    }

    encodedData.resize(dataSize);

    std::unordered_map<std::pair<uint8_t, uint8_t>, std::pair<uint8_t, uint8_t>, pair_hash> decodeMap;

    for (size_t i = 0; i < 256; ++i) {
        std::pair<uint8_t, uint8_t> encodedPair = {i, i};
        std::pair<uint8_t, uint8_t> originalPair = mapping[i];
        if (originalPair.first != 0 || originalPair.second != 0) {
            decodeMap[encodedPair] = originalPair;
            decodeMap[originalPair] = encodedPair;
        }
    }

    size_t pairCount = dataSize / 2;
    for (size_t i = 0; i < pairCount * 2; i += 2) {
        std::pair<uint8_t, uint8_t> currentPair = {encodedData[i], encodedData[i + 1]};
        if (decodeMap.find(currentPair) != decodeMap.end()) {
            auto decodedPair = decodeMap[currentPair];
            encodedData[i] = decodedPair.first;
            encodedData[i + 1] = decodedPair.second;
        }
    }
}

void TransformationAlgorithms::encodeWithMTF(std::vector<uint8_t> &data)
{
    std::vector<uint8_t> dict(256);

    for (int i = 0; i < 256; i++) {
        dict[i] = static_cast<uint8_t>(i);
    }

    std::vector<uint8_t> encoded;
    encoded.reserve(data.size());

    for (uint8_t symbol : data) {
        auto it = std::find(dict.begin(), dict.end(), symbol);
        int index = static_cast<int>(std::distance(dict.begin(), it));

        encoded.push_back(static_cast<uint8_t>(index));

        dict.erase(dict.begin() + index);
        dict.insert(dict.begin(), symbol);
    }

    data = std::move(encoded);
}

void TransformationAlgorithms::decodeWithMTF(std::vector<uint8_t> &encodedData)
{
    std::vector<uint8_t> dict(256);

    for (int i = 0; i < 256; i++) {
        dict[i] = static_cast<uint8_t>(i);
    }

    std::vector<uint8_t> decoded;
    decoded.reserve(encodedData.size());

    for (uint8_t index : encodedData) {
        uint8_t symbol = dict[index];
        decoded.push_back(symbol);
        dict.erase(dict.begin() + index);
        dict.insert(dict.begin(), symbol);
    }

    encodedData = std::move(decoded);
}

void TransformationAlgorithms::encodeWithRLE(std::vector<uint8_t> &data)
{
    if (data.empty()) {
        return;
    }

    std::vector<uint8_t> encoded;
    encoded.reserve(data.size());

    uint8_t count = 1;
    uint8_t currentSymbol = data[0];

    for (size_t i = 1; i < data.size(); i++) {
        if (data[i] == currentSymbol && count < 255) {
            count++;
        } else {
            encoded.push_back(count);
            encoded.push_back(currentSymbol);
            currentSymbol = data[i];
            count = 1;
        }
    }

    encoded.push_back(count);
    encoded.push_back(currentSymbol);
    data = std::move(encoded);
}

void TransformationAlgorithms::decodeWithRLE(std::vector<uint8_t> &encodedData)
{
    std::vector<uint8_t> decoded;

    for (size_t i = 0; i + 1 < encodedData.size(); i += 2) {
        uint8_t count = encodedData[i];
        uint8_t symbol = encodedData[i + 1];

        for (int j = 0; j < count; j++) {
            decoded.push_back(symbol);
        }
    }

    encodedData = std::move(decoded);
}

bool TransformationAlgorithms::encodeFileWithBWT(const std::string &inputFileName, const std::string &outputFileName)
{
    return encodeFile(inputFileName, outputFileName, &TransformationAlgorithms::encodeWithBWT);
}

bool TransformationAlgorithms::decodeFileWithBWT(const std::string &inputFileName, const std::string &outputFileName)
{
    return decodeFile(inputFileName, outputFileName, &TransformationAlgorithms::decodeWithBWT);
}

bool TransformationAlgorithms::encodeFileWithDelta(const std::string &inputFileName,
        const std::string &outputFileName)
{
    return encodeFile(inputFileName, outputFileName, &TransformationAlgorithms::encodeWithDelta);
}

bool TransformationAlgorithms::decodeFileWithDelta(const std::string &inputFileName,
        const std::string &outputFileName)
{
    return decodeFile(inputFileName, outputFileName, &TransformationAlgorithms::decodeWithDelta);
}

bool TransformationAlgorithms::encodeFileWithCube(const std::string &inputFileName,
        const std::string &outputFileName)
{
    return encodeFile(inputFileName, outputFileName, &TransformationAlgorithms::encodeWithCube);
}

bool TransformationAlgorithms::decodeFileWithCube(const std::string &inputFileName,
        const std::string &outputFileName)
{
    return decodeFile(inputFileName, outputFileName, &TransformationAlgorithms::decodeWithCube);
}

bool TransformationAlgorithms::encodeFileWithComplement(const std::string &inputFileName,
        const std::string &outputFileName)
{
    return encodeFile(inputFileName, outputFileName, &TransformationAlgorithms::encodeWithComplement);
}

bool TransformationAlgorithms::decodeFileWithComplement(const std::string &inputFileName,
        const std::string &outputFileName)
{
    return decodeFile(inputFileName, outputFileName, &TransformationAlgorithms::decodeWithComplement);
}

bool TransformationAlgorithms::encodeFileWithBlockSort(const std::string &inputFileName,
        const std::string &outputFileName)
{
    return encodeFile(inputFileName, outputFileName, &TransformationAlgorithms::encodeWithBlockSort);
}

bool TransformationAlgorithms::decodeFileWithBlockSort(const std::string &inputFileName,
        const std::string &outputFileName)
{
    return decodeFile(inputFileName, outputFileName, &TransformationAlgorithms::decodeWithBlockSort);
}

bool TransformationAlgorithms::encodeFileWithPB(const std::string &inputFileName, const std::string &outputFileName)
{
    return encodeFile(inputFileName, outputFileName, &TransformationAlgorithms::encodeWithPB);
}

bool TransformationAlgorithms::decodeFileWithPB(const std::string &inputFileName, const std::string &outputFileName)
{
    return decodeFile(inputFileName, outputFileName, &TransformationAlgorithms::decodeWithPB);
}

bool TransformationAlgorithms::encodeFileWithMTF(const std::string &inputFileName, const std::string &outputFileName)
{
    return encodeFile(inputFileName, outputFileName, &TransformationAlgorithms::encodeWithMTF);
}

bool TransformationAlgorithms::decodeFileWithMTF(const std::string &inputFileName, const std::string &outputFileName)
{
    return decodeFile(inputFileName, outputFileName, &TransformationAlgorithms::decodeWithMTF);
}

bool TransformationAlgorithms::encodeFileWithRLE(const std::string &inputFileName, const std::string &outputFileName)
{
    return encodeFile(inputFileName, outputFileName, &TransformationAlgorithms::encodeWithRLE);
}

bool TransformationAlgorithms::decodeFileWithRLE(const std::string &inputFileName, const std::string &outputFileName)
{
    return decodeFile(inputFileName, outputFileName, &TransformationAlgorithms::decodeWithRLE);
}
