## **Installation**

Installing the Caltech10 assembler can be done if the following requirements are met.

> #### **Requirements**
>
> - A C23-compliant compiler
> - POSIX thread support
> - git
> - bash (some version, not too sure; brew install bash if something doesn't work)
>
> These requirements should be pre-installed on macOS and GNU/Linux.

Furthermore, the Caltech10 assembler will probably not compile on any non-Unix-based system, but you can try.
To compile the assembler, run

```bash
git clone https://github.com/chostmadsen/caltech10-assembler.git
cd caltech10-assembler/assembler
./build-cit10a.sh && ./install-cit10a.sh
```

You can alter how the assembler is build with a few command-line arguments; for every script, the `-h` flag will always
be accepted, so you can provide that to get information.

If the assembler breaks in an unexpected way, try compiling the assembler with the `-a` flag; this will turn on
assertions, which should do a pretty good job at finding out what exactly happened, without too much performance
decrease.

------------------------------------------------------------------------------------------------------------------------

## **Uninstalling**

To uninstall the assembler, while at `caltech10-assembler/assembler`, run

```bash
./uninstall-cit10a.sh
```

This will uninstall the assembler, and you can safely delete your local source without worrying about artifacts from
this assembler.

------------------------------------------------------------------------------------------------------------------------

## **Usage**

Once installed, you can invoke the assembler with `cit10a`. Read the
[README](https://github.com/chostmadsen/caltech10-assembler/blob/main/assembler/README.md) for this assembler for more
information.
