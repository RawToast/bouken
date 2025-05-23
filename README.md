# Bouken

An ASCII-style roguelike game 🙀 built with ReasonML and React, organized as a Turborepo monorepo.

## Project Structure

This project has been restructured into a monorepo with the following packages:

```
├── apps/
│   ├── game/          # Game logic and rules (ReasonML)
│   └── ui/            # React frontend (ReasonML + React)
├── turbo.json         # Turborepo configuration
└── package.json       # Root workspace configuration
```

### Apps

- **`@bouken/game`** - Contains all the game logic, world management, enemy AI, and game mechanics
- **`@bouken/ui`** - Contains the React frontend that provides the user interface and renders the game

## Development Setup

### Prerequisites

- Node.js (v22 or higher)
- npm (v9 or higher)

### Installation

1. Install dependencies for all packages:

```bash
npm install
```

2. Build all packages:

```bash
npm run build
```

### Development Commands

```bash
# Start development servers for all apps
npm run dev

# Build all packages
npm run build

# Run tests across all packages
npm run test

# Run tests in watch mode
npm run test:watch

# Clean build artifacts
npm run clean
```

### Individual Package Commands

You can also work with individual packages:

```bash
# Work with the game package
cd apps/game
npm run build
npm run test

# Work with the UI package
cd apps/ui
npm run start
npm run build
```

## Architecture

### Game Package (`@bouken/game`)

The game package contains:

- **Types** - Core game type definitions
- **World** - World generation and management
- **Level** - Level/map management
- **Enemy** - Enemy AI and behavior
- **Game Logic** - Core game mechanics and rules

### UI Package (`@bouken/ui`)

The UI package contains:

- **React Components** - Game display and interaction
- **Game Views** - Different screens (start, game, score)
- **Input Handling** - Keyboard controls and events

The UI package imports game logic from `@bouken/game` using the BuckleScript/ReasonML module system.

## Technology Stack

- **ReasonML/BuckleScript** - Functional programming language that compiles to JavaScript
- **React** - UI framework
- **Parcel** - Bundler for the frontend
- **Turborepo** - Monorepo build system
- **Jest** - Testing framework

## Migration Notes

This project has been migrated from a single-package structure to a monorepo:

1. Game logic moved from `src/app/` to `apps/game/src/app/`
2. Tests moved from `src/test/` to `apps/game/src/test/`
3. UI components moved from `src/ui/` to `apps/ui/src/ui/`
4. Main app files moved to `apps/ui/src/`
5. Each package now has its own `package.json` and `bsconfig.json`

## Future Plans

- Convert UI package to TypeScript while keeping game logic in ReasonML
- Add more sophisticated build optimizations
- Potentially add more packages (e.g., shared utilities, data layers)
