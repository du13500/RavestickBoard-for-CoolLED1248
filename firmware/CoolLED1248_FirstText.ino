// CoolLED1248: erster eigener Text vom ESP32-C3 (Arduino-ESP32 3.3.12)
// Grundlage: UpDryTwist/coolledx-driver und ohtaras/led
// Ziel: "FUCK YES" in sieben Farben auf einer 64x16-CoolLEDX anzeigen.
// ACHTUNG: Der bisher auf der Tafel gespeicherte Inhalt kann ersetzt werden.

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEClient.h>
#include <BLERemoteCharacteristic.h>
#include <cstring>

static constexpr int WIDTH = 64;
static constexpr int HEIGHT = 16;
static constexpr int PLANE_BYTES = WIDTH * HEIGHT / 8; // 128
static constexpr int RGB_BYTES = PLANE_BYTES * 3;     // 384
static constexpr int PAYLOAD_BYTES = 24 + 1 + 80 + 2 + RGB_BYTES; // 491
static constexpr int CHUNK_BYTES = 128;
static constexpr int NUM_CHUNKS = (PAYLOAD_BYTES + CHUNK_BYTES - 1) / CHUNK_BYTES;

static uint8_t rgb[RGB_BYTES];
static uint8_t payload[PAYLOAD_BYTES];
static volatile uint32_t notifications = 0;

// 7 LEDs-Farben: Bit 2=Rot, Bit 1=Gruen, Bit 0=Blau.
// Glyphen im 5x7-Format, jede Bildzeile wird doppelt so hoch gezeichnet.
static const uint8_t glyphs[8][7] = {
  {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000}, // F
  {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}, // U
  {0b01111, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b01111}, // C
  {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001}, // K
  {0,       0,       0,       0,       0,       0,       0      }, // Leerzeichen
  {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100}, // Y
  {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111}, // E
  {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110}  // S
};
static const uint8_t colors[8] = {4, 6, 5, 3, 0, 2, 1, 7};

void drawPixel(int x, int y, uint8_t color) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
  const int byteIndex = x * (HEIGHT / 8) + y / 8;
  const uint8_t bit = 0x80 >> (y % 8);
  if (color & 4) rgb[byteIndex] |= bit;
  if (color & 2) rgb[PLANE_BYTES + byteIndex] |= bit;
  if (color & 1) rgb[2 * PLANE_BYTES + byteIndex] |= bit;
}

void buildTextPayload() {
  memset(rgb, 0, sizeof(rgb));
  memset(payload, 0, sizeof(payload));

  // Acht 5-Pixel-Zeichen + sieben 1-Pixel-Abstaende = 47 Pixel.
  const int left = (WIDTH - 47) / 2;
  for (int c = 0; c < 8; c++) {
    for (int row = 0; row < 7; row++) {
      for (int col = 0; col < 5; col++) {
        if (glyphs[c][row] & (1 << (4 - col))) {
          drawPixel(left + 6 * c + col, 1 + 2 * row, colors[c]);
          drawPixel(left + 6 * c + col, 2 + 2 * row, colors[c]);
        }
      }
    }
  }

  // CoolLEDX-Textformat: 24 Nullbytes + Zeichenlaenge + 80 Metabytes
  // + 2-Byte-Pixeldatenlaenge + R-, G-, B-Pixelbits.
  payload[24] = 8; // "FUCK YES" hat acht Zeichen.
  for (int i = 0; i < 8; i++) payload[25 + i] = 0x30;
  payload[105] = (RGB_BYTES >> 8) & 0xFF;
  payload[106] = RGB_BYTES & 0xFF;
  memcpy(&payload[107], rgb, RGB_BYTES);
}

// Framing nach CoolLEDX: 01 [16-bit Laenge + Rohdaten, escaped] 03.
size_t makeFrame(const uint8_t *raw, size_t rawLen, uint8_t *framed) {
  size_t p = 0;
  framed[p++] = 0x01;
  auto addEscaped = [&](uint8_t b) {
    if (b >= 1 && b <= 3) {
      framed[p++] = 0x02;
      framed[p++] = b + 4;
    } else {
      framed[p++] = b;
    }
  };
  addEscaped((rawLen >> 8) & 0xFF);
  addEscaped(rawLen & 0xFF);
  for (size_t i = 0; i < rawLen; i++) addEscaped(raw[i]);
  framed[p++] = 0x03;
  return p;
}

// Ein CoolLEDX-Protokollchunk: Befehlsbyte 02 + Header + Daten + XOR.
size_t makeTextChunk(int index, uint8_t *raw) {
  const int offset = index * CHUNK_BYTES;
  const int rest = PAYLOAD_BYTES - offset;
  const int length = rest > CHUNK_BYTES ? CHUNK_BYTES : rest;
  size_t p = 0;
  raw[p++] = 0x02; // TEXT
  raw[p++] = 0x00;
  raw[p++] = (PAYLOAD_BYTES >> 8) & 0xFF;
  raw[p++] = PAYLOAD_BYTES & 0xFF;
  raw[p++] = (index >> 8) & 0xFF;
  raw[p++] = index & 0xFF;
  raw[p++] = length;
  memcpy(&raw[p], &payload[offset], length);
  p += length;
  uint8_t check = 0;
  for (size_t i = 1; i < p; i++) check ^= raw[i];
  raw[p++] = check;
  return p;
}

void onNotify(BLERemoteCharacteristic *, uint8_t *data, size_t len, bool) {
  Serial.print("Antwort: ");
  for (size_t i = 0; i < len; i++) {
    if (data[i] < 16) Serial.print('0');
    Serial.print(data[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  notifications++;
}

bool sendPacket(BLERemoteCharacteristic *chr, uint8_t *frame,
                size_t length, bool waitForAck) {
  const uint32_t before = notifications;
  if (!chr->writeValue(frame, length, false)) {
    Serial.println("BLE-Schreibfehler!");
    return false;
  }
  if (!waitForAck) {
    delay(100);
    return true;
  }
  const uint32_t start = millis();
  while (notifications == before && millis() - start < 2000) delay(10);
  if (notifications == before) {
    Serial.println("Keine Geraetebestaetigung innerhalb von 2 s.");
    return false;
  }
  delay(60);
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("CoolLED1248: ERSTER EIGENER TEXT!");
  Serial.println("Soll: FUCK YES (64 x 16 Pixel, bunt)");
  buildTextPayload();

  BLEDevice::init("");
  BLEDevice::setMTU(247);
  BLEClient *client = BLEDevice::createClient();
  BLEAddress address(String("ff:00:00:03:7e:c3"));
  Serial.println("Verbinde mit CoolLEDX...");
  if (!client->connect(address)) {
    Serial.println("Verbindung fehlgeschlagen.");
    return;
  }
  Serial.println("Verbunden!");
  delay(700); // MTU-Aushandlung abwarten
  const uint16_t mtu = client->getMTU();
  Serial.printf("Ausgehandelte BLE-MTU: %u\n", mtu);

  BLERemoteService *svc = client->getService(BLEUUID((uint16_t)0xFFF0));
  if (!svc) {
    Serial.println("Service FFF0 fehlt.");
    client->disconnect();
    return;
  }
  BLERemoteCharacteristic *chr = svc->getCharacteristic(BLEUUID((uint16_t)0xFFF1));
  if (!chr || !chr->canWriteNoResponse()) {
    Serial.println("FFF1 ist nicht schreibbar.");
    client->disconnect();
    return;
  }
  if (chr->canNotify()) chr->registerForNotify(onNotify);

  // Alle vier Textpakete im Voraus auf die vorhandene MTU pruefen.
  size_t maxFrame = 0;
  uint8_t raw[136];
  uint8_t framed[300];
  for (int i = 0; i < NUM_CHUNKS; i++) {
    size_t n = makeTextChunk(i, raw);
    size_t bytes = makeFrame(raw, n, framed);
    if (bytes > maxFrame) maxFrame = bytes;
  }
  Serial.printf("Textpakete: %d; groesstes Paket: %u Bytes\n",
                NUM_CHUNKS, (unsigned)maxFrame);
  if (mtu < maxFrame + 3) {
    Serial.println("MTU zu klein fuer 128-Byte-CoolLEDX-Pakete.");
    Serial.println("Kein Inhalt wurde ueberschrieben. Bitte Ausgabe senden.");
    client->disconnect();
    return;
  }

  // Statische Darstellung (nicht scrollend).
  uint8_t mode[] = {0x06, 0x01}; // MODE STATIC
  size_t modeLen = makeFrame(mode, sizeof(mode), framed);
  if (!sendPacket(chr, framed, modeLen, false)) {
    client->disconnect();
    return;
  }

  for (int i = 0; i < NUM_CHUNKS; i++) {
    size_t n = makeTextChunk(i, raw);
    size_t bytes = makeFrame(raw, n, framed);
    Serial.printf("Sende Textpaket %d/%d (%u Bytes)...\n",
                  i + 1, NUM_CHUNKS, (unsigned)bytes);
    if (!sendPacket(chr, framed, bytes, true)) {
      Serial.println("Uebertragung abgebrochen. Bitte Ausgabe senden.");
      client->disconnect();
      return;
    }
  }
  Serial.println("Alle Textpakete uebertragen!");
  delay(300);
  client->disconnect();
  Serial.println("FERTIG. Was zeigt die LED-Tafel?");
}

void loop() {}
