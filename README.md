# Simple local password manager with TUI 

## How to install locally 

### Needed libraries

```sh
sqlite3 libsqlite3-dev libssl-dev
```

#### For copy to work on linux install app for your display manager

Wayland  ```wl-clipboard```

X11 ```xclip```

### How to build

To build release version run
```sh
cmake --preset release && cd build && make
```

To build debug version run
```sh
cmake --preset debug && cd build_debug && make
```

## How to use 

To start the app run ```password-manager``` in the terminal
