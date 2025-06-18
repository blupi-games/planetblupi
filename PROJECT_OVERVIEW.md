# Planet Blupi Project Overview

This document provides an overview of the Planet Blupi project, helping both users and developers understand the game and the codebase.

## What is Planet Blupi?

**Planet Blupi** is a strategy and adventure game, originally created by Epsitec SA. It's described as a "delirious spell-binding game" that subtly blends action with thought-provoking challenges. The game is designed for ages 12 to 99.

This version of the game includes:
*   Over 30 missions to complete.
*   A built-in editor that allows you to construct your own missions.

## For Users: Where to Start

If you want to understand how to play the game, its features, controls, and configuration options, the best place to start is the manpage file: `planetblupi.6.ronn`. This file can be found in the root of the repository and provides a comprehensive overview, including:

*   **Game Description:** What Planet Blupi is all about.
*   **Configuration:** How to set up the game, including display settings (fullscreen/windowed, resolution, renderer) and sound.
*   **Controls:** Details on mouse and keyboard commands for playing the game.
*   **Music:** Information about the game's music and how to customize it.
*   **Command-line Options:** Various options for launching and customizing the game experience.

## For Developers: Where to Start

If you're looking to understand the codebase or contribute to the project, here’s how to get started:

1.  **Main Development Repository:** The `README.md` file in this repository mentions that for active development, you should refer to the bundled repository located at `https://github.com/blupi-games/planetblupi-dev.git`. The current repository seems to serve more as a core or distribution point.
2.  **Source Code:** The game's source code is primarily located in the `src/` directory. It appears to be mainly written in C++.
3.  **Build System:** The project uses CMake to manage the build process. You'll find `CMakeLists.txt` in the root directory (and potentially in subdirectories), which defines how the code is compiled and linked.
4.  **Game Assets:** The `resources/` directory contains all the game's assets. This includes:
    *   `data/`: Game data, potentially including mission files (`.blp` files) and configuration (`config.json`).
    *   `fonts/`: Font files used in the game.
    *   `image/`: Image files for graphics and UI elements.
    *   `movie/`: Video files used in the game.
    *   `music/`: Music files (supports OGG and MIDI formats).
    *   `po/`: Localization files for translating the game into different languages.
    *   `sound/`: Sound effect files.
5.  **Original Versions:** The `README.md` also notes that you can find the "mostly" original source code from 1997 (versions 1.7 and 1.8) under the git tags `v1.7.0` and `v1.8.0`.

## Further Information

For more details, please refer to the specific files and directories mentioned above. The `planetblupi.6.ronn` manpage is particularly useful for understanding the game's user-facing aspects.
