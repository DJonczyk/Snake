# Snake Game

A classic snake game written in C, implemented using a doubly linked list data structure.

## Features

- Doubly linked list implementation for snake body
- Three difficulty levels (Easy, Medium, Hard)
- Pause functionality (press 'p')
- Score tracking
- ASCII-based graphics

## Requirements

### Windows
- GCC or MinGW compiler
- Windows API (included in Windows SDK)

### Linux/macOS
- GCC compiler
- ncurses library (for future cross-platform support)

## Building

### Using Make
```bash
make
```

### Manual compilation (Windows)
```bash
gcc -o snake.exe snake.c
```

## Running

```bash
./snake        # Linux/macOS
snake.exe      # Windows
```

Or use:
```bash
make run
```

## Controls

- Arrow Keys: Move the snake
- P: Pause/Resume game
- Use menu options to select difficulty and start game

## Project Structure

```
Snake/
├── snake.c       # Main game source code
├── Makefile      # Build configuration
├── README.md     # This file
└── .gitignore    # Git ignore rules
```

## Notes

- Currently optimized for Windows (uses `windows.h` for console manipulation)
- Future updates will include cross-platform support using ncurses
- Game uses ASCII characters for drawing the game board and snake

## TODO

- [ ] Cross-platform support (Linux/macOS using ncurses)
- [ ] Additional difficulty levels
- [ ] Key hints/instructions in-game
- [ ] High score persistence
