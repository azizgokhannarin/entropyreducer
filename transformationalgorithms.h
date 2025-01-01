/******************************************************************************
 * File Name    : transformationalgorithms.h
 * Coder        : Aziz Gökhan NARİN
 * E-Mail       : azizgokhannarin@yahoo.com
 * Explanation  : Data Transformation Algorithms Tool
 * Versiyon     : 1.0.0
 ******************************************************************************/

#ifndef TRANSFORMATIONALGORITHMS_H
#define TRANSFORMATIONALGORITHMS_H

#include <string>
#include <vector>
#include <functional>

class TransformationAlgorithms
{
public:
    TransformationAlgorithms();
    ~TransformationAlgorithms();

    bool encodeFileWithBWT(const std::string &inputFileName, const std::string &outputFileName);
    bool decodeFileWithBWT(const std::string &inputFileName, const std::string &outputFileName);

    bool encodeFileWithDelta(const std::string &inputFileName, const std::string &outputFileName);
    bool decodeFileWithDelta(const std::string &inputFileName, const std::string &outputFileName);

    bool encodeFileWithCube(const std::string &inputFileName, const std::string &outputFileName);
    bool decodeFileWithCube(const std::string &inputFileName, const std::string &outputFileName);

    bool encodeFileWithComplement(const std::string &inputFileName, const std::string &outputFileName);
    bool decodeFileWithComplement(const std::string &inputFileName, const std::string &outputFileName);

    bool encodeFileWithBlockSort(const std::string &inputFileName, const std::string &outputFileName);
    bool decodeFileWithBlockSort(const std::string &inputFileName, const std::string &outputFileName);

    bool encodeFileWithPB(const std::string &inputFileName, const std::string &outputFileName);
    bool decodeFileWithPB(const std::string &inputFileName, const std::string &outputFileName);

    bool encodeFileWithMTF(const std::string &inputFileName, const std::string &outputFileName);
    bool decodeFileWithMTF(const std::string &inputFileName, const std::string &outputFileName);

    bool encodeFileWithRLE(const std::string &inputFileName, const std::string &outputFileName);
    bool decodeFileWithRLE(const std::string &inputFileName, const std::string &outputFileName);

protected:
    static void encodeWithBWT(std::vector<uint8_t> &data);
    static void decodeWithBWT(std::vector<uint8_t> &encodedData);

    static void encodeWithDelta(std::vector<uint8_t> &data);
    static void decodeWithDelta(std::vector<uint8_t> &encodedData);

    static void encodeWithCube(std::vector<uint8_t> &data);
    static void decodeWithCube(std::vector<uint8_t> &encodedData);

    static void encodeWithComplement(std::vector<uint8_t> &data);
    static void decodeWithComplement(std::vector<uint8_t> &encodedData);

    static void encodeWithBlockSort(std::vector<uint8_t> &data);
    static void decodeWithBlockSort(std::vector<uint8_t> &encodedData);

    static void encodeWithPB(std::vector<uint8_t> &data);
    static void decodeWithPB(std::vector<uint8_t> &encodedData);

    static void encodeWithMTF(std::vector<uint8_t> &data);
    static void decodeWithMTF(std::vector<uint8_t> &encodedData);

    static void encodeWithRLE(std::vector<uint8_t> &data);
    static void decodeWithRLE(std::vector<uint8_t> &encodedData);

private:
    bool encodeFile(const std::string &inputFileName, const std::string &outputFileName,
                    std::function<void(std::vector<uint8_t> &)> encodeAlgorithm);
    bool decodeFile(const std::string &inputFileName, const std::string &outputFileName,
                    std::function<void(std::vector<uint8_t> &)> decodeAlgorithm);
};

#endif // TRANSFORMATIONALGORITHMS_H
