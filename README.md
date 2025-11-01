<!-- Stats -->
<div align="center" style="text-decoration: none;">

<a href="https://github.com/Retroboi64/Project32/releases" style="text-decoration: none;">
  <img src="https://img.shields.io/github/v/release/Retroboi64/Project32?style=for-the-badge&logo=windows&logoColor=white&color=0078D4&labelColor=1a1a1a" alt="Version">
</a>
<a href="https://www.microsoft.com/windows" style="text-decoration: none;">
  <img src="https://img.shields.io/badge/Windows-Compatible-0078D4?style=for-the-badge&logo=windows&logoColor=white&labelColor=1a1a1a" alt="Platform">
</a>

<br>


<a href="https://github.com/Retroboi64/Project32/stargazers" style="text-decoration: none;">
  <img src="https://img.shields.io/github/stars/Retroboi64/Project32?style=for-the-badge&logo=starship&logoColor=white&color=ffd700&labelColor=1a1a1a" alt="Stars">
</a>
<a href="https://github.com/Retroboi64/Project32/network/members" style="text-decoration: none;">
  <img src="https://img.shields.io/github/forks/Retroboi64/Project32?style=for-the-badge&logo=git&logoColor=white&color=00d084&labelColor=1a1a1a" alt="Forks">
</a>
<a href="https://github.com/Retroboi64/Project32/issues" style="text-decoration: none;">
  <img src="https://img.shields.io/github/issues/Retroboi64/Project32?style=for-the-badge&logo=github&logoColor=white&color=ff4088&labelColor=1a1a1a" alt="Issues">
</a>
<a href="https://github.com/Retroboi64/Project32/commits" style="text-decoration: none;">
  <img src="https://img.shields.io/github/last-commit/Retroboi64/Project32?style=for-the-badge&logo=git&logoColor=white&color=9d4edd&labelColor=1a1a1a" alt="Last Commit">
</a>

<br>

<a href="https://buymeacoffee.com/retroboi64" style="text-decoration: none;">
  <img src="https://img.shields.io/badge/Support-Buy%20Me%20a%20Coffee-FFDD00?style=for-the-badge&logo=buymeacoffee&logoColor=000000&labelColor=1a1a1a" alt="Buy Me a Coffee">
</a>

</div>

<!-- End of stats -->

# Project32
> A compact yet powerful and flexible C++ Game Engine designed for modern development needs with multi-engine and multi-window support.

<img width="1867" height="794" alt="image" src="https://github.com/user-attachments/assets/d51802bc-d74a-4aed-8aad-952f2f2aadc7" />

## ⚠️ UNDER CONSTRUCTION
> **This project is currently under active development.** Many features are incomplete or subject to change. Use at your own risk for production applications.

## 🚀 Features
- **Multi-Engine Architecture**: Run multiple independent engine instances simultaneously
- **Multi-Window Support**: Each engine can manage multiple windows with independent rendering contexts
- **Easy Integration**: Simple API and comprehensive documentation *(Work in Progress)*
- **Hot-Reload Ready**: DLL-based architecture enables dynamic loading and reloading
- **Extensible**: Modular design allows for easy customization
- **Well-Tested**: Comprehensive test suite ensures reliability
- **Self-Contained**: All dependencies included in the repository

## 📋 Requirements
- **Operating System**: Windows *(macOS and Linux support coming soon)*
- **IDE**: Visual Studio 2022 with C++ development tools
- **C++ Standard**: C++20 or later
- **Dependencies**: Everything is included in the repository - no external setup required!

## 🛠️ Installation and Building
### Quick Method
```bash
# Clone the repository
git clone https://github.com/Retroboi64/Project32.git
# Navigate to project directory
cd Project32/Project32
# Open solution file
start Project32.sln
# After Visual Studio opens: Build → Build Solution (Ctrl+Shift+B)
```

### Alternative Method
1. Open **Visual Studio 2022**
2. **File** → **Open** → **Project/Solution**
3. Navigate to `Project32/Project32/Project32.sln`
4. **Build** → **Build Solution (Debug/Release — Only x64 is supported)** (or press **Ctrl+Shift+B**)

## 🎯 Use Cases
- Game Development: Build games with multiple viewport support
- Editor Tools: Create Unity/Unreal-style multi-window editors
- Simulation Software: Run multiple simulations simultaneously
- Debugging Tools: Split game and debug views into separate windows
- Multi-Monitor Setups: Leverage multiple displays effectively

## 🏗️ Architecture
### Multi-Engine System
Project32's unique multi-engine architecture allows you to:
- Run multiple independent engines in the same process
-  Each engine has its own window manager and render context
- Engines can run simultaneously or be controlled individually
- Perfect for complex editor scenarios or multi-view applications

### API Layers
- C API: Core exported functions for DLL interaction
- C++ Wrapper: RAII-style P32::Engine namespace for modern C++
- Instance Classes: EngineInstance and WindowInstance for object-oriented usage

## 📚 Documentation *(coming soon)*
For detailed documentation, examples, and API reference:
- 📖 [Documentation Site](link-to-docs) *(coming soon)*
- 📁 Check the `docs/` folder in this repository *(coming soon)*
- 💬 [Join our discussions](https://github.com/Retroboi64/Project32/discussions) for community support *(coming soon)*

## 🤝 Contributing
We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details on how to:
- 🐛 Report bugs
- 💡 Suggest new features  
- 🔧 Submit pull requests
- 📝 Follow our coding standards
  
## 📄 License
This project is licensed under a custom **MIT License with Attribution Requirements**.

### Key Points:
- ✅ **Free** for personal and commercial use
- ✅ **Modify and distribute** as needed
- ⚠️ **Attribution required** in multiple locations (credits, documentation, store pages)
- ⚠️ **GitHub link required** in all attributions
- ⚠️ **Commercial products** must display attribution before purchase
See the [LICENSE](LICENSE) file for complete details.

## 👨‍💻 Author
**Patrick Reese (Retroboi64)**
- 🐙 GitHub: [@Retroboi64](https://github.com/Retroboi64)
- 🚀 Project: [Project32](https://github.com/Retroboi64/Project32)

## ⭐ Support the Project
If you find Project32 useful, please consider:
- ⭐ **Giving it a star**
- 👤 **Following [@Retroboi64](https://github.com/Retroboi64)**
- 📢 **Sharing it with others**
- 🤝 **Contributing to the project**
- 💬 **Joining the discussion**

## 🔗 Quick Links
- [🐛 Issues](https://github.com/Retroboi64/Project32/issues)
- [🔧 Pull Requests](https://github.com/Retroboi64/Project32/pulls)
- [📦 Releases](https://github.com/Retroboi64/Project32/releases)
- [📚 Wiki](https://github.com/Retroboi64/Project32/wiki)
- [💬 Discussions](https://github.com/Retroboi64/Project32/discussions)
---
*Made with ❤️ by [Retroboi64](https://github.com/Retroboi64)*
