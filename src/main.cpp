#include <Arduino.h>
#include <bitset>
#include <HardwareSerial.h>
#define VERBOSE_TOOLS 1
#define VERBOSE_TAIL 1
#define VERBOSE_PERINTAH_PC 1

char perintah_pc = '0';

// dari nano, dibaca f103 ke pc
String allChar;
bool parsing_serial3 = false;
String buffer_inputSerial3 = "";

// pin input
#define INPUT_LOCK_ATC PB5
#define INPUT_TAIL_DEPAN PB9
#define INPUT_TAIL_BELAKANG PB8
#define INPUT_PEDAL PB3
// pin output
#define OUTPUT_LOCK_ATC PB14
#define OUTPUT_CW_ATC PB15
#define OUTPUT_TAIL_JEMPUT PC15
#define OUTPUT_TAIL_TINGGAL PA15

// init hardware serial Rx Tx
HardwareSerial Serial3(PB11, PB10);

void setup()
{
  Serial.begin(115200);
  Serial3.begin(115200);

  // pinmode output
  pinMode(OUTPUT_LOCK_ATC, OUTPUT);
  pinMode(OUTPUT_TAIL_JEMPUT, OUTPUT);
  pinMode(OUTPUT_TAIL_TINGGAL, OUTPUT);
  pinMode(OUTPUT_CW_ATC, OUTPUT);
  // pinmode input
  pinMode(INPUT_LOCK_ATC, INPUT_PULLUP);
  pinMode(INPUT_TAIL_DEPAN, INPUT_PULLUP);
  pinMode(INPUT_TAIL_BELAKANG, INPUT_PULLUP);
  pinMode(INPUT_PEDAL, INPUT_PULLUP);

  // buat semua menjadi low
  digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_JEMPUT), LOW);
  digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_TINGGAL), LOW);
  digitalWriteFast(digitalPinToPinName(OUTPUT_CW_ATC), LOW);
  digitalWriteFast(digitalPinToPinName(OUTPUT_LOCK_ATC), LOW);

  Serial.println("Init... ");
  delay(3000);
}

void verbose_output()
{
  // untuk proxy dan lainnya
  Serial.print("T:");

  Serial.print(digitalReadFast(digitalPinToPinName(INPUT_LOCK_ATC)) ? "U" : "L");
  Serial.print(!digitalReadFast(digitalPinToPinName(INPUT_TAIL_DEPAN)) ? "D" : "");
  Serial.print(!digitalReadFast(digitalPinToPinName(INPUT_TAIL_BELAKANG)) ? "B" : "");
  Serial.print(!digitalReadFast(digitalPinToPinName(INPUT_PEDAL)) ? "P" : "");

// untuk tools
#ifdef VERBOSE_TOOLS
  Serial.print("|P:");
  Serial.print(buffer_inputSerial3);
#endif

// VERBOSE tulis kembali perintah pc
#ifdef VERBOSE_PERINTAH_PC
  Serial.print("|R:");
  Serial.print(perintah_pc);
#endif

  Serial.println(); // enter, baris selanjutnya
}

void loop()
{
  // Periksa apakah ada data yang tersedia dari Serial
  if (Serial.available() > 0)
  {
    // Baca data dari Serial
    String input = Serial.readStringUntil('\n');
    input.trim(); // Hilangkan spasi dan karakter newline

    if (input.equalsIgnoreCase("J"))
    {
      digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_JEMPUT), HIGH);
      digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_TINGGAL), LOW);
      perintah_pc = 'J';
    }
    // tinggal tail stock
    else if (input.equalsIgnoreCase("T"))
    {
      digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_JEMPUT), LOW);
      digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_TINGGAL), HIGH);
      perintah_pc = 'T';
    }
    // stop tail stock
    else if (input.equalsIgnoreCase("S"))
    {
      digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_JEMPUT), LOW);
      digitalWriteFast(digitalPinToPinName(OUTPUT_TAIL_TINGGAL), LOW);
      perintah_pc = 'S';
    }
    // perintah lock atc
    else if (input.equalsIgnoreCase("L"))
    {
      digitalWriteFast(digitalPinToPinName(OUTPUT_LOCK_ATC), LOW);
      perintah_pc = 'L';
    }
    // perintah unlock atc
    else if (input.equalsIgnoreCase("U"))
    {
      digitalWriteFast(digitalPinToPinName(OUTPUT_LOCK_ATC), HIGH);
      perintah_pc = 'U';
    }
    // gerakkan atc
    else if (input.equalsIgnoreCase("A"))
    {
      digitalWriteFast(digitalPinToPinName(OUTPUT_CW_ATC), HIGH);
      perintah_pc = 'A';
    }
    // stop atc
    else if (input.equalsIgnoreCase("C"))
    {
      digitalWriteFast(digitalPinToPinName(OUTPUT_CW_ATC), LOW);
      perintah_pc = 'C';
    }
    else
      Serial.println("Perintah tidak dikenali!!");
  }

  // Periksa apakah ada data yang tersedia di Serial3
  while (Serial3.available() > 0)
  {
    // Baca data dari Serial
    String inputSerial3 = Serial3.readStringUntil('\n');
    inputSerial3.trim(); // Hilangkan spasi dan karakter newline
    buffer_inputSerial3 = inputSerial3;
  }

  verbose_output();
}