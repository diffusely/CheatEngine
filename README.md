# CheatEngine

A lightweight Linux process finder utility that searches for running processes by name.

## Build

```bash
make
```

## Usage

```bash
./cheatengine <process_name>
```

**Example:**
```bash
./cheatengine firefox
```

## Clean

```bash
make clean
```

## Requirements

- GCC
- Linux (uses /proc filesystem)
