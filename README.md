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

## Import records

To import records you can use need to provide absolute path to json file with format

```json
{
  ...
  "items": [
    {
      "name": "name",
      "login": {
        "username": "username",
        "password": "password"
      },
      ...
    },
    ...
  ],
  ...
}
```

Other fields will be discarded
(Records can be imported from Bitwarden)

## Export records

To export records you need to provide absolute path to file to export to
Records will be exported in JSON with format

```json
{
  "items": [
    {
      "name": "name",
      "login": {
        "username": "username",
        "password": "password"
      }
    }
  ]
}
```

## To develop

Install ```clang-format``` and add git hooks ```git config --local core.hooksPath .githooks/```
