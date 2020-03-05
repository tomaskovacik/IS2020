#include "IS2020.h"
#include <Arduino.h>
#include "IS2020.h"

uint8_t IS2020::stopFfwRwd(uint8_t deviceId){ // Stop fast forward or rewind
  return IS2020::musicControl(deviceId,Music_Control_STOP_FFW_RWD);
}

uint8_t IS2020::stopFfwRwd(){ // Stop fast forward or rewind
  return IS2020::stopFfwRwd(0);
}

uint8_t IS2020::ffw(uint8_t deviceId){ //fast forward
  return IS2020::musicControl(deviceId,Music_Control_FFW);
}

uint8_t IS2020::ffw(){ //fast forward
  return IS2020::ffw(0);
}

uint8_t IS2020::repFfw(uint8_t deviceId){ //fast forward with repeat send fast forward command every 800ms
  return IS2020::musicControl(deviceId,Music_Control_REPFFW);
}

uint8_t IS2020::repFfw(){ //fast forward with repeat send fast forward command every 800ms
  return IS2020::repFfw(0);
}

uint8_t IS2020::rwd(uint8_t deviceId){ //rewind
  return IS2020::musicControl(deviceId,Music_Control_RWD);
}

uint8_t IS2020::rwd(){ //rewind
  return IS2020::rwd(0);
}

uint8_t IS2020::repRwd(uint8_t deviceId){ //rewind with repeat send rewind command every 800ms
  return IS2020::musicControl(deviceId,Music_Control_REPRWD);
}

uint8_t IS2020::repRwd(){ //rewind with repeat send rewind command every 800ms
  return IS2020::repRwd(0);
}

uint8_t IS2020::play(uint8_t deviceId){ //PLAY command
  return IS2020::musicControl(deviceId,Music_Control_PLAY);
}

uint8_t IS2020::play(){ //PLAY command
  return IS2020::play(0);
}

uint8_t IS2020::pause(uint8_t deviceId){  //PAUSE command
  return IS2020::musicControl(deviceId,Music_Control_PAUSE);
}

uint8_t IS2020::pause(){  //PAUSE command
  return IS2020::pause(0);
}

uint8_t IS2020::togglePlayPause(uint8_t deviceId){ //PLAY PAUSE toggle
  return IS2020::musicControl(deviceId,Music_Control_TOGLE_PLAY_PAUSE);
}

uint8_t IS2020::togglePlayPause(){ //PLAY PAUSE toggle
  return IS2020::togglePlayPause(0);
}

uint8_t IS2020::stop(uint8_t deviceId){ //STOP command
  return IS2020::musicControl(deviceId,Music_Control_STOP);
}

uint8_t IS2020::stop(){ //STOP command
  return IS2020::stop(0);
}
