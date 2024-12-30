# IOT
// 定義引腳
const int ledPins[5] = {1, 2, 3, 4, 5};         // LED引腳（1~5）
const int segmentPins[7] = {6, 7, 8, 9, 10, 11, 12}; // 七段顯示器引腳（6~12）
const int buzzerPin = A0;                      // 無緣蜂鳴器（A0）
const int buttonPin = A1;                      // 按鈕引腳（A1）

bool gameStarted = false;                      // 遊戲開始標誌
int timerSeconds = 5;                          // 預設倒數時間
const int lightInterval = 200;                 // LED點亮間隔0.2秒
int currentLed = 0;                            // 當前亮起的LED索引
unsigned long lastLedTime = 0;                 // 上一次點亮LED的時間
unsigned long lastBuzzerTime = 0;              // 上一次蜂鳴器響的時間

void setup() {
  Serial.begin(115200);

  // 設定LED腳位
  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // 設定七段顯示器腳位
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }

  pinMode(buzzerPin, OUTPUT);                  // 設定蜂鳴器腳位
  pinMode(buttonPin, INPUT_PULLUP);            // 設定按鈕腳位

  // 播放開始音效
  playSound(buzzerPin, 1000, 300);
  delay(100);
  playSound(buzzerPin, 1500, 300);
}

void loop() {
  if (!gameStarted) {
    if (digitalRead(buttonPin) == LOW) {
      delay(50);  // 防止按鈕抖動
      if (digitalRead(buttonPin) == LOW) {
        gameStarted = true;
        Serial.println("遊戲開始！");
      }
    }
  } else {
    unsigned long currentTime = millis();

    // 倒數計時
    if (currentTime - lastBuzzerTime >= 1000) {
      lastBuzzerTime = currentTime;
      timerSeconds--;
      displayNumber(timerSeconds); // 更新七段顯示器
      playSound(buzzerPin, 2000, 200); // 蜂鳴器響一次
      Serial.print("倒數時間：");
      Serial.println(timerSeconds);
    }

    // 點亮LED
    if (currentTime - lastLedTime >= lightInterval) {
      lastLedTime = currentTime;

      // 熄滅上一個LED，點亮下一個
      for (int i = 0; i < 5; i++) {
        digitalWrite(ledPins[i], i == currentLed ? HIGH : LOW);
      }
      currentLed = (currentLed + 1) % 5; // 循環更新LED索引
    }

    // 如果倒數結束，遊戲停止
    if (timerSeconds <= 0) {
      Serial.println("遊戲結束！");
      gameStarted = false;

      // 熄滅所有LED
      for (int i = 0; i < 5; i++) {
        digitalWrite(ledPins[i], LOW);
      }

      // 播放結束音效
      playSound(buzzerPin, 1000, 500);
    }
  }
}

void playSound(int pin, int frequency, int duration) {
  tone(pin, frequency, duration);
  delay(duration + 50);
}

void displayNumber(int num) {
  // 七段顯示器數字定義
  const byte numbers[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
  };

  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], numbers[num % 10][i]);
  }
}
