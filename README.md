# Vbox: Efficient Black-Box Serializability Verification

## Overview

Vbox is an efficient framework for black-box serializability verification in database transaction histories. It aims to provide an accurate and scalable method for checking serializability without requiring access to the internal database mechanisms, using only transaction logs. 

This repository contains the source code, installation instructions, and usage guidelines for running the framework and reproducing experimental results.

## Table of Contents

- [Overview](#overview)
- [Installation](#installation)
- [Usage](#usage)
- [Reproducing Experimental Results](#reproducing-experimental-results)
- [Contributing](#contributing)
- [License](#license)

## Installation

### Prerequisites

Before installing Vbox, ensure that you have the following dependencies installed:

- **g++ 7.5.0**
- **CMake 3.10.2**
- **MonoSAT** 

sudo apt install zlib1g-dev
sudo apt install libgmp-dev
cmake .
make
sudo make install

### Steps

1. **Clone the repository:**

   ```bash
   git clone https://github.com/WeihuaSun/Vbox.git
   cd Vbox
   ```
2. **Build the project:**

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
## Usage

### Command

   ```bash
    ./build/SerVerifier <log> <verifier> <time> <compact> <merge> <prune> <construct> <update> <sat>
   ```
| Parameter   | Description                                                 | Options                                        |
|-------------|-------------------------------------------------------------|------------------------------------------------|
| `<log>`     | Path to the directory containing the log files.             | -                                              |
| `<verifier>`| Type of verifier to use.                                    | `vbox`, `leopard`                              |
| `<time>`    | Timing enabled.                                             | `true`, `false`                                |
| `<compact>` | Compaction enabled.                                         | `true`, `false`                                |
| `<merge>`   | Merging enabled.                                            | `true`, `false`                                |
| `<prune>`   | Pruning strategy.                                           | `prune_opt`, `prune`                |
| `<construct>`| TC construction strategy.                                  | `purdom+`, `warshall`, `purdom`, `italino`, `italino+` |
| `<update>`  | TC update strategy.                                         | `italino+`, `warshall`, `italino`             |
| `<sat>`     | SAT strategy.                                               | `vboxsat`, `monosat`, `minsat`                 |

### Example
   
    
```bash
./build/SerVerifier /path/to/logs vbox true true true prune_opt purdom+ italino_opt vboxsat
```

## Reproducing Experimental Results

## 配置DBcop(BE算法)

安装cargo

curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

source $HOME/.cargo/env

安装clang
apt-get install clang

安装dbcop

cd baselines/dbcop

cargo install --path .

## 配置Cobra

需要环境

