#include <iostream>

#include "compressionalgorithms.h"
#include "transformationalgorithms.h"

int main()
{
    CompressionAlgorithms compAlgo;
    TransformationAlgorithms transAlgo;

    //    transAlgo.encodeFileWithBWT("movie.mp4", "movie.bwt");
    //    transAlgo.encodeFileWithMTF("movie.bwt", "movie.mtf");
    //    transAlgo.encodeFileWithRLE("movie.mtf", "movie.rle");
    //    compAlgo.compressFileWithLZMA2("movie.rle", "movie.lzma2");

    //    compAlgo.decompressFileWithLZMA2("movie.lzma2", "movie.r.rle");
    //    transAlgo.decodeFileWithRLE("movie.r.rle", "movie.r.mtf");
    //    transAlgo.decodeFileWithMTF("movie.r.mtf", "movie.r.bwt");
    //    transAlgo.decodeFileWithBWT("movie.r.bwt", "movie.r.mp4");


    //    transAlgo.encodeFileWithBWT("movie.mp4", "movie.bwt");
    //    transAlgo.encodeFileWithRLE("movie.bwt", "movie.rle");
    //    compAlgo.compressFileWithLZMA2("movie.rle", "movie.lzma2");

    //    compAlgo.decompressFileWithLZMA2("movie.lzma2", "movie.r.rle");
    //    transAlgo.decodeFileWithRLE("movie.r.rle", "movie.r.bwt");
    //    transAlgo.decodeFileWithBWT("movie.r.bwt", "movie.r.mp4");


    // Only this pair meaningful and works together
    transAlgo.encodeFileWithBWT("movie.mp4", "movie.bwt");
    compAlgo.compressFileWithLZMA2("movie.bwt", "movie.lzma2");

    compAlgo.decompressFileWithLZMA2("movie.lzma2", "movie.r.bwt");
    transAlgo.decodeFileWithBWT("movie.r.bwt", "movie.r.mp4");

    return 0;
}
