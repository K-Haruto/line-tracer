double Setpoint = 0, Input, Input_old = 0, Input_diff = 0, Output_P, Output_I, Output_D; //基準値, 入力値, 出力値(P,D)の設定
int mode = 0; //モード番号
double kp[4] = {0.09, 0.09, 0.09, 0.09}, kd[4] = {1.2, 1.5, 0.5, 0.9}, ki[4] = {0, 0, 0, 0}; //ゲイン調節
int power[4] = {100, 100, 50, 50}; //モーターパワー基準
int n[4] = {1.4, 1.4, 1.4, 1.4}; //重み付値

int led[4] = {8, 9, 10, 11}; //LEDのピン番号格納
int line_sensor[4] = {A0, A1, A2, A3}; //ラインセンサーのピン番号格納
int bat_volt = A4; //電源電圧
int m_rg[2][3] = {
  {4, 7, 6}, //モーター左{IN1,IN2,PWM}
  {3, 2, 5}, //モーター右
};
boolean motor_sw = false; //モータースイッチ, デフォルトoff

double val_sensor[4] = {0, 0, 0, 0}; //ラインセンサーの値格納用
double sensor_l, sensor_r; //左右センサー値
double val_volt; //電源電圧値

#define sw1 12 //スイッチHigh時オン
#define sw2 13
#define m_r 0 //モーター番号定義
#define m_l 1

void motor(char num, double speed);
void motor_stop(char num);

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
  digitalWrite(led[mode], HIGH);
}

void loop() {
  //P制御

  //入力値格納
  for (int i = 0; i < 4; i++) {
    val_sensor[i] = 1023 - analogRead(line_sensor[i]);
  }
  val_volt = analogRead(bat_volt);
  val_volt = val_volt / 1023 * 10;

  //モード設定
  if (digitalRead(sw2) == HIGH) {
    mode++;
    if (mode > 3) {
      mode = mode - 4;
      for (int i = 0; i < 4; i++) { //LED消灯
        digitalWrite(led[i], LOW);
      }
    }
    digitalWrite(led[mode], HIGH);
    delay(200);
  }

  //出力計算
  sensor_l = n[mode] * val_sensor[0] + val_sensor[1];
  sensor_r = n[mode] * val_sensor[3] + val_sensor[2];
  Input = Setpoint - (sensor_r - sensor_l);
  Input_diff += Input;
  Output_P = Input * kp[mode];
  Output_I = Input_diff * ki[mode];
  Output_D = (Input - Input_old) * kd[mode];
  double output_r = power[mode] + Output_P + Output_I + Output_D;
  double output_l = power[mode] - Output_P - Output_I - Output_D;
  Input_old = Input;

  //モーター出力
  if (digitalRead(sw1) == HIGH) {
    motor_sw = !motor_sw;
    for (int i = 0; i < 4; i++) { //LED消灯
      digitalWrite(led[i], LOW);
    }
    delay(1000);
  }
  if (motor_sw == true) {
    //モーター出力
    motor(m_r, output_r);
    motor(m_l, output_l);
    //LED出力
    if (Input <= 0) { //左にずれる
      digitalWrite(led[0], HIGH);
      digitalWrite(led[3], LOW);
    } else {
      digitalWrite(led[0], LOW);
      digitalWrite(led[3], HIGH);
    }
  }

  //シリアルモニタ表示
  //  Serial.print("val_volt=");
  Serial.println(val_volt);
  //  Serial.print("\t");
  //  Serial.print("output_r=");
  //  Serial.println(output_r);
  //  Serial.print("\t");
  //  Serial.print("output_l=");
  //  Serial.print(output_l);
  //  Serial.print("\t");
  //  Serial.print("Output_P=");
  //  Serial.print(Output_P);
  //  Serial.print("\t");
  //  Serial.print("Output_I=");
  //  Serial.print(Output_I);
  //  Serial.println("\t");
  //  Serial.print("Output_D=");
  //  Serial.print(Output_D);
  //  Serial.println("\t");
}

/*
   motor関数の説明
   引数
   モーター番号, 速度
*/
void motor(char num, double speed) {
  if (speed >= 0) { //正転
    digitalWrite(m_rg[num][0], HIGH);
    digitalWrite(m_rg[num][1], LOW);
    analogWrite(m_rg[num][2], speed);
  } else {
    speed = -speed;
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
