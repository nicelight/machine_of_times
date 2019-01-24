
  /* any dirty tests

// приветственная мелодия
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(10, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(10);
  }

rainbowCycle(1);

for(int i = 2; i< 8; i++){
  for(int k = 0; k<255; k++){
    analogWrite(i,k);
    delay(3);
  }
  for(int k = 255; k>=0; k--){
    analogWrite(i,k);
    delay(3);
  }
}

rainbowCycle(1);
analogWrite(2, 50); 
analogWrite(3, 50); 
analogWrite(4, 50); 
analogWrite(5, 50); 
analogWrite(6, 50); 
analogWrite(7, 50); 
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue
  rainbow(20);
  theaterChaseRainbow(50);

rainbowCycle(20);
*/

