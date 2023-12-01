# Matrix Multiplication Project

## Overview

This project demonstrates matrix multiplication using both process-based and threaded solutions. It includes implementations using fork() for process management and Pthreads for multithreaded processing. The performance of these solutions is compared, and the impact of thread management (joining and detaching) on throughput is measured.

## Table of Contents

- [Requirements](#requirements)
- [Usage](#usage)
- [Performance Comparison](#performance-comparison)
- [Thread Management](#thread-management)
- [Results and Analysis](#results-and-analysis)
- [How to Run](#how-to-run)
- [Contributing](#contributing)
- [License](#license)

## Requirements

- POSIX-compliant system for process management
- Pthreads library for multithreaded processing
- C compiler

## Usage

The project consists of a single C file that encompasses the following:

- **Process Management (Part 1):**
  - The `childProcess` function contains code for the child process.

- **Threaded Processing (Part 2):**
  - The `threadFunc` function contains code executed by each thread.

- **Process vs Thread Implementations (Part 3):**
  - Implementation of process-based and threaded solutions after comments in the `main` function.

- **Thread Management (Part 4):**
  - Code for joining/detaching threads after Part 3 in the `main` function.

## Performance Comparison

The performance of the matrix multiplication is compared among the following approaches:

- Naive approach without using child processes or threads.
- Multiple child processes running in parallel.
- Multiple joinable threads running in parallel.
- Multiple detached threads running in parallel.

Experiments are documented, and results are presented in a table for clear comparison.

## Thread Management

The impact of joining vs detaching threads on throughput is analyzed. Various configurations are measured to understand the optimal setup.

## Results and Analysis

The proper/optimal number of child processes or threads is justified based on experimental results. All experiments and results are thoroughly documented.

## How to Run

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/MatrixMultiplication.git
   ```

2. Compile the C code:
   ```bash
   gcc -o matrix_multiplication matrix_multiplication.c -pthread
   ```

3. Run the executable:
   ```bash
   ./matrix_multiplication
   ```
