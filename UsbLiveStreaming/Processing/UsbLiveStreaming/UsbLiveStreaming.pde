import java.util.Date;
import java.text.SimpleDateFormat;
import processing.serial.*;

Serial serial;

// Please change the serial setting for user environment
final String SERIAL_PORTNAME = "/dev/cu.usbmodem01";
final int    SERIAL_BAUDRATE = 921600;

void setup()
{
  // Please select the display size specified for a spresense sketch
  //size(320, 240);   // QVGA
  size(640, 480);   // VGA
  //size(1280, 960);  // QUADVGA
  //size(1280, 720);  // HD
  //size(1920, 1080); // FULLHD
  //size(2560, 1920); // 5M
  //size(2048, 1536); // 3M

  serial = new Serial(this, SERIAL_PORTNAME, SERIAL_BAUDRATE);
}

void draw()
{
  draw_jpeg();
}

// for debug
int count = 0;
int base_time = 0;

void draw_jpeg()
{
  int size = 0;

  // Send a start trigger '>'
  serial.write(">");
  // Receive a start code 'P'
  if (1 == wait_char('P', 3000)) {
    // Receive a binary data size in 4byte
    size = serial.read()<<24 | serial.read()<<16 | serial.read()<<8 | serial.read();
  } else {
    println("recover1");
    recover();
    return;
  }

  // illegal size
  if (size <= 0) {
    serial.clear();
    return;
  }

  // for debug
  int now = millis();
  println(count++, ": size=", size, "time=", now - base_time, "[ms]");
  base_time = now;

  // Receive binary data
  byte [] data = new byte[size];
  int timeout = millis() + 5000;
  for (int i = 0; i < size; ) {
    if (serial.available() > 0) {
      data[i] = (byte)serial.read();
      i++;
    } else {
      if (millis() > timeout) {
        println("recover2");
        recover();
        return;
      }
      delay(50);
    }
  }

  // Save data as a JPEG file and display it
  saveBytes(dataPath("sample.jpg"), data);
  PImage img = loadImage(dataPath("sample.jpg"));
  image(img, 0, 0);

  // Send a end trigger '<'
  serial.write("<");
}

//
// Wait for a specified character with timeout
//
int wait_char(char code, int timeout)
{
  int expire = millis() + timeout;

  while (true) {
    if (serial.available() > 0) {
      int cmd = serial.read();
      if (cmd == code) {
        return 1;
      }
    } else {
      delay(50);
    }
    if (timeout > 0) {
      if (millis() > expire) {
        return 0;
      }
    }
  }
}

//
// recover any error
//
void recover()
{
  serial.clear();
  serial.stop();
  delay(1000);
  while (true) {
    serial = new Serial(this, SERIAL_PORTNAME, SERIAL_BAUDRATE);
    if (serial != null) {
      break;
    }
    delay(1000);
  }
}

void mousePressed()
{
  String filename = "save-" + timestamp() + ".jpg";
  save(dataPath(filename));
  println("Saved " + filename);
}

String timestamp()
{
  Date date = new Date();
  SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd-HHmmss-SSS");
  return sdf.format(date);
}
