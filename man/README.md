## **About**

This directory contains detailed documentation for items within this repository.

- [man/caltech10_cpu_desc.pdf](https://github.com/chostmadsen/caltech10-assembler/blob/main/man/caltech10_cpu_desc.pdf)
has the CPU architecture information.

There should be a document about how the assembler works and how to use it, but I am unfortunately too lazy to write
this (for at least a little bit), so you can probably get by with varous READMEs and providing the assembler with the
`-h` flag.

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

If the assembler fails to compile, there is a chance I actually planned ahead and you were directed to some file within
[tools/](https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/) (this should happen based on some compiler output).
If you are directed there, read the
[README](https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/README.md) there for information.

------------------------------------------------------------------------------------------------------------------------

## **Usage**

You can invoke the assembler with `cit10a` once it is installed. For more detailed usage, see
[assembler/README.md](https://github.com/chostmadsen/caltech10-assembler/blob/main/assembler/README.md).

------------------------------------------------------------------------------------------------------------------------

## **Contributing**

See [CONTRIBUTING](https://github.com/chostmadsen/caltech10-assembler/blob/main/CONTRIBUTING)

------------------------------------------------------------------------------------------------------------------------

## **License**

This specific assembler for the Caltech10 CPU is under the [MIT License](https://opensource.org/license/mit). See
[LICENSE](https://github.com/chostmadsen/caltech10-assembler/blob/main/LICENSE) for more information.
