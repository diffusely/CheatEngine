# CheatEngine for Linux

A memory scanner and editor for Linux processes. Find and modify values in running games and programs.

## Features

- **Multiple Value Types** - Scan for `int`, `float`, or `double` values
- **Diff Scan** - Smart "changed value" scanning to narrow down addresses
- **Memory Write** - Change values at specific addresses
- **PID Selection** - Choose from multiple matching processes
- **Input Validation** - Error handling for invalid inputs

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
=== Type: int ===
[1] Select PID      - Choose which process to scan
[2] Scan value      - Search for a specific value
[3] Diff scan       - Find addresses where value CHANGED
[4] Reset diff      - Clear saved addresses
[5] Write value     - Modify a value at an address
[6] Refresh PIDs    - Update the process list
[7] Change type     - Switch between int/float/double
[0] Exit            - Quit the program
```

## Value Types

Press `[7]` to change the value type:

| Type | Size | Use For |
|------|------|---------|
| `int` | 4 bytes | Health, ammo, coins, score |
| `float` | 4 bytes | Speed, position, multipliers |
| `double` | 8 bytes | High-precision values |

## Diff Scan (Recommended Method)

The **Diff scan** finds addresses where the value **CHANGED**:

1. **First scan** - Enter the current value (e.g., health = 100)
   - Saves all addresses containing that value

2. **Change value in game** - Take damage, health becomes 95

3. **Second scan** - No input needed!
   - Shows addresses where value CHANGED (100 → 95)

4. **Repeat** until you find the exact address

### Example: Finding Health

```
=== Type: int ===
[3] Diff scan (0 saved)
Enter initial value (int): 100
Saved 5000 addresses
Now change the value in game and run diff scan again!

# Take damage in game... health is now 95

[3] Diff scan (5000 saved)
Scanning 5000 addresses for CHANGED values...
CHANGED: 0x7f1234 : 100 -> 95
CHANGED: 0x7f5678 : 100 -> 95
Found 2 changed addresses

# Take more damage... health is now 80

[3] Diff scan (2 saved)
CHANGED: 0x7f1234 : 95 -> 80
Found 1 changed addresses   <- This is your target!

[5] Write value
Enter address: 7f1234
Enter new value (int): 9999
Done!
```

### Example: Finding Speed (Float)

```
[7] Change type
> 2                         # Select float

[3] Diff scan (0 saved)
Enter initial value (float): 1.0
Saved 3000 addresses

# Start running/sprinting in game...

[3] Diff scan (3000 saved)
CHANGED: 0x623abc : 1.00 -> 1.50
Found 1 changed addresses

[5] Write value
Enter address: 623abc
Enter new value (float): 10.0
Done!                       # Now you're super fast!
```

## Tips for Finding Values

| Value Type | Common Starting Values |
|------------|------------------------|
| Health/HP | `100`, `1000`, `100.0` |
| Speed | `1.0`, `100.0`, `200.0` |
| Coins/Money | The exact number shown |
| Position | `0.0`, current X/Y coords |
| Multiplier | `1.0`, `1.5`, `2.0` |

## Requirements

- Linux (kernel 3.2+)
- Root privileges (`sudo`)
- GCC
- ncurses library (optional, for TUI version)
