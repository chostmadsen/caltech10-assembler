## **Perfhash**

Generates perfect hashes with a modified FNV-1a 64-bit hash in $2^{\lceil \lg (n) \rceil + o}$ buckets, where $n$ is
the number of items to hash and $o$ is the given offset.

Hash search is multithreaded if enabled, utilizing all CPU cores. The final offset will be whichever offset is found
first, and may not necessarily be deterministic.

## **Usage**

The algorithm will output the following message during hashing, where `name` is the hash table name, `items` is the
number of items being hashed, `buckets` is the number of buckets, and `probability` is the estimated probability of
success, per thread trial.

```
[name] : [items] -> [buckets] [probability]
```

To generate the hash tables, you will want to construct a file structured like

```c
/**
 * perfhash/impl/example.c
 * Main tokenizer hashing call.
 */

#include    "general.h"
#include    "file_emit.h"
#include    [item_location]

/**
 * Main tokenizer hashing call.
 *
 * @return                      exit code
 */
int main(void) {                                                                // main
    // hash
    hash_full([items], arr_s([items]), [seed], [offset], [max_attempts], [inc_file], [name], [output]);
    return  0;
}
```

where `seed` is the initial seed. The hash item needs to have the string, stringified token, stringified group,
stringified instruction (specific to this assembler) and whether to treat the string as case-sensitive. Case-insensitive
items will hash all case variants of the provided string, so this is only recommended for short strings.
(for this project specifically, lookup has been modified to be case insensitive anyways, so this setting will just
decrease performance).

Look to the examples to see actual code examples and output.

> #### **Requirements**
> 
> - A C23-compliant compiler (gcc 14+, clang 18+, for example; MSVC doesn't yet fully support C23)
> - POSIX thread support if multithreading; otherwise, disable with the `-n` flag
> - git
> - POSIX-compliant shell script interpreter to compile (the compilation is relatively simple, and can probably be
done in ~1 command if this is missing)

## **Caltech10 Assembler Usage**
Run

```bash
./perfhash-tokenizer.sh
```

To rehash the items. Add the call signatures if more hash items are needed. Refer to the
[standalone repository](https://github.com/chostmadsen/perfhash-fnv1a) if wanted.

To debug, provide the `-d` flag. To disable threading, provide the `-n` flag. To specify a c compiler, specify it with
`-c [compiler]`. To view this in the command line, provide the `-h` flag.
