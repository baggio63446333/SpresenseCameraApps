#include <Camera.h>
#include <USBSerial.h>

USBSerial UsbSerial;

// Please change the serial setting for user environment
#define SERIAL_OBJECT   UsbSerial
#define SERIAL_BAUDRATE 921600

// Please select the display size
//int16_t width = CAM_IMGSIZE_QVGA_H,    height = CAM_IMGSIZE_QVGA_V;
int16_t width = CAM_IMGSIZE_VGA_H,     height = CAM_IMGSIZE_VGA_V;
//int16_t width = CAM_IMGSIZE_HD_H,      height = CAM_IMGSIZE_HD_V;
//int16_t width = CAM_IMGSIZE_QUADVGA_H, height = CAM_IMGSIZE_QUADVGA_V;
//int16_t width = CAM_IMGSIZE_FULLHD_H,  height = CAM_IMGSIZE_FULLHD_V;
//int16_t width = CAM_IMGSIZE_5M_H,      height = CAM_IMGSIZE_5M_V;
//int16_t width = CAM_IMGSIZE_3M_H,      height = CAM_IMGSIZE_3M_V;

void CamCB(CamImage img)
{
  static int toggle = 0;
  static uint64_t base_time = 0;

  // for debug
  uint64_t now = millis();
  printf("time= %lld [ms]\n", now - base_time);
  base_time = now;
  (toggle++ & 1) ? ledOn(LED1) : ledOff(LED1);

  if (img.isAvailable()) {
    send_jpeg(img.getImgBuff(), img.getImgSize());
    uint64_t tdiff = millis() - now;
    printf("time= %lld [ms] %.3lf [Mbps]\n", tdiff, (float)(img.getImgSize() * 8) / tdiff / 1000);
  }
}

void setup()
{
  CamErr err;

  Serial.begin(115200);
  SERIAL_OBJECT.begin(SERIAL_BAUDRATE);

  Serial.println("Prepare camera");
  err = theCamera.begin(2,
                        CAM_VIDEO_FPS_15,
                        width,
                        height,
                        CAM_IMAGE_PIX_FMT_JPG);
  assert(err == CAM_ERR_SUCCESS);

  /* Auto white balance configuration */
  //err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_AUTO);
  //err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_INCANDESCENT);
  //err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_FLUORESCENT);
  err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  //err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_FLASH);
  //err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_CLOUDY);
  //err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_SHADE);
  assert(err == CAM_ERR_SUCCESS);

  /* Start video stream */
  err = theCamera.startStreaming(true, CamCB);
  assert(err == CAM_ERR_SUCCESS);
}

void loop()
{
}

int send_jpeg(const uint8_t* buffer, size_t size)
{
  // Receive a start trigger '>'
  if (wait_char('>', 3000)) { // 3sec
    // Send a start code 'P'
    SERIAL_OBJECT.write('P'); // Payload
    // Send a binary data size in 4byte
    SERIAL_OBJECT.write((size >> 24) & 0xFF);
    SERIAL_OBJECT.write((size >> 16) & 0xFF);
    SERIAL_OBJECT.write((size >>  8) & 0xFF);
    SERIAL_OBJECT.write((size >>  0) & 0xFF);

    // Send binary data
    size_t sent = 0;
    do {
      size_t s = SERIAL_OBJECT.write(&buffer[sent], size - sent);
      if ((int)s < 0) {
        static int recover = 0;
        Serial.println(recover++);
        SERIAL_OBJECT.end();
        SERIAL_OBJECT.begin(SERIAL_BAUDRATE);
        return -1;
      }
      sent += s;
    } while (sent < size);

    // Receive a end trigger '<'
    if (wait_char('<', 3000)) {
      return 0;
    } else {
      return -1;
    }
  }
  return 0;
}

//
// Wait for a specified character with timeout
//
int wait_char(char code, int timeout)
{
  uint64_t expire = millis() + timeout;

  while (1) {
    if (SERIAL_OBJECT.available() > 0) {
      uint8_t cmd = SERIAL_OBJECT.read();
      if (cmd == code) {
        return 1;
      }
    }
    if (timeout > 0) {
      if (millis() > expire) {
        return 0;
      }
    }
  }
}
