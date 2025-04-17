int led[4] = {8, 9, 10, 11}; //LEDのピン番号格納
int line_sensor[4] = {A0, A1, A2, A3}; //ラインセンサーのピン番号格納
int bat_volt = A4; //電源電圧
int m_rg[2][3] = {
  {7, 4, 6}, //モーター左{IN1,IN2,PWM}
  {2, 3, 5}, //モーター右
};

int val_sensor[4] = {0, 0, 0, 0}; //ラインセンサーの値格納用

#define sw1 12 //スイッチHigh時オン
#define sw2 13
#define m_r 0 //モーター番号定義
#define m_l 1

void setup() {
  Serial.begin(9600);
  pinMode(led[0], OUTPUT);
  pinMode(led[1], OUTPUT);
  pinMode(led[2], OUTPUT);
  pinMode(led[3], OUTPUT);
  pinMode(m_rg[0][0], OUTPUT); //例えばm_rg[1][2]は7を表す
  pinMode(m_rg[0][1], OUTPUT);
  pinMode(m_rg[0][2], OUTPUT);
  pinMode(m_rg[1][0], OUTPUT);
  pinMode(m_rg[1][1], OUTPUT);
  pinMode(m_rg[1][2], OUTPUT);
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
}

void loop() {
  //動作チェックプログラム
  
  for(int i = 0; i < 4; i++){ //ラインセンサーのチェック
    val_sensor[i] = 1023 - analogRead(line_sensor[i]);
  }
  Serial.print(val_sensor[0]);
  Serial.print("\t");
  Serial.print(val_sensor[1]);
  Serial.print("\t");
  Serial.print(val_sensor[2]);
  Serial.print("\t");
  Serial.println(val_sensor[3]);

  if(digitalRead(sw1) == HIGH){ //スイッチ1とモーター正転のチェック
    motor(m_r,100,true);
    digitalWrite(led[0],HIGH);
    delay(1000);
  }
  if(digitalRead(sw2) == HIGH){ //スイッチ2とモーター後転のチェック
    motor(m_l,100,false);
    digitalWrite(led[2],HIGH);
    digitalWrite(led[3],HIGH);
    delay(1000);
  }

  //動作終了処理
  for(int i = 0;i < 4;i++){
    digitalWrite(led[i],LOW);
  }
  for(int i = 0;i < 2;i++){
    for(int j = 0;j < 3;j++){
      digitalWrite(m_rg[i][j],LOW);
    }
  }
}

/*
   motor関数の説明
   引数
   モーター番号, 速度, 回す方向
*/
void motor(char num, char speed, boolean direction) {
  if (direction == true) { //正転
    digitalWrite(m_rg[num][0], HIGH);
    digitalWrite(m_rg[num][1], LOW);
    analogWrite(m_rg[num][2], speed);
  } else {
    digitalWrite(m_rg[num][0], LOW);
    digitalWrite(m_rg[num][1], HIGH);
    analogWrite(m_rg[num][2], speed);
  }
}

/*
   motor_stop関数の説明
   引数
   モーター番号
*/
void motor_stop(char num) {
  digitalWrite(m_rg[num][0], HIGH);
  digitalWrite(m_rg[num][1], HIGH);
}

