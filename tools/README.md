## **About**

Extra supporting tools for the Caltech10 Assembler.

------------------------------------------------------------------------------------------------------------------------

## **Simulator**

The simulator wasn't written by me, but it was provided by this class so it most likely is fine. It is an `.exe`, file,
found as `caltech_sim.exe` in this directory. If you want to run it on non-windows operating systems, you can use
[Wine](https://www.winehq.org/) or some equivalent.

------------------------------------------------------------------------------------------------------------------------

## **Debugging Tools**

If your compiler fails to compile the assembler, you should be directed to some file within this directory. Below are
what the files do.

- [tools/alignment_def.sh](https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/alignment_def.sh) /
[tools/alignment_def.c](https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/alignment_def.c) help
determine padding to align singular line assembly instructions across cache lines, so the threader can properly split
items across threads without false sharing. It shouldn't need modification, but if it does, run the shell script and
it will tell you what to set the padding to.

------------------------------------------------------------------------------------------------------------------------

## **Vim / Neovim Tools**

Syntax highlighting for Vim and Neovim for `.obj` files, aligned with what the Caltech10 C.P.U. takes. For installation
instructions, see
[tools/vim/README.md](https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/vim/README.md).
