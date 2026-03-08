# tANS compression system for text


## Table of Contents

1. [Introduction](#introduction)
2. [Description](#description)
3. [Setup](#setup)
4. [License](#license)

## Introduction

Playground repository with C++ implementation of the table Asymmetric Numeral System (ANS) compression, based on [Asymmetric numeral systems: entropy coding combining speed of Huffman coding with compression rate of arithmetic coding](https://arxiv.org/abs/1311.2540). 
The project provides a tool for compressing and decompressing text data, with compression tailored to text content.

## Description

Upon execution, the program provides a menu with 4 options:

  1. Compress: File (.txt) → Binary (.bin)
  2. Decompress: Binary (.bin) → File (.txt)
  3. Decompress to Screen: Binary (.bin) → Terminal output
  4. Compress from Screen: Manual text input → Binary (.bin)


## Setup

Simple setup require:

- C++ Compiler: Supports C++14 or higher (e.g., GCC 9+, Clang, or MSVC).

- CMake: Version 3.15 or newer


To use this repository, follow these steps:

1. Clone this repository:

   ```bash
   git clone https://github.com/RafalBistron/tANS
   ```

2. Build the project:
   You can build the project using the command line:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build 
   ```
   Or open and run project in CLion.
   

## License

This repository is licensed under the MIT License. Feel free to use the code and notebook for your educational and personal projects.
