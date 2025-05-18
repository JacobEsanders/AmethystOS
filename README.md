# AmethystOS

AmethystOS is an educational operating system based on XINU architecture with an amethyst theme. This project serves as a learning platform for operating system concepts and implementation.

## Features

### Core System
- Custom bootloader with multiboot support
- Memory management with paging
- Process scheduling and context switching
- Interactive shell with command history
- Amethyst-themed UI elements

### Device Drivers
- VGA text mode driver with color support
- PS/2 keyboard driver with full layout support
- LED control (Caps Lock, Num Lock, Scroll Lock)
- Support for extended keyboard keys

### File System
- Hierarchical directory structure
- Basic file operations (create, read, write, delete)
- Directory operations (mkdir, rmdir, list)
- Path manipulation (absolute/relative paths)
- File permissions (read/write/execute)
- File descriptors for I/O operations

### Package Manager (amy)
The `amy` package manager provides basic software management:
- Package installation and removal
- Dependency management
- Package database
- Search functionality
- Package information display

Commands:
```
amy install <package>  - Install a package
amy remove <package>   - Remove a package
amy update [package]   - Update packages
amy list              - List installed packages
amy search <query>    - Search for packages
amy info <package>    - Show package information
```

### Shell Commands
- File Operations: ls, cd, pwd, mkdir, rmdir, touch, rm
- Process Management: ps, kill
- System Information: meminfo
- UI Customization: clear, color
- Package Management: amy

## Project Structure

- `boot/` - Bootloader and low-level initialization
- `kernel/` - Core kernel components
- `system/` - System services (memory, process, filesystem)
- `device/` - Device drivers
- `include/` - Header files
- `lib/` - Library functions
- `shell/` - Shell implementation
- `doc/` - Documentation

## Building

1. Prerequisites:
   - GCC cross-compiler (i686-elf target)
   - GNU Make
   - NASM assembler
   - GNU Binutils

2. Build Commands:
   ```sh
   make           # Build the operating system
   make clean     # Clean build files
   ```

## Development

The system is designed to be educational and extensible. Key components are modular and well-documented. The amethyst theme is consistently applied throughout the user interface.

### Adding New Features
1. Device Drivers: Add new files in `device/`
2. System Services: Implement in `system/`
3. Shell Commands: Add to `shell/`
4. Packages: Register in the package database

## License

MIT License

Copyright (c) 2025 AmethystOS

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
