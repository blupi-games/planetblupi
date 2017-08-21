
# Planet Blupi (v1.9)

This version contains over 30 missions and a built in editor, which enables you
to construct your own missions.

## Configuration

| Category | Description |
| -------- | ----------- |
| OS       | At least GNU/Linux 2.6.32, macOS 10.7 or Microsoft Windows Vista |
| VIDEO    | An 3D accelerated video card is highly recommended               |
| SOUND    | Any sound card supported by ALSA, sndio, macOS or DirectSound    |
| INPUT    | Keyboard and mouse                                               |

## Screen problems

What ever your screen resolution is, the game
"Planet Blupi" always runs in 640x480 on full screen. If your screen does not
support this mode, you may run the game in windowed mode. Proceed as follows:

- 1. Quit the game "Planet Blupi".
- 2. Open the file  "<INSTALLDIR>/data/config.json" with a text editor.
- 3. Replace `fullscreen: true` by "fullscreen: false".
- 4. Save and restart the game.

In this mode the screen is no more scrolled if the mouse touches the window
border. Use the Keyboard arrows instead.

## Mouse

Left button:
This button is always used in three steps:

- 1. Select a Blupi.
- 2. Click where you want him to act.
- 3. Click the button corresponding to the required operation.

If the chosen Blupi is already selected (blue or red circle around him),
step 1) is not necessary.

You may select several Blupis by maintaining the Shift button pressed.

Right button:
The right button is a shortcut. It orders the selected Blupi to do the most
useful operation on the selected spot.

## Keyboard

| Keys         | Description |
| ------------ | ----------- |
| Arrows       | Scrolls the visible part of the scene.            |
| Spacebar     | Shows or hides items.                             |
| F1           | Describes the goal without interrupting the game. |
| F5           | Normal speed (x1).                                |
| F6           | Double speed (x2).                                |
| Home         | Returns to place of mission departure             |
| Ctrl+F9..F12 | Bookmarks a spot.                                 |
| F9..F12      | Returns to the corresponding bookmarked spot.     |
| Pause        | Temporarily suspends the game.                    |

When you switch to an other application, the game is automatically paused.

## EPSITEC

Planet Blupi is an original creation of EPSITEC SA.

http://www.blupi.org
info@blupi.org
