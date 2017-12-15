
# Planet Blupi

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
2. Open the file `share/planetblupi/data/config.json` with a text editor.
3. Replace `fullscreen: true` by `fullscreen: false`.
4. Save and restart the game.

In this mode the screen is no more scrolled if the mouse touches the window
border. Use the Keyboard arrows instead.

> It's possible to start the game with `--fullscreen off` instead of editing
> the `config.json` file.

If the problem persists, maybe you should try to start the game with the software
renderer. Edit the `config.json` file and add `"renderer": "software"` or start
the game with `--renderer software`.

On Windows, an other possibility exists. You can try to change the driver used
by the software renderer. By default it uses `"direct3d"`, you can change by
`"opengl"` with `"driver": "opengl"` with the `config.json` file or
`--driver opengl` by command line argument.

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

## Music

The music is provided in OGG and MIDI formats. The OGG variant is prefered
because the render is the same for all platforms. For purist fanboys, it's
possible to enforce the MIDI variant by starting the game with the
`--restore-midi` command line argument. Note that in this case, it's possible
to lose some instruments, it depends of your system.

> You can edit the `share/planetblupi/data/config.json` file and add an entry
> `"restoremidi": true`, or simply use the new settings available in the global
> settings screen in the game.

If you want, you can overload all musics with yours by providing OGG or MIDI
music files in your user directory.

On Windows, type `%APPDATA%\Epsitec SA\Planet Blupi` in the file browser,
on macOS, look at `~/Library/Application Support/Epsitec SA/Planet Blupi`, and
on Linux, look at `~/.local/share/Epsitec SA/Planet Blupi`.

You can create a `music` sub-directory with your music. For example:  
`%LOCALAPPDATA%\Epsitec SA\Planet Blupi\music\music000.ogg`.

Only 10 musics are supported: `music000` to `music009`.

## Epsitec SA

_Planet Blupi_ is an original creation of [Epsitec SA][1].

http://www.blupi.org  
info@blupi.org

[1]: http://www.epsitec.ch
