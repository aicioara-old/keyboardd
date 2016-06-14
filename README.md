# keyboardd

Keyboard daemon. When started, listens for keyboard input and calls callback on first keystroke

# Usage

`./keyboardd /dev/input/event3 callbackProgram.exe`

To find the keyboard event handler

`find /dev/input/by-path/ -iname '*kbd*' | xargs ls -l | grep -e 'event[0-9]$'`

# Sudo

Keyboard event files are owned by root. To work around this either

`sudo ./keyboardd /dev/input/event3 callbackProgram.exe`

or

```
sudo chown root:root keyboardd
sudo chmod u+s keyboardd
./keyboardd /dev/input/event3 callbackProgram.exe
```

# Installation

```bash
mkdir build
cd build
cmake ..
make
```