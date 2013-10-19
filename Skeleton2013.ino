/*
Halloween Skeleton Sketch for 2013

The Skeleton has:
  - LEDs for eyes
  - A Motor to make it shake like crazy
  - An ultrasonic sensor to detect when someone (or something) is nearby
  - An MP3 Player Shield to make sounds, and

Earlier versions of this sketch were written for a self-contained
unit (motion sensor, motor/LED control, and MP3 player). This version
just has the motion sensor/LED control and motor control. It's
intended to be used with a device running Linux (e.g. a Raspberry Pi)
that interacts with this sketch over a serial connection with a few
well-defined commands to read sensor input and control eye and motor
state.
  
*/

#define EYES_PIN   6
#define MOTOR_PIN  3
#define PING_PIN   5

#define EYE_INCREMENT 10  // PWM step for eye pulsing
#define EYE_INTERVAL 27  // ms to delay between eye value updates

#define CMD_SHAKE 's'
#define CMD_THROB 't'
#define CMD_DISTANCE 'd'


unsigned int eye_value;  // current PWM value for pulsing eyes
unsigned int eye_increment = EYE_INCREMENT;
boolean eyes_pulse;  // if true, pulse eye LEDs
unsigned long next_eye_update = 0L;


unsigned int get_distance() {
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(PING_PIN, OUTPUT);
  digitalWrite(PING_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(PING_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(PING_PIN, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(PING_PIN, INPUT);
  long duration = pulseIn(PING_PIN, HIGH);

  // convert the time into a distance
  return duration / 74 / 2;
}

void motor_on() {
  Serial.println("MOTOR ON");
  digitalWrite(MOTOR_PIN, HIGH);
  digitalWrite(EYES_PIN, HIGH);
}

void motor_off() {
  Serial.println("MOTOR OFF");
  digitalWrite(MOTOR_PIN, LOW);
  digitalWrite(EYES_PIN, LOW);
}


void set_shake_on() {
  digitalWrite(MOTOR_PIN, HIGH);
}


void set_shake_off() {
  digitalWrite(MOTOR_PIN, LOW);
}


void set_eyes_on() {
  digitalWrite(EYES_PIN, HIGH);
  eyes_pulse = false;
}


void set_eyes_pulse() {
  eyes_pulse = true;
}


char *dispatch(int cmd) {
  //Serial.print("I received: ");
  //Serial.println(cmd, DEC);
  if (cmd == CMD_SHAKE) {
    set_shake_on();
    set_eyes_on();
  } else if (cmd == CMD_THROB) {
    set_shake_off();
    set_eyes_pulse();
  } else if (cmd == CMD_DISTANCE) {
    Serial.print(get_distance());
  }
}


void update_eyes() {
  if (!eyes_pulse) {
    return;
  }
  if (millis() > next_eye_update) {
    int next_value = eye_value + eye_increment;
    if (next_value < 0 || next_value > 255) {
      eye_increment = -eye_increment;
      next_value = eye_value + eye_increment;
    }
    eye_value = next_value;
    analogWrite(EYES_PIN, eye_value);
    next_eye_update = millis() + EYE_INTERVAL;
  }
}


void setup() {                
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(EYES_PIN, OUTPUT);

  Serial.begin(9600);
  eyes_pulse = true;
}
  

void loop() {
  // Poll serial port for input. We use single-byte commands from the
  // computer to the Arduino so we don't need to do any command
  // buffering.
  if (Serial.available() > 0) {
    dispatch(Serial.read());
  }
  update_eyes();
}
