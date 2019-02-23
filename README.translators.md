# How to translate Planet Blupi

## The interface

All texts in the interface are using gettext PO files format. The files are
located in the `resources/po/` directory. For adding a new translation, open
the `resources/po/planetblupi.pot` file with an editor like [poedit][1] for
example. It can be a bit difficult to understand the context without playing
with your translations but there is a trick. Once you are ready to test your
translations, save your `.po` file and the corresponding `.mo` file. Rename
the `.mo` file to `planetblupi.mo`. Go to the directory where is installed
Planet Blupi and look at (for replacing the french translation by yours for
example) `share/locale/fr/LC_MESSAGES/` and put your `.mo` file in this
directory.

> Under Windows OS, you should find the french translations in
> `C:\Program Files\Planet Blupi\share\locale\fr\LC_MESSAGES\`.  
> With macOS it's possible to open the `Planet Blupi.app` and update the
> file directly in the `.app`. But note that it breaks the application
> signature. To start the game, maybe it's necessary to open via the
> context menu for by passing the macOS security.  
> For Linux you must unpack the AppImage file somewhere for changing the
> files.

Then start the game, the interface will use your new translation if you select
"Français" in the global settings screen.

If you see some strange characters, like a square, then contact me and I will
add the necessary glyphs for your language. Planet Blupi is not using a real
font but just sprites with a limited range of characters.

## The missions

The missions are located in the `resources/data/` directory. Open the
`en/stories.blp` with a text editor like Notepad++, Visual Studio Code,
Sublime Text, etc, ... Ensure to work in UTF-8. Like for the interface,
you can replace for example the french file by yours in the
`share/planetblupi/data/fr/` directory.

## Some rules

1. The "Planet Blupi" title must not be translated excepted if your language is
   not using Latin-1 like Hebrew, Korean and Chinese for example.
2. If you plan to translate in Korean, ask me for details because I already
   have most translations but I must (still) draw all glyphs.
3. Like for the Hebrew translations; I can enable the reversed game layout
   (right to left reading) for your translation if necessary.

## The voices

It's not mandatory but it's very appreciate if you can provide the Blupi
voices; check this [README][2]. And if you don't know how to filter the sound
in order to produce a voice with an higher ton (like Blupi); contact me and
I will help you.

---

[1]: https://poedit.net/
[2]: README.voices.md
