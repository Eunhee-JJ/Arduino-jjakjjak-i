#include <PDM.h>
#include <Arduino_HTS221.h>

UART mySerial(digitalPinToPinName(4), digitalPinToPinName(3), NC, NC); // TX: 4, RX: 3

const int RED = D12;
const int GREEN = D11;
const int BLUE = D10;
int R_light = 0;
int G_light = 0;
int B_light = 0;
boolean LED_power = HIGH;
int light_mode = 1;
unsigned long clapTime_curr = 0;
unsigned long clapTime_prev = 0;
unsigned long hearingClap_prev=0;
unsigned long hearingClap_curr=0;
unsigned long toggleTime=0;
unsigned long changeTime=0;
unsigned long fadeTime_curr=0;
unsigned long prevTime_1=0;
unsigned long prevTime_2=0;
unsigned long currTime=0;
unsigned long fadeTime_prev=0;


int sum=0; // 소리 센서 버퍼값의 총합 저장
int soundAVG=0; // 소리 센서 평균값 저장

short sampleBuffer[256];

volatile int samplesRead; // 읽을 샘플 수

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  if (!HTS.begin()) { // 온습도 센서를 초기화합니다. 초기화중 문제가 발생하면 오류를 발생시킵니다.
    Serial.println("HTS2221센서 오류!");
    while (1);
  }
  //while(!Serial);

  PDM.onReceive(onPDMdata); // 소리 데이터 인식 함수

  if(!PDM.begin(1,16000)){
    Serial.println("Failed to start PDM!");
    while (1);
  }
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void loop() {
  /*온습도*/
  currTime=millis();
  if((currTime-prevTime_1)>=2345){
    prevTime_1=currTime;
    float Tem = HTS.readTemperature();
    float Hum = HTS.readHumidity();
    Serial.println(Tem,1);
    mySerial.print(Tem,1);
    mySerial.print("t");
    Serial.println(Hum,1);
    mySerial.print(Hum,1);
    mySerial.print("h"); 
    mySerial.print(" ");
    }

  /*박수인터페이스*/
  //if((currTime-prevTime_2)>=0){
    prevTime_2=currTime;

    sum=0; // 소리 센서 버퍼값의 총합 초기화
    soundAVG=0; // 소리 센서 평균값 초기화

    printSoundAVG();
    lightMode(light_mode);
   
    
    if(soundAVG >= 70 || soundAVG<=-70){    // 소리 센서의 절대값이 50 이상이면(박수 소리 크기)
      hearingClap_curr = 1;   // 박수를 쳤다고 인지하여 hearingClap_curr를 1로 선언
      Serial.println("clapped");
    } else {
      hearingClap_curr = 0; // 소리가 작으면 hearingClap_curr를 0으로 함  
    }
  
    clapTime_curr = millis(); // 아두이노가 켜지고 난 후부터의 시간을 저장
    clapInterface();
  
    hearingClap_prev = hearingClap_curr;  // 현재 박수소리 상태를 이전 박수소리 상태로 해줌
    samplesRead=0;
  //}
}

 void onPDMdata(){ //소리 아날로그 값 읽어오기
  // 사용가능한 바이트 수 질의
  int bytesAvailable = PDM.available();

  // sampleBuffer에 작성
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit. 2 bytes per sample
  samplesRead=bytesAvailable/2;
} 

void printSoundAVG(){
  if(samplesRead){ // 사운드 센서값을 저장한 버퍼의 평균값 추력
    for(int i=0; i<samplesRead; i++){
      sum+=sampleBuffer[i];
    }
    soundAVG=sum/samplesRead;
    Serial.println(soundAVG);
  }
}

void clapInterface(){
  if(hearingClap_curr == 1){
    if (
      (clapTime_curr > clapTime_prev + 300) && // 이전 박수 0.2초 후에 박수소리가 들린거면
      (hearingClap_prev == 0) &&  // 이전에 박수소리가 없었으면(큰소리가 계속 들리는경우 불이 켜지지 않음)
      (clapTime_curr < clapTime_prev + 800) && // 이전 박수 0.8초 이전에 박수소리가 들린거면
      (clapTime_curr > toggleTime + 1000) && // 전등 상태가 토글된지 1초 이후이면(박수를 세번쳤을때 반응이 없도록)
      (clapTime_curr > changeTime + 1000) // 전등 모드가 변경된지 1초 이후이면
    ) {
      // 위 조건을 모두 만족한 경우(박수를 2번 친 경우)
      Serial.println("toggled");
      LED_power = !LED_power; // 전등의 전원 상태를 토글, 꺼져있었으면 켜주고, 켜져있었으면 꺼주는 상태
      if(LED_power==LOW){
        light_mode = 0;
      }
      else{
        light_mode = 1;
      }

      toggleTime = clapTime_curr; // 현재의 시간을 changeTime으로 함
    }
    
    else if (// 박수를 1번만 친 경우, 무드등 모드 변경
        (clapTime_curr > clapTime_prev + 1000) &&
        (LED_power == HIGH) &&
        (hearingClap_prev == 0) &&
        (clapTime_curr > changeTime + 1000) &&
        (clapTime_curr > toggleTime + 1000)
        // 전등 모드가 달라진지 1초 이후이면
        ) {
          //unsigned long  currTime_2=millis();
          //if(currTime_2-clapTime_curr >= 1000){
            light_mode++;
            if(light_mode>=6) light_mode=1;
            changeTime = clapTime_curr ;
          //}
       }
    clapTime_prev = clapTime_curr; // 현재 박수시간을 이전 박수시간으로 해줌
    }
}

void lightMode(int mode){
  if(mode == 0){
    analogWrite(RED, 0);
    analogWrite(GREEN, 0);
    analogWrite(BLUE, 0);
  }
  if(mode == 1){ // 웜화이트
    //Serial.println("mode1");
    analogWrite(RED, 255);
    analogWrite(GREEN, 170);
    analogWrite(BLUE, 120);
  }
  else if(mode == 2){ // 쿨화이트
    //Serial.println("mode2");
    analogWrite(RED, 225);
    analogWrite(GREEN, 243);
    analogWrite(BLUE, 255);
  }
  else if(mode == 3){ // 그린화이트
    //Serial.println("mode3");
    analogWrite(RED, 150);
    analogWrite(GREEN, 255);
    analogWrite(BLUE, 200);
  }
  else if(mode == 4){ // RGB 숨쉬기모드
    hearingClap_curr = 0;
    int i=0;
    int j=255;
      while(i<256){ // fadeIn
      fadeTime_curr = millis();
      if(fadeTime_curr-fadeTime_prev>=5){
        //if(hearingClap_curr==1) return;
        i++;
        analogWrite(RED, i);
        analogWrite(BLUE, 0);
        analogWrite(GREEN, 0);
        //analogWrite(
        Serial.println("fadeIn");
        Serial.print("lightness: ");
        Serial.println(i);
        fadeTime_prev = fadeTime_curr;
        }
    
      }
    while(j>0){ //fadeOut
      fadeTime_curr = millis();
      if(fadeTime_curr-fadeTime_prev>=5){
        //if(hearingClap_curr==1) return;
        j--;
        analogWrite(RED, j);
        analogWrite(BLUE, 0);
        analogWrite(GREEN, 0);
        Serial.println("fadeOut");
        Serial.print("lightness: ");
        Serial.println(j);
        fadeTime_prev = fadeTime_curr;
        }
      }

    i=0;
    j=255;
    while(i<256){ // fadeIn
      fadeTime_curr = millis();
      if(fadeTime_curr-fadeTime_prev>=5){
        //if(hearingClap_curr==1) return;
        i++;
        analogWrite(RED, 0);
        analogWrite(BLUE, 0);
        analogWrite(GREEN, i);
        //analogWrite(
        Serial.println("fadeIn");
        Serial.print("lightness: ");
        Serial.println(i);
        fadeTime_prev = fadeTime_curr;
        }
    
      }
    
    while(j>0){ //fadeOut
      fadeTime_curr = millis();
      if(fadeTime_curr-fadeTime_prev>=5){
        //if(hearingClap_curr==1) return;
        j--;
        analogWrite(RED, 0);
        analogWrite(BLUE, 0);
        analogWrite(GREEN, j);
        Serial.println("fadeOut");
        Serial.print("lightness: ");
        Serial.println(j);
        fadeTime_prev = fadeTime_curr;
        }
      }

    i=0;
    j=255;
    while(i<256){ // fadeIn
      fadeTime_curr = millis();
      if(fadeTime_curr-fadeTime_prev>=5){
        //if(hearingClap_curr==1) return;
        i++;
        analogWrite(RED, 0);
        analogWrite(BLUE, i);
        analogWrite(GREEN, 0);
        //analogWrite(
        //Serial.println("fadeIn");
        //Serial.print("lightness: ");
        //Serial.println(i);
        fadeTime_prev = fadeTime_curr;
        }
    
      }
    while(j>0){ //fadeOut
      fadeTime_curr = millis();
      if(fadeTime_curr-fadeTime_prev>=5){
        //if(hearingClap_curr==1) return;
        j--;
        analogWrite(RED, 0);
        analogWrite(BLUE, j);
        analogWrite(GREEN, 0);
        //Serial.println("fadeOut");
        //Serial.print("lightness: ");
        //Serial.println(j);
        fadeTime_prev = fadeTime_curr;
        }
      }
    light_mode++;

  }
  else if(mode == 5){ // 분위기 인식
    //Serial.println("mode5");
   for(int i=0; i<samplesRead; i++){
    if(sampleBuffer[i] > 35 || sampleBuffer[i] <-35){
      Serial.print("b: ");
      Serial.println(sampleBuffer[i]);
      if(sampleBuffer[i] > 80 || sampleBuffer[i] <- 80){
        light_mode=1;
        break;
      }
      fadeInOut();
    }
    Serial.println("read finish");
   }
  }
}

void fadeInOut(){
  int i=0;
  int j=255;
  while(i<256){
    fadeTime_curr = millis();
    if(fadeTime_curr-fadeTime_prev>=1){
      if(hearingClap_curr==1) break;
      i++;
      analogWrite(RED, i);
      analogWrite(BLUE, i);
      analogWrite(GREEN, i);
      //Serial.println("fadeIn");
      //Serial.print("lightness: ");
      //Serial.println(i);
      fadeTime_prev = fadeTime_curr;
    }
  }
  while(j>0){
    fadeTime_curr = millis();
    if(fadeTime_curr-fadeTime_prev>=1){
      if(hearingClap_curr==1) break;
      j--;
      analogWrite(RED, j);
      analogWrite(BLUE, j);
      analogWrite(GREEN, j);
      //Serial.println("fadeOut");
      //Serial.print("lightness: ");
      //Serial.println(j);
      fadeTime_prev = fadeTime_curr;
    }
  }
  
}

// 박수 인식 출처: https://drive.google.com/file/d/1ZExkw5YPfzP2sXjj-WBM0SDV9FNffPOd/view
