# csad2425ki405kostyukbogdan14

### Details about repo

This repository created for course "Computer systems automated design" of "Lviv Polytechnic National University".

### Task details

Create a rock, paper, scissors game.

### Student details

| Student number | Task | Config format|
| ------ | ---- | ------------ |
| 14 | rock paper scissors | JSON |

### Technical details

- Programming language: C++
- Hardware: ESP32

## Task 2

To upload code into the board:

1. Install [PlatformIO](https://platformio.org/)
2. Connect your ESP32
3. Run `pio run -t upload`, this will build and upload code to the board.

PlatformIO (`pio`) is smart enough to auto-detect board connection port. If you also need to view
the logs, add `-t monitor` to the upload command (`pio run -t upload -t monitor`). This will build
the project, upload the code and start monitoring serial port.

Then, to actually try the communication:

1. Connect to `ESP32` WiFi
2. Open `http://192.168.1.1` in your browser
3. Now, you are able to update the counter which stays in sync with server

### Details

Server is the source of truth. Client sends request to the server to update `State` counter and
returns html with new state, which client inserts into the page.

### Encountered issues

1. You may need to install driver for ESP32 usb from [Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads)
