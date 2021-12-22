#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utilities.h"  // DO NOT REMOVE this line
#include "implementation_reference.h"   // DO NOT REMOVE this line

//DEFINE hash values for Key Constants for switch case
#define W 177660
#define A 177638
#define S 177656
#define D 177641
#define CW 5862207 
#define CCW 193452258
#define MX 5862538
#define MY 5862539

//Declare Functions
//Hash
const unsigned long hash(const char *str);

//struct containing a color and location of a pixel
typedef struct Pixel {
    int red;
    int green;
    int blue;
    int row;
    int col;
} pixel;

/**
 * Takes the movement command and hashes it for easier searching through a switch case
 **/
const unsigned long hash(const char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}

/***********************************************************************************************************************
 * WARNING: Do not modify the implementation_driver and team info prototype (name, parameter, return value) !!!
 *          Do not forget to modify the team_name and team member information !!!
 **********************************************************************************************************************/
void print_team_info(){
    // Please modify this field with something interesting
    char team_name[] = "something interesting";

    // Please fill in your information
    char student_first_name[] = "kyu bum";
    char student_last_name[] = "kim";
    char student_student_number[] = "1003969100";

    // Printing out team information
    printf("*******************************************************************************************************\n");
    printf("Team Information:\n");
    printf("\tteam_name: %s\n", team_name);
    printf("\tstudent_first_name: %s\n", student_first_name);
    printf("\tstudent_last_name: %s\n", student_last_name);
    printf("\tstudent_student_number: %s\n", student_student_number);
}

/***********************************************************************************************************************
 * WARNING: Do not modify the implementation_driver and team info prototype (name, parameter, return value) !!!
 *          You can modify anything else in this file
 ***********************************************************************************************************************
 * @param sensor_values - structure stores parsed key value pairs of program instructions
 * @param sensor_values_count - number of valid sensor values parsed from sensor log file or commandline console
 * @param frame_buffer - pointer pointing to a buffer storing the imported  24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param grading_mode - turns off verification and turn on instrumentation
 ***********************************************************************************************************************
 *
 **********************************************************************************************************************/
void implementation_driver(struct kv *sensor_values, int sensor_values_count, unsigned char *frame_buffer,
                           unsigned int width, unsigned int height, bool grading_mode) {
    
    //colorCounter is both the index of the array and number of colored pixels
    int colorCounter = 0;

    //count the number of colored pixels to initialize size of array
    for (int row = 0; row < height; row++){
        int rowIndex = row * width * 3;
        for (int col = 0; col < width; col++){
            int index = rowIndex + col * 3;
            int red = frame_buffer[index];
            int green = frame_buffer[index + 1];
            int blue = frame_buffer[index + 2]; 
            
            int color = (blue << 16) | (green << 8) | red;

            if (color != 0xffffff){
                colorCounter += 1;
            }
        }
    }

    //initialize array of struct pixel
    pixel * coloredPixels = malloc(colorCounter * sizeof (pixel));
    
    //reset colorCounter for indexing
    colorCounter = 0;

    //input in rgb values plus current index of pixel
    for (int row = 0; row < height; row++){
        int rowIndex = row * width * 3;
        for (int col = 0; col < width; col++){
            int index = rowIndex + col * 3;
            int red = frame_buffer[index];
            int green = frame_buffer[index + 1];
            int blue = frame_buffer[index + 2]; 
            
            int color = (blue << 16) | (green << 8) | red;

            if (color != 0xffffff){
                coloredPixels[colorCounter].red = red;
                coloredPixels[colorCounter].green = green;
                coloredPixels[colorCounter].blue = blue;
                coloredPixels[colorCounter].row = row;
                coloredPixels[colorCounter].col = col;

                frame_buffer[index] = 255;
                frame_buffer[index + 1] = 255;
                frame_buffer[index + 2] = 255;
                colorCounter += 1;
            }
        }
    }

    //number of proccessed frames
    int processed_frames = 0;
    
    //value of key value pair
    int value;

    //number of times to move horizontally. positive to the right, negative to the left
    int horizontalCounter = 0;

    //number of times to move vertically. negative upwards, positive downwards
    int verticalCounter = 0;

    //number of times to rotate. positive is CW, negative is CCW
    int rotationCounter = 0;

    //number of times to flip horizontally
    bool flipXCounter = false;

    //number of times to flip vertically
    bool flipYCounter = false;  

    //temporary values for swapping and storing
    int temp;

    for (int sensorValueIdx = 0; sensorValueIdx < sensor_values_count; sensorValueIdx++) {
        // printf("Processing sensor value #%d: %s, %d\n", sensorValueIdx, sensor_values[sensorValueIdx].key,
        //        sensor_values[sensorValueIdx].value);

        value = sensor_values[sensorValueIdx].value;

        // printf("horizontal; %d, verical: %d, flipx: %d, flipy: %d\n", horizontalCounter, verticalCounter, flipXCounter, flipYCounter);
        switch(hash(sensor_values[sensorValueIdx].key)){
            // sum value to corresponding move key
            case W:
                verticalCounter -= value;
                break;
            case A: 
                horizontalCounter -= value;
                break;
            case S:
                verticalCounter += value;
                break;
            case D:
                horizontalCounter += value;
                break;
            
            case MX:
                //inverse flipX as flipping twice is the same as not flipping
                flipXCounter = !flipXCounter;
             
                //move up, flipx = flipx, move down and viceversa. thus inverse vertical counter
                verticalCounter *= -1;

                break;
            case MY:
                //inverse flipY as flipping twice is the same as not flipping
                flipYCounter = !flipYCounter;
                
                //move left, flipy = flipy, move right and viceversa. thus inverse horizontal counter
                horizontalCounter *= -1;

                break;
            case CW:
                value %= 4;
                switch(value){
                    
                    //if 2 or negative 2, flip x and flip y
                    case -2: case 2:
                        //MX call
                        flipXCounter = !flipXCounter;
                        verticalCounter *= -1;

                        //MY call
                        flipYCounter = !flipYCounter;
                        horizontalCounter *= -1;

                        //dont need to add value to rotation since same as adding 0. no rotations required
                        break;
                    //if -3, same as rotating cw once
                    case -3: case 1:
                        //map WASD movement to the rotation
                        temp = horizontalCounter; 
                        horizontalCounter = -1 * verticalCounter;
                        verticalCounter = temp; 

                        //map lipx, flipy to the rotation
                        temp = (int) flipYCounter;
                        if (flipXCounter){
                            flipXCounter = !flipXCounter;
                            flipYCounter = !flipYCounter;
                        }
                        
                        if (temp){
                            flipXCounter = !flipXCounter;
                            flipYCounter = !flipYCounter;
                        }
                        
                        //since rotating only once, add 1
                        rotationCounter += 1;
                        break;
                    //if 3, rotate 270 degrees, equivalent to ccw once
                    case 3: case -1:
                        //map WASD movement to the rotation
                        temp = horizontalCounter; 
                        horizontalCounter = verticalCounter;
                        verticalCounter = -1 * temp; 

                        //map lipx, flipy to the rotation
                        temp = (int) flipYCounter;
                        if (flipXCounter){
                            flipXCounter = !flipXCounter;
                            flipYCounter = !flipYCounter;
                        }
                       
                        if (temp){
                            flipXCounter = !flipXCounter;
                            flipYCounter = !flipYCounter;
                        }
                        
                        //since rotating only once, set to 1
                        rotationCounter -= 1;
                        break;
                }           
                break;     
            case CCW:
                value %= 4;
                switch(value){
                    
                    //if 2 or negative 2, flip x and flip y
                    case -2: case 2:
                        //MX call
                        flipXCounter = !flipXCounter;
                        verticalCounter *= -1;

                        //MY call
                        flipYCounter = !flipYCounter;
                        horizontalCounter *= -1;

                        break;
                    //if -3, same as rotating ccw once
                    case -3: case 1:
                        //map WASD movement to the rotation
                        temp = horizontalCounter; 
                        horizontalCounter = verticalCounter;
                        verticalCounter = -1 * temp; 

                        //map lipx, flipy to the rotation
                        temp = (int) flipYCounter;
                        if (flipXCounter){
                            flipXCounter = !flipXCounter;
                            flipYCounter = !flipYCounter;
                        }
                        
                        if (temp){
                            flipXCounter = !flipXCounter;
                            flipYCounter = !flipYCounter;
                        }
                        
                        //since rotating only once, set to 1
                        rotationCounter -= 1;
                        break;
                    //if 3, rotate -270 degrees, equivalent to cw once
                    case 3: case -1:
                        //map WASD movement to the rotation
                        temp = horizontalCounter; 
                        horizontalCounter = -1 * verticalCounter;
                        verticalCounter = temp; 

                        temp = (int) flipYCounter;
                        //map lipx, flipy to the rotation
                        if (flipXCounter){
                            flipXCounter = !flipXCounter;
                            flipYCounter = !flipYCounter;
                        }
                    
                        if (temp){
                            flipXCounter = !flipXCounter;
                            flipYCounter = !flipYCounter;
                        }
                        
                        //since rotating only once, set to 1
                        rotationCounter += 1;
                        break;
                }
                break;
            //only defaults if w, a, s, d, cw, ccw, mx, my are not used
            default:
                printf("Invalid Key: %s\n", sensor_values[sensorValueIdx].key);
        }

        //increment process frames
        processed_frames += 1;
        // printf("horizontal; %d, verical: %d, flipx: %d, flipy: %d, rotate: %d\n", horizontalCounter, verticalCounter, flipXCounter, flipYCounter, rotationCounter);

        if (processed_frames % 25 == 0) {
            rotationCounter %= 4; 
            int tempRow;
            for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                switch(rotationCounter){
                    
                    //if 2 or negative 2, flip x and flip y
                    case -2: case 2:
                        //mirror x
                        coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                        //mirror y
                        coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                        break;
                    //if -3, same as rotating cw once
                    case -3: case 1:
                        //rotate clockwise
                        //transpose
                        tempRow = coloredPixels[pixelIndex].row;
                        coloredPixels[pixelIndex].row = coloredPixels[pixelIndex].col;
                        coloredPixels[pixelIndex].col = tempRow;

                        //mirror y
                        coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                        break;
                    //if 3, rotate -270 degrees, equivalent to ccw once
                    case 3: case -1:
                        //rotate ccw
                        //transpose
                        tempRow = coloredPixels[pixelIndex].row;
                        coloredPixels[pixelIndex].row = coloredPixels[pixelIndex].col;
                        coloredPixels[pixelIndex].col = tempRow;

                        //mirror x
                        coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                        break;
                }
                if (flipXCounter) {
                    //flipx
                    coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                }

                if (flipYCounter) {
                    //flipY
                    coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                }

                //move
                coloredPixels[pixelIndex].col += horizontalCounter;
                coloredPixels[pixelIndex].row += verticalCounter;

                //draw pixel
                int index = coloredPixels[pixelIndex].row * width * 3 + coloredPixels[pixelIndex].col * 3;
                // printf("index: %d, pixelRow: %d, pixelCol: %d\n", index, coloredPixels[pixelIndex].row, coloredPixels[pixelIndex].col);
                frame_buffer[index] = coloredPixels[pixelIndex].red;
                frame_buffer[index + 1] = coloredPixels[pixelIndex].green;
                frame_buffer[index + 2] = coloredPixels[pixelIndex].blue;
                
            }
 
            verifyFrame(frame_buffer, width, height, grading_mode);

            //erase frame
            for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex++){
                int index = coloredPixels[pixelIndex].row * width * 3 + coloredPixels[pixelIndex].col * 3;
                frame_buffer[index] = 255;
                frame_buffer[index + 1] = 255;
                frame_buffer[index + 2] = 255;
            }
            
            //reset variable counters
            horizontalCounter = 0;
            verticalCounter = 0;
            rotationCounter = 0;
            flipXCounter = false;
            flipYCounter = false;

            if (sensor_values_count - 1 - sensorValueIdx < 25) {
                break;
            }
        }
    }
    free (coloredPixels);
    return;
}
