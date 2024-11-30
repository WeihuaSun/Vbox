# Vbox: Efficient Black-Box Serializability Verification

## Overview

Vbox is an efficient framework for black-box serializability verification in database transaction histories. It aims to provide an accurate and scalable method for checking serializability without requiring access to the internal database mechanisms, using only transaction logs. 

This repository contains the source code, installation instructions, and usage guidelines for running the framework and reproducing experimental results.

## Table of Contents

- [Overview](#overview)
- [Installation](#installation)
- [Usage](#usage)
- [Reproducing Experimental Results](#reproducing-experimental-results)
- [License](#license)

## Installation

### Prerequisites

Before installing Vbox, ensure that you have the following dependencies installed:

- **g++ 7.5.0**
- **CMake 3.10.2**

### Steps

1. **Clone the repository:**

   ```bash
   git clone https://github.com/WeihuaSun/Vbox.git
   cd Vbox
   ```
2. **Install MonoSAT:**
   ```bash
   apt install zlib1g-dev
   apt install libgmp-dev
   cd monosat
   cmake .
   make
   make install
   cd ..
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

# Reproducing Experimental Results

This guide provides the necessary steps to reproduce the experimental results from the paper. It includes instructions for configuring **DBcop** (BE Algorithm) and **Cobra**, along with installation and environment setup.

## Configuring DBcop (BE Algorithm)

To reproduce the results, you first need to configure **DBcop**, which is the implementation of the **BE Algorithm** used for black-box verification.

### Step 1: Clone the DBcop Repository

Clone the DBcop Git repository, ensuring that you are using the correct branch (`oopsla-2019`):

```bash
git clone -b oopsla-2019 https://github.com/rnbguy/dbcop.git ./baselines/dbcop
```

### Step 2: Install Environment
DBcop is written in Rust, so you will need to install the Rust programming language. Run the following commands to install Rust and Clang:

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source $HOME/.cargo/env
apt-get install clang
```

### Step 3: Install DBcop
Navigate to the DBcop directory and install it:

```bash
cd baselines/dbcop
cargo install --path .
```

## Configuring Cobra

### Step 1: Clone the Cobra Repository
Clone the Cobra repository and its submodules:
```bash
git clone --recurse-submodules https://github.com/DBCobra/CobraHome.git ./baselines/CobraHome
```

### Step 2: Install Environment
Cobra requires several dependencies. Install them using the following commands:
```bash
apt install openjdk-8-jdk
apt install libgmpxx4ldbl maven wcstools
wget https://developer.download.nvidia.com/compute/cuda/10.1/Prod/local_installers/cuda_10.1.243_418.87.00_linux.run
sh cuda_10.1.243_418.87.00_linux.run
```
After installing CUDA and JAVA, you need to set the CUDA_HOME and JAVA_HOME environment variables. Add the following to your shell configuration file (.bashrc, .zshrc, etc.):

```bash
export CUDA_HOME=/usr/local/cuda-10.1
export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
```

After adding these lines, source the file to apply the changes:

```bash
source ~/.bashrc
```

Then, navigate to the Cobra directory and load the environment:

You also need to set the CUDA_HOME and JAVA_HOME in CobraHome/./env.sh

```bash
cd ./baseline/CobraHome/
source ./env.sh
```

Step 3: Install Cobra

```bash
cd $COBRA_HOME/CobraVerifier/
mvn install:install-file -Dfile=./monosat/monosat.jar -DgroupId=monosat \
  -DartifactId=monosat -Dversion=1.4.0 -Dpackaging=jar -DgeneratePom=true
./run.sh build
```

## Running the Experiment Script

### Python Environment Setup
The experiment script requires Python 3.6 and the psutil package. You can install Python 3.6 and the required package as follows:

```bash
apt-get install python3.6
pip install psutil
```

### Running the Script
After completing the above steps, you can run the experiment script to evaluate the performance and reproduce the results. 

To run the script, execute the following command:

```bash
python3 scripts/evaluate.py <eval_type>
```
Where `<eval_type>` is the evaluation type you wish to run.

#### Available Evaluation Types
The script supports several evaluation types. Each type evaluates a different aspect of the framework. Below is a table of available evaluation types, their descriptions, and the functions they trigger.

Where <eval_type> is the evaluation type you wish to run.


#### Example Commands

1. Basic Efficiency Test:
```bash
python3 scripts/evaluate.py base
```
2.  Scalability Test:
```bash
python3 scripts/evaluate.py scalability
```
3.   Completeness Test:
 ```bash
python3 scripts/evaluate.py completeness
```
4. Construction of Closure Test:
```bash
python3 scripts/evaluate.py construct
```
5. Update Efficiency Test:

```bash
python3 scripts/evaluate.py update
```
6. Solver Performance Test:
```bash
python3 scripts/evaluate.py solver
```

Note: The some evalutions may take several hours to complete. The results will be saved in the output/raw folder.

Note: Ensure that all dependencies are correctly installed and environment variables are set, or you may encounter runtime errors.
