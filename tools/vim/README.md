## **About**

Additional Caltech10 tools for Vim / Neovim.

------------------------------------------------------------------------------------------------------------------------

## **Syntax Highlighting**

Syntax highlighting for `.obj` files, in accordance with the `.obj` formatting to run the `.obj` files.

#### **Installation**

Clone the repository if you haven't.

```bash
git clone https://github.com/chostmadsen/caltech10-assembler.git
```

Copy the files at [tools/vim](https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/vim/) to `~/.vim/` or
`~/.config/nvim/`. If you are at the repository root, you can do this (for Neovim) with

```bash
cp -r tools/vim/{ftdetect,ftplugin,syntax} ~/.config/nvim/
```

or, for Vim, with

```bash
cp -r tools/vim/{ftdetect,ftplugin,syntax} ~/.vim/
```

You can verify proper installation with the following:

```vim
:set filetype?      " should return filetype=citobj
:syntax list        " should return syntax items (should be synced w/ tools/vim/syntax/citobj.vim)
```

> #### **Requirements**
> - Vim 8.0+ or any version of Neovim

> #### **File Information**
> - [tools/vim/syntax/citobj.vim]
> (https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/vim/syntax/citobj.vim) adds syntax
> highlighting
> - [tools/vim/ftplugin/citobj.vim]
> (https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/vim/ftplugin/citobj.vim) adds
> buffer-local settings
> - [tools/vim/ftdetect/citobj.vim]
> (https://github.com/chostmadsen/caltech10-assembler/blob/main/tools/vim/ftdetect/citobj.vim) recognizes
> `.obj` files correctly
