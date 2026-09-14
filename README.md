## **About**

The Caltech10 assembler is a multithreaded assembler meant to target the Caltech10 CPU architecture (an 8-bit, Harvard
architecture RISC CPU). It has an instruction set similar to 6502 assembly. To read more about this specific CPU
architecture, read
[man/caltech10_cpu_desc.pdf](https://github.com/chostmadsen/caltech10-assembler/blob/main/man/caltech10_cpu_desc.pdf).

For more information on the assembler itself, read
[assembler/README.md](https://github.com/chostmadsen/caltech10-assembler/blob/main/assembler/README.md).

For detiled documentation on various items, see `.pdf` files at
[man/](https://github.com/chostmadsen/caltech10-assembler/blob/main/man/).

------------------------------------------------------------------------------------------------------------------------

## **Installation**

To install the assembler, ensure you have the requirements at
[assembler/INSTALL.md](https://github.com/chostmadsen/caltech10-assembler/blob/main/assembler/INSTALL.md). This also
has more information on installing the assembler.

For basic installation, you can compile the assembler with the following.

```bash
git clone https://github.com/chostmadsen/caltech10-assembler.git
cd caltech10-assembler/assembler
./build-cit10a.sh && ./install-cit10a.sh
```

Then, invoke the assembler with `cit10a`. See [Usage](#usage) for more information.

------------------------------------------------------------------------------------------------------------------------

## **Usage**

You can invoke the assembler with `cit10a` once it is installed. For more detailed usage, see
[assembler/README.md](https://github.com/chostmadsen/caltech10-assembler/blob/main/assembler/README.md).

------------------------------------------------------------------------------------------------------------------------

## **Contributing**

See [CONTRIBUTING](https://github.com/chostmadsen/caltech10-assembler/blob/main/CONTRIBUTING).

------------------------------------------------------------------------------------------------------------------------

## **License**

This specific assembler for the Caltech10 CPU is under the [MIT License](https://opensource.org/license/mit). See
[LICENSE](https://github.com/chostmadsen/caltech10-assembler/blob/main/LICENSE) for more information.
