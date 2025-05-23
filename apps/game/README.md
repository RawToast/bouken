# @bouken/game

Game logic and mechanics for the Bouken ASCII roguelike game.

## Overview

This package contains all the core game functionality written in ReasonML:

- **Game mechanics** - Player movement, combat, progression
- **World management** - Level generation, world state
- **Enemy AI** - Enemy behavior and interactions
- **Game rules** - Scoring, win/lose conditions
- **Asset loading** - CSV-based world data loading

## Key Modules

- **Types** - Core type definitions for the entire game
- **World** - World and level management
- **Level** - Individual level/map handling
- **Enemy** - Enemy AI and behavior
- **Bouken** - Main game logic coordinator
- **Pathing** - Movement and pathfinding algorithms
- **Gameloop** - Turn-based game state updates

## Development

```bash
# Build the game logic
npm run build

# Run tests
npm run test

# Watch for changes
npm run watch

# Clean build artifacts
npm run clean
```

## Testing

The package includes comprehensive tests for:

- Game mechanics
- World generation
- Enemy behavior
- Level management
- Pathing algorithms

## Usage

This package is consumed by the `@bouken/ui` package. When built, it exposes all necessary modules through the `BoukenGame` namespace for import by other packages in the monorepo.
