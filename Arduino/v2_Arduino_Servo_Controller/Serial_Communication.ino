/*  Serial Decode
 *   
 *   Read Format
 *   
 *   20-20-400-200
 *   
 *   Write
 *   
 */

String inString = "";    // string to hold input char
String inString_buffer = "";    // string to hold input Serial msg
String inString_display_buffer = "";    // string to hold Display info


  // ============ CONFIG HERE ================
#define Input_size 3  //define number of inputs value(s)
  // ============================================
  
int current_index = 0;//index of current decoding number
long input_value[Input_size];  //inputs value(s) buffer
boolean update_flag = false; //Flag for end of success input string follow /n



void serial_decode()   // Read serial input:
{
  byte Read_Flag = 0;
  while (Serial.available() > 0)
  {
    int inChar = Serial.read();
    char_decode(inChar) ;
  }
 

}




void char_decode(int inChar)
{
  // Read_Flag=0;
  inString_buffer += (char)inChar;
  if (isDigit(inChar))       //if get a number
  {
    // convert the incoming byte to a char
    // and add it to the string:
    inString += (char)inChar;
    //Serial.println(inString);
  }
  else if (inChar == '-')       // if you get a comma, convert to a number,
  {
    if (current_index < Input_size) // within size
    {
      input_value[current_index] = inString.toInt(); //
    //  pdInput_value[current_index] = inString.toInt(); //
      current_index++;  //increase index
    }



    
    //            else// giveup the value to avoide over access array
    //            {
    //              Serial.println("overflow");
    //            }
    inString = "";   // clear the string buffer for new input:
  }
  else if (inChar == '\n' || inChar == 'N')       //if you get a newline, convert to a number and print all value
  {
    if (current_index < Input_size) // within size
    {
      input_value[current_index] = inString.toInt(); //final value
      val = input_value[0];
            //pdInput_value[current_index] = inString.toInt(); //
    }
    //            else// giveup the value to avoide over access array
    //            {
    //              Serial.println("overflow(end)");
    //            }


    inString = "";   // clear the string buffer for new input:
    current_index = 0;// reset the index
    //Set update_Flag
    update_flag = true;
    //print all value
    for (int index = 0; index < Input_size; index++)
    {
      Serial.print(index);
      Serial.print(":\t");
      Serial.print(input_value[index]);
      Serial.print("\t");
    }
    Serial.print("\n ");
  }
  else if (inChar == 'D')       // if dummy
  {
   //DO STH HERE
    inString = "";   // clear the string buffer for new input:
    Serial.println("Reached Here");
  }

}


void check_update()   //Check update flag and write value when string finish
{
  if ( update_flag == true) //update value
  {
    update_flag = false; //clear flag
    inString_display_buffer = inString_buffer;    // string to hold input Serial msg
    inString_buffer = "";   // clear the string buffer for new input:
  }
}

