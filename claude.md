# Claude Context - Snake Game Project

## Project Overview

This is a Windows-native Snake game implementation written in C, using a doubly linked list data structure to manage the snake's body. The project is written in Polish (menu text, comments) and targets Windows exclusively using the Windows Console API.

**Language:** C (C99)
**Platform:** Windows only
**Primary File:** `snake.c` (419 lines)
**Build System:** Makefile (configured for Windows)

## Architecture

### Data Structures

#### `struct point`
Simple 2D coordinate structure:
```c
struct point {
    int x, y;
}
```
Used for food/target positions.

#### `struct node`
Doubly linked list node representing one segment of the snake:
```c
struct node {
    struct node *prev;    // Points to previous segment (toward tail)
    struct node *next;    // Points to next segment (toward head)
    int x, y;            // Current position
    int _x, _y;          // Next position (used during movement)
}
```

#### `struct snake`
Main game state:
```c
struct snake {
    struct node *head;    // Snake's head (front)
    struct node *tail;    // Snake's tail (end)
    long int score;       // Current score
    short int count;      // Number of segments
    short int last_x, last_y;  // Last tail position (for cleanup)
    bool field[15][25];   // Grid occupancy map
    bool status;          // Game over flag (false = game over)
}
```

### Map/Grid System

- **Dimensions:** 15x25 (defined by `map_size_x` and `map_size_y`)
- **Coordinates:** [0-14][0-24]
- **Border:** Outer edge (row 0, row 14, col 0, col 24)
- **Playable area:** Inner cells [1-13][1-23]
- **Character array:** `char tab[15][25]` stores ASCII characters for rendering

### ASCII Characters Used

- `201` (╔) - Top-left corner
- `187` (╗) - Top-right corner
- `200` (╚) - Bottom-left corner
- `188` (╝) - Bottom-right corner
- `205` (═) - Horizontal border
- `186` (║) - Vertical border
- `178` (▓) - Snake body
- `158` (₧) - Food/point
- `255` (non-breaking space) - Empty cell

## Key Functions

### Initialization

**`board_init(char (*ptr)[25])`** (snake.c:194)
- Initializes the game board with borders and empty spaces
- Sets up ASCII box-drawing characters

**`snake_init(struct snake *s)`** (snake.c:222)
- Creates initial snake with single segment at position (5, 5)
- Initializes all game state variables
- Sets up the occupancy field array

**`snake_set(char (*ptr)[25], struct snake *s)`** (snake.c:264)
- Places the snake head on the board

### Game Loop Logic

**`movement(char (*ptr)[25], struct snake *s, int move_x, int move_y)`** (snake.c:269)
- Core movement logic - handles all snake segment updates
- **Collision detection:**
  - Boundary check: snake.c:271
  - Self-collision: snake.c:320
- **Movement algorithm:**
  - Updates head position with move_x/move_y delta
  - Propagates positions backward through linked list
  - Each segment moves to where the previous segment was
  - Tail position is cleared from the board
- **Important:** Uses both current (x, y) and next (_x, _y) positions for smooth updates

### Food/Point System

**`random_point(struct snake *s, struct point *p, char (*ptr)[25])`** (snake.c:332)
- Generates random coordinates within playable area
- Recursively retries if position overlaps with snake (using `s->field[][]`)

**`compare_point(struct snake *s, struct point *p)`** (snake.c:345)
- Checks if snake head has reached the food

**`append(struct snake *s, struct point *p, short int difficulty)`** (snake.c:352)
- Adds new segment to tail when food is eaten
- Calculates score based on difficulty level:
  - Easy (18): `score += (count * 0.4) * (difficulty * 0.1)`
  - Medium (14): `score += (count * 0.4) * (difficulty * 0.2)`
  - Hard (10): `score += (count * 0.4) * (difficulty * 0.4)`

### Rendering

**`board_print(char (*ptr)[25], COORD begin)`** (snake.c:247)
- Prints entire board to console using SetConsoleCursorPosition
- Called every game tick

### Timing

**`wait(short int difficulty)`** (snake.c:258)
- Frame delay using busy-wait with `clock()`
- Difficulty values (lower = faster):
  - Easy: 18 (180ms)
  - Medium: 14 (140ms)
  - Hard: 10 (100ms)
- Formula: `difficulty * CLOCKS_PER_SEC/100`

### Input Handling

Uses Windows Console API functions:
- **`kbhit()`** - Non-blocking key press detection
- **`getch()`** - Gets pressed key code

**Key codes:**
- 72: Up arrow
- 80: Down arrow
- 75: Left arrow
- 77: Right arrow
- 112: 'p' (pause)

**Input logic (snake.c:134-148, 170-183):**
- Checks for input before AND after movement
- Prevents 180° turns: `abs(ch-last)==8 || abs(ch-last)==2`
- Pause functionality: Loops until 'p' pressed again

### Menu System

**`menu()`** (snake.c:382)
- Displays main menu with colored background
- Uses `SetConsoleTextAttribute` for styling

**`menu_option()`** (snake.c:393)
- Waits for valid menu choice (1, 2, or 3)

**`menu_difficulty()`** (snake.c:410)
- Shows difficulty selection menu

## Windows API Dependencies

Critical Windows-specific functions used:

1. **Console handles:**
   - `GetStdHandle(STD_OUTPUT_HANDLE)` - Get console handle
   - Stored in global `HANDLE wHnd`

2. **Console manipulation:**
   - `SetConsoleTitle()` - Sets window title
   - `SetConsoleWindowInfo()` - Sets window size
   - `SetConsoleScreenBufferSize()` - Sets buffer dimensions
   - `SetConsoleCursorPosition()` - Moves cursor for rendering
   - `SetConsoleTextAttribute()` - Sets text/background colors

3. **Input:**
   - `kbhit()` - Check for key press (conio.h)
   - `getch()` - Get character without echo (conio.h)

4. **Audio:**
   - `Beep(frequency, duration)` - Sound effects

5. **Types:**
   - `COORD` - Console coordinate structure
   - `SMALL_RECT` - Rectangle structure
   - `HANDLE` - Console handle type

## Build Configuration

**Makefile notes:**
- Currently set up for Windows compilation only
- Uses GCC with flags: `-Wall -Wextra -std=c99`
- No additional libraries needed on Windows
- Output: `snake.exe`

**Commands:**
- `make` - Build the executable
- `make clean` - Remove build artifacts
- `make run` - Build and run the game

## Main Loop Structure (snake.c:132-185)

```
while (!snake->status) {
    1. Check for input (kbhit/getch)
    2. Handle pause if 'p' pressed
    3. Validate input (no 180° turns)
    4. Execute movement based on direction
    5. Render board
    6. Check if food eaten -> append segment
    7. Wait for frame delay
    8. Check for input again (improved responsiveness)
    9. Display score
}
```

## Known Issues & Implementation Notes

1. **Memory Management:**
   - Snake nodes are malloc'd but never explicitly freed (except for main struct)
   - Should implement cleanup function to free all nodes

2. **Rare Bug Fix:**
   - Line 159: `set_point()` called after `board_print()` to fix food disappearing

3. **Console Setup:**
   - Window size: 26x18
   - Buffer size: 26x18
   - Title: "Snake v1.0"

4. **Game Over Conditions:**
   - Hit boundary wall
   - Hit own body
   - Both trigger: `Beep(100, 400)` and `status = true`

## Polish Language Strings

Menu and messages are in Polish:
- "Nowa gra" - New game
- "Poziom trudnosci" - Difficulty level
- "Wyjscie" - Exit
- "Latwy" - Easy
- "Sredni" - Medium
- "Trudny" - Hard
- "Przegrales!" - You lost!
- "Wynik:" - Score:

## File Structure

```
/home/user/Snake/
├── snake.c          # Main game source (419 lines)
├── Makefile         # Build configuration
├── README.md        # User documentation
├── .gitignore       # Git ignore rules
└── claude.md        # This file (AI context)
```

## Development Notes

### Current TODOs from source (snake.c:11-17):
- Critical: (none listed)
- Optional:
  - Multiple levels/stages
  - Key hints/help text in game

### Git Information
- Current branch: `claude/initial-setup-011CUNTebH71o2AULLSYvyzt`
- Repository is clean after initial setup

## Important Code Locations

- Main game loop: snake.c:132-185
- Movement algorithm: snake.c:269-330
- Collision detection: snake.c:271, snake.c:320
- Input handling: snake.c:134-148, 170-183
- Score calculation: snake.c:374-379
- Initialization sequence: snake.c:122-129

## When Making Changes

1. **Always test on Windows** - This code will not compile/run on Linux/macOS
2. **Preserve ASCII characters** - They're integral to the visual design
3. **Be careful with the doubly linked list** - Movement logic is complex and fragile
4. **Memory leaks exist** - Consider cleanup when adding features
5. **Polish language** - Keep UI text in Polish unless requested otherwise
6. **Frame timing** - Difficulty values affect game speed directly
