#include <Arduino.h> // Arduino 라이브러리 포함
#include <WiFi.h> // Wi-Fi 연결 라이브러리
#include <FirebaseESP32.h> // ESP32와 Firebase 연결 라이브러리
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <SimpleTimer.h> // 심플 타이머 라이브러리

// 공유기 아이디, 비밀번호 및 Firebase 정보 설정
//--------------------------------------
#define WIFI_SSID "WIFI_AP" // Wi-Fi SSID (이름)
#define WIFI_PASSWORD "WIFI_PASSWORD" // Wi-Fi 비밀번호

#define API_KEY "API_KEY" // Firebase API 키
#define DATABASE_URL "URL" // Firebase 데이터베이스 URL

#define USER_EMAIL "USER_EMAIL" // Firebase 사용자 이메일
#define USER_PASSWORD "USER_PASSWORD" // Firebase 사용자 비밀번호
//--------------------------------------

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;

void setup()
{
  //---------------------------------------------------------
  Serial.begin(115200); // 시리얼 통신 초기화
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Wi-Fi 연결 시작
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP 주소로 연결됨: ");
  Serial.println(WiFi.localIP()); // Wi-Fi로 연결된 IP 주소 출력
  Serial.println();
  Serial.printf("Firebase 클라이언트 v%s\n\n", FIREBASE_CLIENT_VERSION); // Firebase 클라이언트 버전 출력

  config.api_key = API_KEY; // Firebase 설정에 API 키 설정
  auth.user.email = USER_EMAIL; // Firebase 사용자 설정에 이메일 설정
  auth.user.password = USER_PASSWORD; // Firebase 사용자 설정에 비밀번호 설정
  config.database_url = DATABASE_URL; // Firebase 데이터베이스 설정에 URL 설정
  config.token_status_callback = tokenStatusCallback; // 토큰 상태 콜백 함수 설정
  Firebase.reconnectNetwork(true); // 네트워크 재연결 활성화
  fbdo.setBSSLBufferSize(4096, 1024); // 버퍼 크기 설정

  Firebase.begin(&config, &auth); // Firebase 초기화
  Firebase.setDoubleDigits(5); // 출력 숫자 자릿수 설정

  count++;
  //---------------------------------------------------------
}

void loop()
{
  //---------------------------------------------------------
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // Firebase에 불리언 값을 설정하고 가져오기
    Firebase.setBool(fbdo, F("/test/bool"), count % 2 == 0);
    Firebase.getBool(fbdo, FPSTR("/test/bool"));
    bool bVal;
    Firebase.getBool(fbdo, F("/test/bool"), &bVal);

    // Firebase에 정수 값을 설정하고 가져오기
    Firebase.setInt(fbdo, F("/test/int"), count);
    Firebase.getInt(fbdo, F("/test/int"));
    int iVal = 0;
    Firebase.getInt(fbdo, F("/test/int"), &iVal);

    // Firebase에 부동 소수점 값을 설정하고 가져오기
    Firebase.setFloat(fbdo, F("/test/float"), count + 10.2);
    Firebase.getFloat(fbdo, F("/test/float"));

    // Firebase에 더블 값을 설정하고 가져오기
    Firebase.setDouble(fbdo, F("/test/double"), count + 35.517549723765);
    Firebase.getDouble(fbdo, F("/test/double"));

    // Firebase에 문자열 값을 설정하고 가져오기
    Firebase.setString(fbdo, F("/test/string"), "Hello World!");
    Firebase.getString(fbdo, F("/test/string"));

    FirebaseJson json;

    if (count == 0)
    {
      // JSON 데이터를 Firebase에 설정
      json.set("value/round/" + String(count), F("cool!"));
      json.set(F("vaue/ts/.sv"), F("timestamp"));
      Firebase.set(fbdo, F("/test/json"), json);
    }
    else
    {
      // JSON 데이터를 Firebase에 업데이트
      json.add(String(count), "smart!");
      Firebase.updateNode(fbdo, F("/test/json/value/round"), json);
    }

    count++;
  }
  //---------------------------------------------------------
}
