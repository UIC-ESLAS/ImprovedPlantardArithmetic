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


The setup for testing and evaluating of our code is based on the framework provided in the [pqm4](https://github.com/mupq/pqm4) project.
## Prerequisites

- `arm-none-eabi-gcc`: version 10.2.1
- `libopencm3`: commit `44e142d4f97863e669737707a1a22bf40ed49bbc` from [GitHub](https://github.com/libopencm3/libopencm3/tree/44e142d4f97863e669737707a1a22bf40ed49bbc)
- `st-link` for flashing the binaries
- `python3` with the packages `pyserial` and `numpy` (only required for the evaluation scripts)
- Hardware: `STM32F407G-DISC1`

## Files in this archive

- `common`: contains code that is shared between different schemes
- `config.py`: Saves platform configuration
- `crypto_kem`: contains the implementations for kyber512, kyber768, kyber1024, nttru
    - `kyber512`
        - `old`: Our code with the improved Plantard arithmetic based on the implementation in [ABCG20].
        - `m4fspeed`: Our code with the improved Plantard arithmetic based on the speed-version implementation in [AHKS22]
        - `m4fstack`: Our code with the improved Plantard arithmetic based on the stack-version implementation in [AHKS22]
    - `kyber512-90s`
        - `m4fspeed`: Our code with the improved Plantard arithmetic based on the speed-version implementation in [AHKS22]
        - `m4fstack`: Our code with the improved Plantard arithmetic based on the stack-version implementation in [AHKS22]
    - `kyber768`
        - `old`: Our code with the improved Plantard arithmetic based on the implementation in [ABCG20].
        - `m4fspeed`: Our code with the improved Plantard arithmetic based on the speed-version implementation in [AHKS22]
        - `m4fstack`: Our code with the improved Plantard arithmetic based on the stack-version implementation in [AHKS22]
    - `kyber768-90s`
        - `m4fspeed`: Our code with the improved Plantard arithmetic based on the speed-version implementation in [AHKS22]
        - `m4fstack`: Our code with the improved Plantard arithmetic based on the stack-version implementation in [AHKS22]
    - `kyber1024`
        - `old`: Our code with the improved Plantard arithmetic based on the implementation in [ABCG20].
        - `m4fspeed`: Our code with the improved Plantard arithmetic based on the speed-version implementation in [AHKS22]
        - `m4fstack`: Our code with the improved Plantard arithmetic based on the stack-version implementation in [AHKS22]
    - `kyber1024-90s`
        - `m4fspeed`: Our code with the improved Plantard arithmetic based on the speed-version implementation in [AHKS22]
        - `m4fstack`: Our code with the improved Plantard arithmetic based on the stack-version implementation in [AHKS22]
    - `nttru`
        - Our code with the improved Plantard arithmetic based on [LS19]
    - `f_speed.c`: Firmware used for benchmarking parts of the scheme. Can be used by using `f_benchmarks.py`.
    - `speed.c`: From pqm4; Firmware for benchmarking the schemes' cycle counts. Can be used by using `benchmarks.py`.
    - `stack.c`: From pqm4; Firmware for benchmarking the schemes' stack usage. 
    - `test.c`: From pqm4; Firmware for self-testing the schemes. Can be used by using `test.py`.
    - `testvectors.c`: From pqm4; Firmware for computing testvectors for Kyber only.
- `crypto_sign`: contains the implementations for dilithium2, dilithium3, and dilithium5
    - `dilithium2`
        - `new`: Our code with the improved Plantard arithmetic based on the implementation in [AHKS22].
        - `old`: Code in [AHKS22]
    - `dilithium3`
        - `new`: Our code with the improved Plantard arithmetic based on the implementation in [AHKS22].
        - `old`: Code in [AHKS22]
    - `dilithium5`
        - `new`: Our code with the improved Plantard arithmetic based on the implementation in [AHKS22].
        - `old`: Code in [AHKS22]
    - `f_speed.c`: Firmware used for benchmarking parts of the scheme. Can be used by using `f_benchmarks.py`.
    - `speed.c`: From pqm4; Firmware for benchmarking the schemes' cycle counts. Can be used by using `benchmarks.py`.
    - `stack.c`: From pqm4; Firmware for benchmarking the schemes' stack usage. 
    - `test.c`: From pqm4; Firmware for self-testing the schemes. Can be used by using `test.py`.
    - `testvectors.c`: From pqm4; Firmware for computing testvectors for Dilithium only.
- `gen_table`: contains code to generate the twiddle factors in Plantard domain for our implementations.
- `Makefile`: Makefile to build the code
- `benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `speed.c`. The desired algorithms as well as the number of iterations can be set in the code.
- `f_benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `f_speed.c`. The desired algorithms as well as the number of iterations can be set in the code.
- `stack_benchmarks.py`: This script is used for building, flashing, and evaluating the outputs produced by `stack.c`. The desired algorithms as well as the number of iterations can be set in the code.
- `stm32f405x6_full.ld`: Linker script using 128kB of memory (SRAM1 and SRAM2)
- `stm32f405x6.ld`: Linker script using 112kB of memory (SRAM1 only)
- `stm32f4discovery.cfg`: Configuration for openocd in case debugging is desired
- `test.py`: This script is used for building, flashing, and evaluating the outputs produced by `test.c`.

## Usage
Detailed instructions on interacting with the hardware and on installing required software can be found in [pqm4](https://github.com/mupq/pqm4)'s readme.

The scripts `benchmarks.py`, `f_benchmarks.py`, `stack.py` and `test.py` cover most of the frequent use cases.
In case separate, manual testing is required, the binaries for a scheme can be build using
```
make IMPLEMENTATION_PATH=crypto_{kem,sign}/{scheme}/{variant} bin/crypto_{kem,sign}_{scheme}_{variant}_{firmware}.bin
```
, where `firmware` is one of `{test, testvectors, speed, f_speed, stack}` and `variant` is one of `old, m4fstack` (`testvectors` only available for Kyber).

It can then be flashed using: 
```
st-flash --reset write bin/crypto_{kem,sign}_{scheme}_{variant}_{firmware}.bin 0x8000000
```
### Example
For building the `test` firmware for our m4fstack version of `kyber768` the following command can be used:
```
make IMPLEMENTATION_PATH=crypto_kem/kyber768/m4fstack bin/crypto_kem_kyber768_newstack_test.bin
```
It can the be flashed using:
```
st-flash --reset write bin/crypto_kem_kyber768_newstack_test.bin 0x8000000
```

For building the `test` firmware for our new version of `dilithium2` the following command can be used:
```
make IMPLEMENTATION_PATH=crypto_sign/dilithium2/new bin/crypto_sign_dilithium2_new_test.bin
```
It can the be flashed using:
```
st-flash --reset write bin/crypto_sign_dilithium2_new_test.bin 0x8000000
```

### reference
[AHKS22] Amin Abdulrahman, Vincent Hwang, Matthias J Kannwischer, and Daan Sprenkels. Faster Kyber and Dilithium on the Cortex-M4. Cryptology ePrint Archive, 2022.  
[LS19] Lyubashevsky V, Seiler G. NTTRU: Truly Fast NTRU Using NTT[J]. IACR Transactions on Cryptographic Hardware and Embedded Systems, 2019: 180-201.  
[ABCG20] Erdem Alkim, Yusuf Alper Bilgin, Murat Cenk, and François Gérard. Cortex-M4 optimizations for {R, M}-LWE schemes. IACR Transactions on Cryptographic Hardware and Embedded Systems, pages 336–357, 2020.  


### License

Each subdirectory containing implementations contains a LICENSE or COPYING file stating under what license that specific implementation is released. The files in common contain licensing information at the top of the file (and are currently either public domain or MIT). All other code in this repository is licensed under the Creative Commons Attribution 4.0 International License. To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/.

### Citation

Please cite our paper if you want to use this repository.
@article{Huang2022Improved,  
author = {Junhao Huang, Jipeng Zhang, Haosong Zhao, Zhe Liu, Ray C. C. Cheung, \c{C}etin Kaya Ko\c{c}, and Donglong Chen},  
title = {Improved {Plantard} Arithmetic for Lattice-based Cryptography},  
journal = {{IACR} Trans. Cryptogr. Hardw. Embed. Syst.},  
volume = {2022},  
number = {4},  
year = {2022}  
}