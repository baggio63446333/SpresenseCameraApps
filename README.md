# Spresense Camera Applications
## UsbLiveStreaming

The JPEG data captured by the `Spresense Camera` is transferred to a PC and displayed in real time using `Processing`. Sketches for both `Processing` and `Arduino` are provided.

### Requirements

- Hardware
    - [SPRESENSE Main Board](https://www.switch-science.com/catalog/3900/)
    - [SPRESENSE Extension Board](https://www.switch-science.com/catalog/3901/)
    - [SPRESENSE Camera Board](https://www.switch-science.com/catalog/4119/)
    - microUSB cable x 2 (One for `Main` and one for `Extension` board.)
- Software
    - [Processing](https://processing.org/download)
    - [Arduino IDE](https://www.arduino.cc/en/software)
    - [Spresense Arduino Package](https://developer.sony.com/develop/spresense/docs/arduino_set_up_en.html)
    - [USBSerial Library](https://github.com/baggio63446333/USBSerial)

### Procedure

#### Hardware preparation

1. Connect all of the boards, `Main`, `Extension` and `Camera` boards.
2. Connect `Main` board with your PC via microUSB cable.
3. Connect `Extension` board with your PC via microUSB cable.
#### Arduino

1. Launch `Arduino IDE`.
2. Open `Arduino/UsbLiveStreaming/UsbLiveStreaming.ino`.
3. Upload.

#### Processing

1. Launch `Processing`.
2. Open `Processing/UsbLiveStreaming/UsbLiveStreaming.pde`.
3. Change the serial setting `SERIAL_PORTNAME` for your environment which is a port name connected via USB CDC/ACM on `Extension` board.
4. Execute.
