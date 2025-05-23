# @bouken/ui

React-based user interface for the Bouken ASCII roguelike game.

## Overview

This package provides the frontend experience for the Bouken game, built with ReasonML and React. It consumes the game logic from `@bouken/game` and presents it through a web interface.

## Features

- **ASCII Game Rendering** - Displays the game world using ASCII characters
- **Keyboard Controls** - Full keyboard-based gameplay
- **Game Views** - Multiple screens (start screen, game, score screen)
- **Responsive UI** - Game stats, controls, and instructions

## Controls

The player can move in 8 directions with:

`q` `w` `e`

`a` 🐾 `d`

`z` `x` `c`

- Attack by moving toward enemies
- Use stairs/exits with `s`
- Wait a turn with `s` (when not on stairs/exit)

## Game Elements

- `O` Player
- `.` Ground
- `:` Tough terrain
- `w` Water
- `#` Wall
- `Z` Zombie
- `M` Minotaur
- `+` Health
- `g` Gold
- `,` Trap
- `;` Snare
- `/` Stairs
- `e` Exit

## Development

```bash
# Start development server
npm start

# Build for production
npm run build

# Build ReasonML only
npm run bsb

# Clean build artifacts
npm run clean
```

## Architecture

### Components

- **App** - Main application coordinator
- **StartView** - Game start screen
- **GameView** - Main game display
- **GameMap** - ASCII game map rendering
- **GameStats** - Player statistics display

### Dependencies

- Imports game logic from `@bouken/game`
- Uses React for UI components
- Parcel for bundling and development server

## Future Plans

This package will eventually be converted to TypeScript while maintaining the ReasonML game logic in the separate `@bouken/game` package.
