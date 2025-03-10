# Exploding Kittens Multiplayer Game

## Overview
A networked multiplayer implementation of the Exploding Kittens card game using C, socket programming, and ncurses for the user interface.

## Features
- Real-time multiplayer gameplay
- Network-based multiplayer architecture
- Ncurses-powered interactive CLI
- Multiple card types with unique actions
- Game restart/rematch functionality

## Prerequisites
- GCC compiler
- ncurses library
- POSIX-compliant system

## Components
- `server.c`: Manages game sessions and client connections
- `client.c`: Handles client-side game interaction
- `game_logic.c`: Implements core game mechanics
- `game_logic.h`: Defines game structures and function prototypes

## Compilation
```bash
gcc -o server server.c game_logic.c -lpthread
gcc -o client client.c game_logic.c -lncurses
```

## Game Rules
- 2 players per game
- Objective: Avoid drawing the Exploding Kitten card
- Players can play various action cards to manipulate gameplay
- Special cards include Attack, Skip, Favor, Shuffle, etc.

## Gameplay Mechanics
- Draw cards from the deck
- Play cards with unique actions
- Use Defuse cards to survive Exploding Kitten draws
- Last player standing wins

## Networking
- Uses TCP sockets for client-server communication
- Supports multiple simultaneous game sessions

## License
Open-source project
