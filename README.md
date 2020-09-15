# XAP Core Buffer Utilities

This package implemented a set of utilities used to read/write buffer in network application.

## Requirements

| Package | Version   |
|---------|-----------|
| CMake   | &gt;=3.10 |

| OS              | Version |
|-----------------|------------|
| Ubuntu Server   | &gt;=18.04 |          
| macOS Catalina  | &gt;=10.15 |

| Compiler    | Version     |
|-------------|-------------|
| GCC         | &gt;=7.5.0  |
| Apple clang | &gt;=11.0.3 |

## Build

You can run the following command to build static/shared library and unit test executable.

```
cmake .
make
```

## Test

Run the following command to run unit test:

```
cmake test
```