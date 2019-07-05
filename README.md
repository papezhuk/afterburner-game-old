Afterburner
===========

*(Working title)*

Open-source reboot of [James Bond 007: Nightfire](https://en.wikipedia.org/wiki/James_Bond_007:_Nightfire). The primary purpose of this mod is to recreate and improve the experience of the PC version of Nightfire, and to provide in the process an open-source codebase that will allow for extension and refinement. The Xash3D (GoldSrc) engine was chosen because its codebase is fully available, it's smaller and simpler than Source, it's closer to the genuine Nightfire engine, and it doesn't come with the expectation that games will be photorealistic (I don't have the time or the energy as a single hobbyist to be working on some sort of HD photorealistic remake).

## I want to play, where are the downloads?

See [here](https://github.com/x6herbius/afterburner-game/releases). Please note, however, that at time of writing only pre-release versions of the game are available for testing purposes. These are liable to be buggy and will not be representative of the final game.

## I want to compile the game from source, how do I do that?

Ensure that you have Python 3 and CMake installed on your computer, as well as the appropriate C/C++ development tools for your platform (Windows, Mac and Linux are supported). You will also need to ensure that the **32-bit development** version of the [SDL2 library](https://www.libsdl.org/download-2.0.php) is installed. If this is not available for download for your platform, you should compile it from source.

*Note that 64-bit builds of the game will be supported in future, but are not at the moment.*

Clone this repository using `git clone --recursive`. If you don't use the `--recursive` switch, the game **will not** compile without manually updating the subrepositories.

To build, open a terminal in the root of the repository and call:

```
python3 build.py
```

This will build a release version of the game. The binaries and content can be found in `build/engine`.

If you downloaded SDL2 from the website, you will need to pass the path to the directory you extracted it to:

```
python3 build.py --sdl2 C:\path\to\sdl2
```

To build for debugging, pass `-c debug` in addition. For a list of other options, run `build.py --help`.

## I want to contribute!

Since this project is currently only in active development by myself, I don't have any set roadmap or procedure for contributing. If you would like to get involved, please drop me a line (jonathan.poncelet@talk21.com, or @x6herbius on Twitter) and we can discuss!

## Credits

Many thanks to:

* Gearbox for the original Nightfire assets
* The FWGS team for the [Xash3D engine](https://github.com/FWGS/xash3d-fwgs) and [Xash3D version of Half Life SDK](https://github.com/FWGS/hlsdk-xash3d)
* j-yaniv and others for [Rho-Bot Half Life bots](https://sourceforge.net/projects/rhobot/)
* Developers of [STB libraries](https://github.com/nothings/stb)
* Contributors to [XeNTaX game reverse-engineering forums](https://forum.xentax.com/)
* Various Nightfire modders over the years whose tools and reverse-engineering have proven invaluable when developing this mod (specifically Tres, 009, Ford, UltimateSniper).