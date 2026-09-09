# CodeGen - JACK Compiler

A C implementation of a compiler for the JACK programming language. The compiler parses JACK source files, performs symbol and semantic checks, and generates VM code.

## Project structure

```text
CodeGen/
├── src/                    # Compiler implementation
│   ├── compiler.c
│   ├── lexer.c
│   ├── parser.c
│   └── symbols.c
├── include/                # Header files
├── jack_os/                # JACK standard library definitions used by the compiler
├── tests/
│   ├── test_compiler.c     # Compiler test runner
│   └── fixtures/
│       ├── programs/       # JACK programs compiled during tests
│       └── expected/       # Expected VM output
├── .github/workflows/
│   └── tests.yml           # GitHub Actions test workflow
├── Makefile
└── README.md
```

## Requirements

You need a C compiler and `make`.

### macOS

Install Apple's command-line developer tools if they are not already installed:

```bash
xcode-select --install
```

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential
```

## Build

From the repository root:

```bash
make
```

This creates:

```text
build/codegen
```

## Compile a JACK program

Pass a directory containing one or more `.jack` files to the compiler:

```bash
./build/codegen path/to/jack/program
```

For example, to compile one of the included test programs:

```bash
./build/codegen tests/fixtures/programs/Pong
```

The generated `.vm` files are written into the same program directory.

## Run the tests

Run the full compiler test suite with:

```bash
make test
```

The test runner compiles each program under `tests/fixtures/programs/` and compares the generated VM instructions with the reference files under `tests/fixtures/expected/`.

The current test programs are:

- Seven
- Fraction
- HelloWorld
- Square
- Average
- ArrayTest
- MathTest
- List
- ConvertToBin
- Pong

## Clean generated files

```bash
make clean
```

This removes the compiled binaries and VM files generated inside the test input folders.

## Automated testing with GitHub Actions

The workflow in `.github/workflows/tests.yml` automatically runs:

```bash
make test
```

on every push and pull request. GitHub will show the test result in the repository's **Actions** tab and on pull requests.

No additional GitHub configuration is required after the workflow file is committed and pushed.
