<h1 align="center">
  <br>
  <a href="https://github.com/rbl-dot/sakura"><img src="https://i.imgur.com/WFsf63R.png" alt="Sakura" width="200"></a>
  <br>
  Sakura
  <br>
</h1>

<div align="center">
  <img src="https://img.shields.io/badge/License-MIT-green">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-red">
</div>

<h4 align="center">Sakura is a free and public cheat for Counter-Strike 1.6 written in <b>C++</b>.</h4>

<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#how-to-use">How To Use</a> •
  <a href="#known-bugs">Known bugs</a> •
  <a href="#to-do">To do</a> •
  <a href="#credits">Credits</a> •
  <a href="#support">Support</a> •
  <a href="#license">License</a>
</p>

![screenshot](https://i.imgur.com/1pD62e6.png)

## Key Features

* Modern and User-Friendly Interface: Sakura features a sleek and intuitive menu interface that is easy to navigate. It provides a seamless user experience and makes it easy to access all the features.
* Customizable Colors: With Sakura, you can customize the menu colors to your liking. This means you can personalize the interface and make it match your own preferences.
* Powerful LUA Scripting: Sakura is built on top of the powerful LUA scripting language, which allows for complex and dynamic interactions. This means you can create custom scripts that automate tasks, enhance functionality, and improve performance. If you need help with scripting, check out our [documentation](https://sakura-9.gitbook.io/sakura/).
* Bypasses: Sakura includes several bypasses that allow you to bypass various anti-cheat systems. These bypasses are constantly updated to ensure that they work with the latest versions of anti-cheat software.

## How To Use

To run this cheat, you'll need [Git](https://git-scm.com) or downloaded source code and installed [Visual Studio](https://visualstudio.microsoft.com/) on your computer and basic knowledge how to compile it. Also for the loader you'll need DirectX SDK from [here](https://www.microsoft.com/en-us/download/details.aspx?id=6812).

It's compilable in Visual Studio 2022 and 2019.

```bash
$ git clone --recurse-submodules https://github.com/rbl-dot/sakura.git
```

## Known bugs

* Fov for Perfect Silent doesn't work.
* CreateBeamPoint is not working from Lua script.
* First snapshot is double taken while using anti screenshot function.

## To-do

* Improve Lua functions.
* Add Save/Load functions for Lua.
* Improve UI customization.
* Fix bugs.

## Credits

- [BloodSharp](https://github.com/BloodSharp)
- [Imgui](https://github.com/ocornut/imgui)
- [Lua](https://www.lua.org/)
- [LuaBridge](https://github.com/vinniefalco/LuaBridge)
- [Detours](https://github.com/microsoft/Detours)

## Support

We welcome contributions to Sakura from anyone, whether you're an experienced developer or just getting started. Here are some ways you can help:

* Make Pull Requests: If you have made changes to the code that you would like to share, consider making a pull request.
* Report Issues: If you encounter any issues or bugs while using Sakura, please report them on our [GitHub repository](https://github.com/rbl-dot/sakura/issues).

## License

Sakura is licensed under the MIT License. See [LICENSE](https://github.com/rbl-dot/sakura/blob/main/LICENSE) for more information.
