# GUID Blocker

A plugin for CoD4X Server that blocks players from connecting based on their GUID.

## Features

- Blocks players before map loading
- Loads blocked GUIDs from `guidblocklist.txt`
- Case-insensitive GUID matching
- Stores up to 1024 blocked GUIDs

## Setup

1. Add GUIDs to `guidblocklist.txt` (one per line)
2. Compile and load the plugin on your CoD4X server
3. Blocked players will be disconnected before entering the game

## Build

```
make
```
