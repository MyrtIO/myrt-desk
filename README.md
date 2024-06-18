# MyrtDesk

Firmware for Ikea Bekant based smart desk using Raspberry Pi Pico W.

## Features

- Control using [MyrtIO Protocol](https://github.com/MyrtIO/myrt_desk/tree/main/lib/MyrtIO) via UDP.
- Customizable backlight, various effects and animations.
- Adjusting the height of the desk.

## Installation

Assemble and connect the circuit. Connect the Pico W to your computer in firmware mode and run the commands in the terminal:

```sh
make configure
make flash
```
