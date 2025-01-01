/******************************************************************************
 * File Name    : compressionalgorithms.h
 * Coder        : Aziz Gökhan NARİN
 * E-Mail       : azizgokhannarin@yahoo.com
 * Explanation  : Compression Algorithms Tool
 * Versiyon     : 1.0.0
 ******************************************************************************/

#ifndef COMPRESSIONALGORITHMS_H
#define COMPRESSIONALGORITHMS_H

#include <vector>
#include <string>
#include <functional>

class CompressionAlgorithms
{
public:
    CompressionAlgorithms();
    ~CompressionAlgorithms();

    bool compressFileWithLZMA2(const std::string &inputFileName, const std::string &outputFileName);
    bool decompressFileWithLZMA2(const std::string &inputFileName, const std::string &outputFileName);

    bool compressFileWithLZ77(const std::string &inputFileName, const std::string &outputFileName);
    bool decompressFileWithLZ77(const std::string &inputFileName, const std::string &outputFileName);

    bool compressFileWithLZ78(const std::string &inputFileName, const std::string &outputFileName);
    bool decompressFileWithLZ78(const std::string &inputFileName, const std::string &outputFileName);

    bool compressFileWithLZW(const std::string &inputFileName, const std::string &outputFileName);
    bool decompressFileWithLZW(const std::string &inputFileName, const std::string &outputFileName);

protected:
    static std::vector<uint8_t> compressWithLZMA2(const std::vector<uint8_t> &data);
    static std::vector<uint8_t> decompressWithLZMA2(const std::vector<uint8_t> &compressedData);

    static std::vector<uint8_t> compressWithLZ77(const std::vector<uint8_t> &data);
    static std::vector<uint8_t> decompressWithLZ77(const std::vector<uint8_t> &compressedData);

    static std::vector<uint8_t> compressWithLZ78(const std::vector<uint8_t> &data);
    static std::vector<uint8_t> decompressWithLZ78(const std::vector<uint8_t> &compressedData);

    static std::vector<uint8_t> compressWithLZW(const std::vector<uint8_t> &data);
    static std::vector<uint8_t> decompressWithLZW(const std::vector<uint8_t> &compressedData);

private:
    bool compressFile(const std::string &inputFileName, const std::string &outputFileName,
                      std::function<std::vector<uint8_t>(const std::vector<uint8_t> &)> compressAlgorithm);
    bool decompressFile(const std::string &inputFileName, const std::string &outputFileName,
                        std::function<std::vector<uint8_t>(const std::vector<uint8_t> &)> decompressAlgorithm);
};

#endif // COMPRESSIONALGORITHMS_H
