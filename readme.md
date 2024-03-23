# Deduplicator

A file de-duplication tool (base on hash and size of file).

## Usage

```sh
deduplicator <dir>
```

- stderr will output progress information
- stdout will output duplicated files' info

Example:

```shell
user@Machine ~ $ deduplicator /foo/bar 2>/dev/null
# ========== duplicated ==========
#rm /foo/bar/a/b/baf
#rm /foo/bar/c/baf.1
# ================================

# ========== duplicated ==========
#rm /foo/bar/d/e/hello
#rm /foo/bar/f/hello.0
# ================================

```

You can redirect stdout to a file and remove the `#` before `rm` to decide which file to remove:

```shell
user@Machine ~ $ deduplicator /foo/bar 2>/dev/null > dup_files
user@Machine ~ $ vim dup_files # remove the `#` before `rm` to select the file to remove
user@Machine ~ $ sh dup_files  # remove duplicated files
```

## Build

Dependencies:

- [OpenSSL](https://www.openssl.org/): SHA-512 message digest
- [sqlitemm](https://github.com/exdinner/sqlitemm): SQLite

```sh
cmake -DCMAKE_BUILD_TYPE=Release -S . -B ./build
cmake --build ./build
cmake --install ./build
```
