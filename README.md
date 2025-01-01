# entropyreducer
Entropy Reducer

# Entropy Reduction Tool

This project is just a tool. I used it to perform operations on high entropy data and see the results of different tests.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)

## Features

- **Flexible and Scalable:** Easily adaptable to different datasets and applications.
- **Open Source:** Open to community contributions and continuously being developed.

## Installation

### Requirements

- **CMake**: Version 3.10 or higher
- **C++ Compiler**: Supporting C++11 or higher (e.g., GCC, Clang, MSVC)

### Steps

1. **Clone the Repository:**
    ```bash
    git clone https://github.com/azizgokhannarin/guessingbytepairs.git
    cd guessingbytepairs
    ```
2. **Create a Build Directory:**
    ```bash
    mkdir build
    cd build
    ```

3. **Configure the Project with CMake:**
    ```bash
    cmake ..
    ```
    *If you need to specify custom installation paths or enable/disable features, you can pass additional flags to CMake. For example:*
    ```bash
    cmake -DCMAKE_INSTALL_PREFIX=/desired/install/path -DENABLE_ASSIMP=ON ..
    ```

4. **Build the Project:**
    ```bash
    cmake --build .
    ```
    *Alternatively, you can use make or your preferred build system:*
    ```bash
    make
    ```

5. **Install the Project (Optional):**
    ```bash
    cmake --install .
    ```
    *This step installs the built binaries and resources to the specified installation directory.*

## Usage

I used the TransformationAlgorithms class for operations used to change the structure of the data.

I used the CompressionAlgorithms class to examine the behavior and logic of different compression algorithms.

## Examples

    CompressionAlgorithms compAlgo;
    TransformationAlgorithms transAlgo;
    
    transAlgo.encodeFileWithBWT("movie.mp4", "movie.bwt");
    compAlgo.compressFileWithLZMA2("movie.bwt", "movie.lzma2");

    compAlgo.decompressFileWithLZMA2("movie.lzma2", "movie.r.bwt");
    transAlgo.decodeFileWithBWT("movie.r.bwt", "movie.r.mp4");

### Simple Byte Data

## Contributing

We welcome your contributions! To contribute to the project, follow these steps:

1. **Fork the Repository:** Fork the project repository on GitHub.
2. **Create a New Branch:** 
    ```bash
    git checkout -b feature/FeatureName
    ```
3. **Make Your Changes:** Add or fix code as needed.
4. **Commit Your Changes:** 
    ```bash
    git commit -m "Descriptive commit message"
    ```
5. **Push to the Branch:** 
    ```bash
    git push origin feature/FeatureName
    ```
6. **Open a Pull Request:** Submit a pull request to integrate your changes into the main repository.

## License

This project is licensed under the [Apache 2.0](LICENSE). For more details, please refer to the `LICENSE` file.
