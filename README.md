# extract-xiso
[![Master Build Status](https://ci.appveyor.com/api/projects/status/github/XboxDev/extract-xiso?branch=master&svg=true)](https://ci.appveyor.com/project/xboxdev-bot/extract-xiso/history)

A command line utility created by [*in*](mailto:in@fishtank.com) to allow the creation, modification, and extraction of XISOs. Currently being maintained and modernized by the [*XboxDev organization*](https://github.com/XboxDev/XboxDev).

[**Click Here Latest Windows Build**](https://ci.appveyor.com/api/projects/xboxdev-bot/extract-xiso/artifacts/export/Release.zip?branch=master)

## Features

- Create XISOs from a directory.

- Extract XISO content to a directory.

- Convert Redumps to an XISO

- Multi-Platform and Open-Source.

## Usage

The `extract-xiso` utility can run in multiple modes: *create*, *list*, *rewrite*, and *extract*.

### Create `-c`

Create an XISO from a directiory.
```
# Create halo-2.iso in the current directory containing the files within ./halo-2.iso
./extract-xiso -c ./halo-2

# Create halo-ce.iso in the /home/me/games directory containing files in the ./halo-ce directory
./extract-xiso -c ./halo-ce /home/me/games/halo-ce.iso
```

### List `-l`

List the file contents within an XISO file.
```
# Get file contents of a XISO
./extract-xiso -l ./halo-ce.iso

# List file contents of multiple XISOs
./extract-xiso -l ./halo-2.iso ./halo-ce.iso
```

### Rewrite `-r`

Rewrites filesystem structure of an XISO.
```
# Rewrites XISO
./extract-xiso -r ./halo-ce.iso
# Can be batched
./extract-xiso -r ./halo-ce.iso ./halo-2.iso
```

### Extract `-x`

Extract XISO contents to a directory.
```
# Default mode when no arguments given, extracts to ./halo-ce/
./extract-xiso ./halo-ce.iso

# Can be given a target directory
./extract-xiso ./halo-2.iso -d /home/games/halo-2/
```

### Options

`extract-xiso` has a few optional arguments that can be provided in different modes:
```
-d <directory>      In extract mode, expand xiso in <directory>.
                    In rewrite mode, rewrite xiso in <directory>.
-D                  In rewrite mode, delete old xiso after processing.
-h                  Print this help text and exit.
-m                  In create or rewrite mode, disable automatic .xbe
                      media enable patching (not recommended).
-q                  Run quiet (suppress all non-error output).
-Q                  Run silent (suppress all output).
-s                  Skip $SystemUpdate folder.
-v                  Print version information and exit.
```

## Building

### Requirements

- cmake
- make
- gcc

### Windows / macOS / Linux

After requirements are installed with your distribution's package manager (or homebrew for macOS), open terminal and change directory to the project root. Then run the following build commands:

```
# Clone Repo
git clone https://github.com/XboxDev/extract-xiso.git

# cd into directory
cd extract-xiso

# Create working directory
mkdir build
cd build

# Build project
cmake ..
make
```

The compiled binary should now be in the `extract-xiso/build` directory as `extract-xiso`.
