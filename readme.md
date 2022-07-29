# Improved Plantard Arithmetic for Lattice-based Cryptography
This repository provides code for our implementations of Kyber and NTTRU using the improved Plantard arithmetic on the Arm Cortex-M4.
Authors: 
 - [Junhao Huang](https://github.com/JunhaoHuang) `<huangjunhao@uic.edu.cn>`
 - [Jipeng Zhang](https://github.com/Ji-Peng) `<jp-zhang@outlook.com>`
 - Haosong Zhao `<zhaohaosonguic@gmail.com>`
 - Zhe Liu `<zhe.liu@nuaa.edu.cn>` 
 - Ray C. C. Cheung `<r.cheung@cityu.edu.hk>`
 - Çetin Kaya Koç `<cetinkoc@ucsb.edu>`
 - Donglong Chen `<donglongchen@uic.edu.cn>` (Corresponding Author)

Our paper is available at https://eprint.iacr.org/2022/956.

The current branch mostly contains code from [1] and the reference code from [2]. The implementations of this branch are based on the Montgomery and Barrett arithmetic. The ``plantard`` branch consists of the code based on the improved Plantard arithmetic presented in ``Improved Plantard Arithmetic for Lattice-based Cryptography`` in TCHES2022-04.

The setup for testing and evaluating of our code is based on the framework provided in the [pqm4](https://github.com/mupq/pqm4) project.
Detailed instructions on interacting with the hardware and on installing required software can be found in pqm4's readme.
## Prerequisites

- `arm-none-eabi-gcc`: version 10.2.1
- `libopencm3`: commit `b1d8a4c546531d6a79f9a7be156205c6a40f215c` from [GitHub](https://github.com/libopencm3/libopencm3/tree/b1d8a4c546531d6a79f9a7be156205c6a40f215c)
- `st-link` for flashing the binaries
- `python3` with the packages `pyserial` and `numpy` (only required for the evaluation scripts)
- Hardware: `STM32F407G-DISC1`

## Files in this archive

- `common`: contains code that is shared between different schemes
- `config.py`: Saves platform configuration
- `crypto_kem`: contains the implementations for kyber512, kyber768, kyber1024, nttru
    - `kyber512(-90s)`
        - `old`: Code from the implementation in pqm4
        - `new`: Code containing proposals in [1]
        - `newstack`: Code containing only optimizations that do not require additional stack usage in [1]
    - `kyber768(-90s)`
        - `old`: Code from the implementation in pqm4
        - `new`: Code containing proposals in [1]
        - `newstack`: Code containing only optimizations that do not require additional stack usage in [1]
    - `kyber1024(-90s)`
        - `old`: Code from the implementation in pqm4
        - `new`: Code containing proposals in [1]
        - `newstack`: Code containing only optimizations that do not require additional stack usage in [1]
    - `nttru`
        - reference code for Cortex-M4 based on [2]; modified version to suit on Cortex-M4.
    - `f_speed.c`: Firmware used for benchmarking parts of the scheme. Can be used by using `f_benchmarks.py`.
    - `speed.c`: From pqm4; Firmware for benchmarking the schemes' cycle counts. Can be used by using `benchmarks.py`.
    - `stack.c`: From pqm4; Firmware for benchmarking the schemes' stack usage. 
    - `test.c`: From pqm4; Firmware for self-testing the schemes. Can be used by using `test.py`.
    - `testvectors.c`: From pqm4; Firmware for computing testvectors.
- `gen_table`: contains code to generate the twiddle factors for our implementations.
- `Makefile`: Makefile to build the code
- `benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `speed.c`. The desired algorithms as well as the number of iterations can be set in the code.
- `f_benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `f_speed.c`. The desired algorithms as well as the number of iterations can be set in the code. Additionally, it can be switched wether the hashing should be included in the benchmark for the matrix-vector multiplication. 
- `read_serial.py`: Script to receive serial communication from the target.
- `stack_benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `stack.c`. The desired algorithms as well as the number of iterations can be set in the code.
- `stm32f405x6_full.ld`: Linker script using 128kB of memory (SRAM1 and SRAM2)
- `stm32f405x6.ld`: Linker script using 112kB of memory (SRAM1 only)
- `stm32f4discovery.cfg`: Configuration for openocd in case debugging is desired
- `test.py`: This script is used for building, flashing, and evaluating the outputs produced by `test.c`.

## Usage

The scripts `benchmarks.py`, `f_benchmarks.py`, `stack.py` and `test.py` cover most of the frequent use cases.
In case separate, manual testing is required, the binaries for a scheme can be build using
```
make IMPLEMENTATION_PATH=crypto_kem/{scheme}/{variant} bin/crypto_kem_{scheme}_{variant}_{firmware}.bin
```
, where `firmware` is one of `{test, testvectors, speed, f_speed, stack}` and `variant` is one of `old, new, newstack` (`testvectors` only available for Kyber).

It can then be flashed using: 
```
st-flash --reset write bin/crypto_kem_{scheme}_{variant}_{firmware}.bin 0x8000000
```
### Example
For building the `test` firmware for our speed-optimized version of `kyber768` the following command can be used:
```
make IMPLEMENTATION_PATH=crypto_kem/kyber768/new bin/crypto_kem_kyber768_new_test.bin
```
It can the be flashed using:
```
st-flash --reset write bin/crypto_kem_kyber768_new_test.bin 0x8000000
```

### references:
[1] Abdulrahman A, Hwang V, Kannwischer M J, et al. Faster Kyber and Dilithium on the Cortex-M4[J]. Cryptology ePrint Archive, 2022.
[2] Lyubashevsky V, Seiler G. NTTRU: Truly Fast NTRU Using NTT[J]. IACR Transactions on Cryptographic Hardware and Embedded Systems, 2019: 180-201.

### Citation
Please cite our paper if you want to use this repository.

@article{Huang2022Improved,
author    = {Junhao Huang, Jipeng Zhang, Haosong Zhao, Zhe Liu, Ray C. C. Cheung, \c{C}etin Kaya Ko\c{c}, and Donglong Chen},
    title     = {Improved {Plantard} Arithmetic for Lattice-based Cryptography},
    journal   = {{IACR} Trans. Cryptogr. Hardw. Embed. Syst.},
    volume    = {2022},
    number    = {4},
    year      = {2022}
}