// 引腳定義
const int ledPins[5] = {1, 2, 3, 4, 5}; // LED 引腳
const int segmentPins[7] = {6, 7, 8, 9, 10, 11, 12}; // 七段顯示器引腳
const int buzzerPin = A0; // 蜂鳴器引腳
const int buttonPin = A1; // 按鈕引腳


const byte segmentLetters[6][7] = {
    {1, 0, 0, 1, 1, 1, 1}, // E
    {0, 1, 1, 0, 0, 0, 0}, // I
    {0, 1, 1, 0, 1, 1, 1}, // H
    {1, 0, 1, 1, 0, 1, 1}, // S
    {0, 0, 0, 1, 1, 1, 0}, // L
    {0, 0, 1, 1, 1, 1, 1}  // U
};


const byte segmentNumbers[10][7] = {
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


int currentMode = 0; // 當前模式：0: 簡單, 1: 中等, 2: 困難


void setup() {
    // 初始化引腳
    for (int i = 0; i < 5; i++) pinMode(ledPins[i], OUTPUT);
    for (int i = 0; i < 7; i++) pinMode(segmentPins[i], OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(buttonPin, OUTPUT);


    // 播放開啟音效
    playTone(1000, 200);
    delay(100);
    playTone(1500, 200);


    // 初始化顯示難度
    displayMode(currentMode); // 預設顯示難度 E（簡單）
}


void loop() {
    static bool gameStarted = false;


    if (!gameStarted) {
        chooseDifficulty(); // 選擇難度
    }


    if (isButtonHeld(3000)) { // 長按 3 秒開始遊戲
        gameStarted = true;
        startGame(); // 執行遊戲流程


        // 遊戲結束後保持結果顯示，並停止程式
        while (true); // 停止程式執行
    }
}


// 其他函數（與先前程式一致）




// 選擇難度
void chooseDifficulty() {
    if (isButtonPressed()) {
        currentMode = (currentMode + 1) % 3; // 循環切換模式
        displayMode(currentMode);           // 顯示當前難度
        delay(200);                         // 消抖
    }
}


// 開始遊戲
void startGame() {
    // 顯示選定難度
    displayMode(currentMode);
    delay(1000); // 顯示 1 秒


    // 倒數計時
    countdown(3);
   
    int randomDelay = random(2000, 5000);
    delay(randomDelay);


    // 遊戲計時
    bool success = runTimer(currentMode);


    // 顯示結果
    displayResult(success);
}


// 倒數計時
void countdown(int seconds) {
    for (int i = seconds; i > 0; i--) {
        displayNumber(i);  // 顯示當前數字
        playTone(500, 100); // 提示音效
        delay(1000);        // 延遲 1 秒
    }
}


// 遊戲計時
bool runTimer(int mode) {
    int duration = (mode == 0) ? 7 : (mode == 1) ? 5 : 3; // 根據難度設置計時秒數
    unsigned long startMillis = millis();  // 記錄遊戲開始時間

    for (int i = duration; i > 0; i--) {
        unsigned long intervalStart = millis(); // 記錄每個倒數單位的起始時間
        displayNumber(i);    // 顯示剩餘秒數

        // 在 1 秒內進行燈泡動畫和提示音
        while (millis() - intervalStart < 1000) {
            animateLights();  // 播放燈泡動畫
            playTone(500, 100); // 提示音效
        }

        // 檢查是否按下按鈕
        if (digitalRead(buttonPin) == HIGH) {
            return true;      // 按下按鈕，返回成功
        }

        // 超過時間後顯示 L 並返回失敗
        if (millis() - startMillis >= duration * 1000) {
            displayResult(false); // 顯示失敗
            return false;
        }
    }

    return false; // 倒數結束且未按下按鈕，返回失敗
}


// 顯示模式（E, I, H）
void displayMode(int mode) {
    clearSegment();
    for (int i = 0; i < 7; i++) {
        digitalWrite(segmentPins[i], segmentLetters[mode][i]);
    }
}


// 顯示數字
void displayNumber(int number) {
    clearSegment();
    for (int i = 0; i < 7; i++) {
        digitalWrite(segmentPins[i], segmentNumbers[number][i]);
    }
}


// 顯示結果
void displayResult(bool success) {
    clearSegment();
    const byte* result = success ? segmentLetters[3] : segmentLetters[4]; // S 或 L
    for (int i = 0; i < 7; i++) {
        digitalWrite(segmentPins[i], result[i]);
    }
    playTone(success ? 1000 : 300, 500); // 成功音效或失敗音效
}


// 燈泡動畫
void animateLights() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(ledPins[i], HIGH);
        delay(200);
        digitalWrite(ledPins[i], LOW);
    }
}


// 清空七段顯示器
void clearSegment() {
    for (int i = 0; i < 7; i++) {
        digitalWrite(segmentPins[i], LOW);
    }
}


// 播放音效
void playTone(int frequency, int duration) {
    tone(buzzerPin, frequency, duration);
}


// 按鈕檢測
bool isButtonPressed() {
    static bool lastState = HIGH;
    bool currentState = digitalRead(buttonPin);
    if (lastState == HIGH && currentState == LOW) {
        lastState = currentState;
        return true;
    }
    lastState = currentState;
    return false;
}


// 長按檢測
bool isButtonHeld(int duration) {
    static unsigned long pressStart = 0;
    if (digitalRead(buttonPin) == LOW) {
        if (pressStart == 0) pressStart = millis();
        if (millis() - pressStart >= duration) return true;
    } else {
        pressStart = 0;
    }
    return false;
}
