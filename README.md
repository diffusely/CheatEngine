# CheatEngine for Linux

A simple memory scanner and editor for Linux processes. Find and modify values in running programs.

## Features

**Scan memory** - Find values in process memory
**Write memory** - Change values at specific addresses
**PID selection** - Choose from multiple matching processes
**Refresh PIDs** - Update process list without restarting

## Build

```bash
make        # Build cheatengine
make test   # Build test program
make clean  # Clean build files
make re     # Rebuild all
```

## Usage

```bash
sudo ./cheatengine <process_name>
```

### Example

```bash
# Terminal 1: Run target program
./test

# Terminal 2: Run cheatengine (needs sudo!)
sudo ./cheatengine test
```

## Menu Options

```
[1] Select PID      - Choose which process to work with
[2] Scan value      - Search for a value in memory
[3] Diff scan       - Smart scan with address tracking
[4] Reset diff      - Clear saved addresses
[5] Write value     - Change a value at an address
[6] Refresh PIDs    - Update the process list
[0] Exit            - Quit the program
```

## Diff Scan (Recommended)

The **Diff scan** feature makes finding addresses much easier:

1. **First diff scan** - Enter the current value (e.g., 100 HP)
   - Saves all addresses containing that value

2. **Change value in game** - Take damage, now 90 HP

3. **Second diff scan** - Enter the new value (90)
   - Only shows addresses that changed from 100 → 90

4. **Repeat** until you find the exact address

### Example

```
[3] Diff scan (0 saved)
> 3
Enter value: 100
Saved 847 addresses

# Take damage in game...

[3] Diff scan (847 saved)
> 3
Enter value: 90
MATCH: 0x7fff1234 = 90
Remaining: 1

> 5
Enter address: 7fff1234
Enter new value: 9999
Done!
```

## How to Find the Right Address (Manual Method)

Finding the correct memory address takes multiple scans:

1. **First scan** - Player has 100 HP
   ```
   > 2
   Enter value: 100
   Found: 0x7fff1234 = 100
   Found: 0x7fff5678 = 100
   Found: 0x7fff9abc = 100
   Total matches: 3
   ```

2. **Change the value in the target** - Take damage, now 90 HP

3. **Second scan** - Search for new value
   ```
   > 2
   Enter value: 90
   Found: 0x7fff1234 = 90
   Total matches: 1
   ```

4. **Write new value** - Set health to 9999
   ```
   > 3
   Enter address (0x...): 0x7fff1234
   Enter new value: 9999
   Done!
   ```

5. **Check target program** - Health is now 9999!

## Multiple PIDs

If multiple processes have the same name, you can choose which one:

```
$ sudo ./cheatengine chrome
[0] PID 1234: chrome
[1] PID 5678: chrome
[2] PID 9012: chrome

> 1
Enter index [0-2]: 1
Selected PID 5678 (chrome)
Ready to scan PID 5678
```

## Requirements

- Linux (kernel 3.2+)
- Root privileges (`sudo`)
- GCC
