//Dart Gun Controller - Arduino Program that takes one of four
//  serial commands (arm/shot/salvo/stop) and sets Digital Pins
//  in order to execute the desired action.
//
//  Desired actions are:
//      arm   - Start the rotating wheels that are used to 
//              accelerate the darts out of the gun.
//      shot  - Turn on the drum for 324ms (calibrated to my
//              setup; YMMV) which should result in one round
//              being pushed fired.  If weapon was not armed
//              when this command was issued, the wheels will
//              be rotated for 2 seconds prior to turning on 
//              the drum and stopped immediately afterwards.
//      salvo - Turn on the drum until the stop command is
//              issued.  If the weapon was not armed when this
//              command was issued, the wheels will be rotated 
//              for 2 seconds prior to turning on the drum.
//      stop -  Stop both circuit immediately, stopping the 
//              weapon from firing.

//pin definitions
#define WheelPin 10
#define DrumPin 11

//Command definitions
#define NO_CMD 0
#define ARM 1
#define SHOT 2
#define SALVO 3
#define STOP 4

//Armed state definitions
#define ARMED 1
#define NOT_ARMED 0

//Variable definitions - variables needed between loop and
//    serialEvent interrupt routine are set to volatile 
volatile int Armed=NOT_ARMED; // Tracks whether weapon is armed
char inByte; //Single byte for reading serial buffer
String command=""; //string to store serial command
volatile int CommandInt=NO_CMD; //Stores encoded command

void setup (){
    Serial.begin(9600);//Start serial
    //Set Pin mode and initial state (HIGH=off, LOW=on)
    pinMode(WheelPin,OUTPUT);
    pinMode(DrumPin,OUTPUT);
    digitalWrite(WheelPin,HIGH);
    digitalWrite(DrumPin,HIGH);     
    Serial.println("Initialized");//Print initialization mode
}

void loop(){

  //CommandInt is populated by serialEvent interrupt routine
  //(below)
  if (CommandInt > 0){

      if (CommandInt==SALVO){
          if (Armed==NOT_ARMED) {
              digitalWrite(WheelPin,LOW);
              delay(2000);
              Armed=ARMED;
          }
          digitalWrite(DrumPin,LOW);
          Serial.println("Weapon firing");
          CommandInt=NO_CMD;
      }

      else if (CommandInt==STOP){
          digitalWrite(WheelPin,HIGH);
          digitalWrite(DrumPin,HIGH);
          Serial.println("Firing stopped");
          Armed=NOT_ARMED;
          CommandInt=NO_CMD;
      }

      else if (CommandInt==ARM){
          digitalWrite(WheelPin,LOW);
          Serial.println("Armed");
          Armed=ARMED;
          CommandInt=NO_CMD;
      }

      else if (CommandInt==SHOT){
          if (Armed==NOT_ARMED){
              digitalWrite(WheelPin,LOW);
              delay(2000);
          }
          digitalWrite(DrumPin,LOW);
          Serial.println("Firing one shot");
          delay (324);
          if (Armed==NOT_ARMED){
              digitalWrite(WheelPin,HIGH);
          }
          digitalWrite(DrumPin,HIGH);
          Serial.println("Firing stopped");
          CommandInt=NO_CMD;   
      }
  }
}

void serialEvent(){
    //This subroutine fires when the serial buffer receives
    //characters
    
    while (Serial.available() > 0){
        //read in first word (the command)
        inByte = Serial.read();
        //if Byte is alpha-numeric, add it to command
        if ((inByte >= 65 && inByte <= 90) || (inByte >=97 && inByte <=122) || (inByte >= 48 &&     inByte <=57) || inByte == 43 || inByte == 61 || inByte == 63) {
            command.concat(inByte);
        }
        if (inByte == 32 || inByte == 10){
            command.toLowerCase();

            //turn string command into command number
            if (command=="arm"){
                CommandInt=ARM;
            }
            else if (command=="shot"){
                CommandInt=SHOT;
            }
            else if (command=="salvo"){
                CommandInt=SALVO;
            }
            else if (command=="stop"){
                CommandInt=STOP;
            }
            else {
                CommandInt=NO_CMD;
            }
            command="";
        }    
    }
}


void clearSerial(){
  while (Serial.available() > 0){
      inByte = Serial.read();
      //Serial.println(inByte); 
    }
}
