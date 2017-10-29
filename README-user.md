
# Planet Blupi (v1.9.x)

This version contains over 30 missions and a built in editor, which enables you
to construct your own missions.

## Configuration

| Category | Description |
| -------- | ----------- |
| OS       | At least GNU/Linux 2.6.32, macOS 10.9 or Microsoft Windows Vista   |
| VIDEO    | An 3D accelerated video card is highly recommended                 |
| SOUND    | Any sound card supported by ALSA, PulseAudio, macOS or DirectSound |
| INPUT    | Keyboard and mouse                                                 |

## Screen problems

What ever your screen resolution is, the game "Planet Blupi" always runs in
640x480 on full screen. If your screen does not support this mode, you may run
the game by default in windowed mode. Proceed as follows:

1. Quit the game _Planet Blupi_.
2. Open the file `<INSTALLDIR>/data/config.json` with a text editor.
3. Replace `fullscreen: true` by `fullscreen: false`.
4. Save and restart the game.

In this mode the screen is no more scrolled if the mouse touches the window
border. Use the Keyboard arrows instead.

## Mouse

Left button:
This button is always used in three steps:

1. Select a Blupi.
2. Click where you want him to act.
3. Click the button corresponding to the required operation.

If the chosen Blupi is already selected (blue or red circle around him),
step 1) is not necessary.

You may select several Blupis by maintaining the Shift button pressed.

Right button:
The right button is a shortcut. It orders the selected Blupi to do the most
useful operation on the selected spot.

## Keyboard

| Keys         | Description |
| ------------ | ----------- |
| <kbd>Arrows</kbd>                             | Scrolls the visible part of the scene.            |
| <kbd>Spacebar</kbd>                           | Shows or hides items.                             |
| <kbd>F1</kbd>                                 | Describes the goal without interrupting the game. |
| <kbd>F5</kbd>                                 | Normal speed (x1).                                |
| <kbd>F6</kbd>                                 | Double speed (x2).                                |
| <kbd>Home</kbd>                               | Returns to place of mission departure             |
| <kbd>Ctrl</kbd>+<kbd>F9</kbd>..<kbd>F12</kbd> | Bookmarks a spot.                                 |
| <kbd>F9</kbd>..<kbd>F12</kbd>                 | Returns to the corresponding bookmarked spot.     |
| <kbd>Pause</kbd>                              | Temporarily suspends the game.                    |

When you switch to an other application, the game is automatically paused.

## Epsitec SA

_Planet Blupi_ is an original creation of [Epsitec SA][1].

http://www.blupi.org  
info@blupi.org

[1]: http://www.epsitec.ch
