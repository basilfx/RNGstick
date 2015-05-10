# RNGstick
Entropy stick for experimenting, generating and evaluating true random numbers.

Status: evaluating prototype.

## Introduction
This project provides a hardware random number generation with a customizable firmware for experimenting. A NE555 free-running oscillator is used, of which the clock drift is measured and used as a source of randomness. The stick can be used for feeding the Linux entropy pool for random numbers.

The firmware is configurable via a built-in command line.

## Background
The idea for this project is from a research project I conducted during my master's programme. Together with two others, we did a research on using accelerometers from mobile phones for generating true random numbers.

### True randomness
True random numbers (in contrast to pseudorandom numbers) are unpredictable. Given a number, the next number is not predictable and not linked to it. Computers cannot generate true random numbers via an algorithm, but they can observe physical events and extract uncertainty (entropy). For instance, mouse movement, disk access time and network traffic times are considered unpredictable. There is one problem: these events may occur not frequent enough, which means that the pool of entropy will exhaust. For general purpose random numbers, this is not a big deal, but for generating cryptographic keys, this becomes a problem.

An operating system does not directly generate numbers from this pool, but uses a pseudorandom algorithm that is seeded with the collected entropy. On Linux, `/dev/random` will block when the entropy pool is exhausted, but `/dev/urandom` won't. A quick experiment is to `cat /dev/random` and move the mouse around. You will notice the rate of numbers will increase (at least on Linux without HWRNG).

### Project rationale
In the last few years, processor manufacturers have incorporated hardware random number generators in processors. A kernel can use this source directly to generate random numbers, or to feed the entropy pool. However, with recent NSA leakings, some OS's have already [announced](http://www.theregister.co.uk/2013/12/09/freebsd_abandoning_hardware_randomness/) to not trust these output sources directly.

This project is an open-source attempt to generate random numbers of sufficien quality, using a low number of hardware parts. It is based on [this](http://1474orchard.ca/projects/?x=entry:entry120926-185104) blog post, where the author is using a NE555 free-running oscillator and uses the clock drift as a source of randomness. An Arduino samples the output of the NE555 regularly. Clock drift is affected by temperature, part stability, voltage and many more factors, that are unpredictable in some sense. In short, given two waveforms they won't be the same as one period may be short or longer than the other.

I have used this design and created an USB stick with a customizable firmware for generating random bytes. In addition to the original design, the NE555 can be reset before each iteration and the voltage reference for the ADC is customizable. Many parameters can be tuned to generate random numbers in a different way. The following features are available:

* Sampling: batch-samples, or per-sample
* Sample processing: masking, shifting and delays
* Bit extracting: one-majority, or raw bits
* Extraction: (Improved) Von Neumann bias removal, or none
* Generating: SHA-1, or none
* Buffering: store bytes to output chuncks, or continuously
* Outputting: (Binary) bytes or bits

To give an idea of the output, the following histogram was created, using ones-majority over the lower five bits and bias removal using Improved Von Neumann, without any generator. We should be careful to call this random from a graph only, because a predictable sequence from 0-255 over and over would generate a similar graph :-)

![board](https://raw.github.com/basilfx/RNGstick/master/docs/byte_histogram.png)

### Testing
The problem with random numbers is that you cannot prove they are random. However, there are statistical tests which can indicate if a sequence of random numbers is 'random' enough. Even a sequence of numbers that doesn't look random could be random, as the random number generator could produce this sequence.

Three programs are available to check the produced sequences. The first is `ent`, which calculates the Entropy, Chi-square Distribution and the Arithmetic Mean Value. The second one is `rngtest`, which implements the FIPS 140-2 tests. The last one is `dieharder`, which implements a lot of different tests.

TODO add test results here

## Contents
* Eagle schematic and PCB
* Arduino firmware

## Eagle

### Bill of materials
| Part      | Value         | Device                    | Package         |
|-----------|---------------|---------------------------|-----------------|
|   C1      |   22pF        |   C-EUC0603K              |   C0603K        |
|   C2      |   22pF        |   C-EUC0603K              |   C0603K        |
|   C3      |   22pF        |   C-EUC0603K              |   C0603K        |
|   C4      |   22pF        |   C-EUC0603K              |   C0603K        |
|   C5      |   10nF        |   C-EUC0603K              |   C0603K        |
|   C6      |   100nF       |   C-EUC0603K              |   C0603K        |
|   C7      |   100nF       |   C-EUC0603K              |   C0603K        |
|   D1      |               |   PESD5V0U1BB             |   SOD523        |
|   D2      |               |   PESD5V0U1BB             |   SOD523        |
|   IC1     |               |   NE555D                  |   SO08          |
|   JP1     |   USB-A-H     |   USB-A-H                 |   USB-A-H       |
|   JP4     |   M02PTH      |   M02PTH                  |   1X02          |
|   R1      |   5K          |   TRIM_EU-3223W           |   3223W         |
|   R2      |   330R        |   R-EU_R0603              |   R0603         |
|   R3      |   10k         |   R-EU_R0603              |   R0603         |
|   R4      |   10k         |   R-EU_R0603              |   R0603         |
|   U$1     |               |   CH340G                  |   SOIC16        |
|   U1      |               |   ATMEGA328_SMT           |   TQFP32-08     |
|   X1      |   12MHz       |   CRYSTALS-NOGND-4-HSMX   |   HC49-4-HSMX   |
|   X2      |   16MHz       |   CRYSTALS-NOGND-4-HSMX   |   HC49-4-HSMX   |

### Board render
Render of the stick. It measures 1.50x0.65 inch.

![board](https://raw.github.com/basilfx/RNGstick/master/docs/stick.png)

## Firmware
The firmware resides in `firmware/RNG`. You will need a copy of the [CommandLine](https://github.com/basilfx/Arduino-CommandLine) Arduino library.

## Software

### Configuration
To configure the stick, short JP4 while booting. A setup prompt should appear. Use the `help` command to list all available commands. Do not forget to save a change using the `save` command. Use `defaults` or `load` respectively.

### Installation
This will install `rng-tools` for feeding the random numbers back into the kernel. It applies to Ubuntu 15.04, but others will work fine too. `/dev/ttyACM0` is assumed to be the RNGstick.

* Instal `rng-tools` via `sudo apt-get install rng-tools`
* Change `/etc/defaults/rng-tools`
 * Add `stty -F /dev/ttyACM0 115200 time 25`
 * Add `HRNGDEVICE=/dev/ttyACM0`
* Restart `rng-tools` with `sudo service rng-tools restart`
* Observe `/proc/sys/kernel/random/entropy_avail`


## License
See the `LICENSE` file (MIT license).