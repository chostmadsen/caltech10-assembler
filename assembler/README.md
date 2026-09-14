## **About**

The assembler uses a perfect hash to look through opcodes, and assembles multiple lines at once once various lookup
tables are created (via hashmaps). The fact that it is multithreaded means it is theoretically faster than single
threaded assemblers, but the spawn time for threads is so great that this will almost certantly never pay off.

For detailed information about how the assembler works, read [man/caltech10_assembler_info.pdf]. For usage, see
[Usage](#usage). For installation instructions, see [Installation](#installation).

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

The assembler takes the main entry as its only non-flagged argument. Otherwise, provide additional `.asm` files with the
`-S` flag (the next argument must be a directory), and additional `.inc` files with the `-I` flag (this also must be a
directory). The `-o` flag specifies the output; this will default to whatever your input will be, with the `.asm`
(or `.a`) extension replced with `.obj` in the directory the assembler is invoked from.

The assembler takes the `-h` flag for usage information; you can view this to view some of the more niche arguments.

The assembler has a `.code` section, for code, `.data` for data, and `.none` to break out of any section.
You can include files with `.include`, and define numeric constants with `.const`. The assembler stays more or less
fully compliant with the spec at 
[man/caltech10_cpu_desc.pdf](https://github.com/chostmadsen/caltech10-assembler/blob/main/man/caltech10_cpu_desc.pdf).
The only main difference is how operands are provided for load / store instructions. Instead of the `+` syntax, the
syntax uses a comma instead, so something like

```asm
ld      -x+o
st      s++o
```

becomes

```asm
ld      -x,     o
st      s+,     o
```

Furthermore, you can omit any `o` item in the spec from an argument for `o` to default it to 0 (like the following).

```asm
ld      s+0
```

is the equivalent to either of the following.

```asm
ld      s,      0
ld      s
```

The compiler's opcodes and registers are case insensitive. The headers and constants are case insensitive by default,
but this can be changed by providing the `-C` flag (which will then treat things as case sensitive).

There is a simulator under [tools/](https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/). For
information on how to get it to run, see
[tools/README.md](https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/README.md).
