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

    int processed_frames = 0;
    
    //value of key value pair
    int value;

    char *prevKey = "NA";
    char *currKey;

    //number of times to move horizontally. positive to the right, negative to the left
    int horizontalCounter = 0;

    //number of times to move vertically. negative upwards, positive downwards
    int verticalCounter = 0;

    //number of times to rotate. positive is CW, negative is CCW
    int rotationCounter = 0;

    //number of times to flip horizontally
    int flipXCounter = 0;

    //number of times to flip vertically
    int flipYCounter = 0;  

    for (int sensorValueIdx = 0; sensorValueIdx < sensor_values_count; sensorValueIdx++) {
        // printf("Processing sensor value #%d: %s, %d\n", sensorValueIdx, sensor_values[sensorValueIdx].key,
        //        sensor_values[sensorValueIdx].value);

        value = sensor_values[sensorValueIdx].value;
        currKey = sensor_values[sensorValueIdx].key;

        // printf("%s, %s\n", currKey, prevKey);
        
        // printf("horizontal; %d, verical: %d, flipx: %d, flipy: %d\n", horizontalCounter, verticalCounter, flipXCounter, flipYCounter);
        switch(hash(currKey)){
            //current instruction is move, check if prev is not move 
            case W: case A: case S: case D:
                switch (hash(prevKey)){

                    //if rotate, then rotate now
                    case CW: case CCW:

                        //modulus the value so it does not over rotate, every 4th is the same spot
                        rotationCounter %= 4;
                        switch(rotationCounter){
                            
                            //if 2 or negative 2, flip x and flip y
                            case -2: case 2:
                                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                                    coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                                    coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                                }
                                rotationCounter = 0;
                                break;
                            //if -3, same as rotating cw once
                            case -3: case 1:
                                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                                    //transpose
                                    int tempRow = coloredPixels[pixelIndex].row;
                                    coloredPixels[pixelIndex].row = coloredPixels[pixelIndex].col;
                                    coloredPixels[pixelIndex].col = tempRow;

                                    //mirror y
                                    coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                                }
                                rotationCounter = 0;
                                break;
                            //if 3, rotate 270 degrees, equivalent to ccw once
                            case 3: case -1:
                                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {

                                    //transpose
                                    int tempRow = coloredPixels[pixelIndex].row;
                                    coloredPixels[pixelIndex].row = coloredPixels[pixelIndex].col;
                                    coloredPixels[pixelIndex].col = tempRow;

                                    //mirror x
                                    coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                                }
                                rotationCounter = 0;
                                break;
                        }
                    
                    case MX: case MY:
                        if (flipXCounter){
                            for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                               coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                            }
                        }
                        flipXCounter = 0;

                        if (flipYCounter) {
                            for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                                coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                            }
                        }
                        flipYCounter = 0;
                        break;
                }
                switch(hash(currKey)){
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
                }
                prevKey = currKey;
                break;
            case CW: case CCW:
                switch (hash(prevKey)){

                    //if move, then move now
                    case A: case D: case W: case S:
                        for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                            coloredPixels[pixelIndex].col += horizontalCounter;
                            coloredPixels[pixelIndex].row += verticalCounter;
                        }
                        horizontalCounter = 0;
                        verticalCounter = 0;
                        break; 
                    
                    case MX: case MY:
                        if (flipXCounter){
                            for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                               coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                            }
                        }
                        flipXCounter = 0;

                        if (flipYCounter) {
                            for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                                coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                            }
                        }
                        flipYCounter = 0;
                        break;
                }
                switch (hash(currKey)){
                    case CW:
                        rotationCounter += value;
                        break;
                    case CCW:
                        rotationCounter -= value;
                        break;
                }
                prevKey = currKey;
                break;
            case MX: case MY:
                switch (hash(prevKey)){

                    //if move, then move now
                    case W: case A: case S: case D:
                        for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                            coloredPixels[pixelIndex].col += horizontalCounter;
                            coloredPixels[pixelIndex].row += verticalCounter;
                        }
                        horizontalCounter = 0;
                        verticalCounter = 0;
                        break; 
                    
                     //if rotate, then rotate now
                    case CW: case CCW:

                        //modulus the value so it does not over rotate, every 4th is the same spot
                        rotationCounter %= 4;
                        switch(rotationCounter){
                            
                            //if 2 or negative 2, flip x and flip y
                            case -2: case 2:
                                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                                    coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                                    coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                                }
                                rotationCounter = 0;
                                break;
                            //if -3, same as rotating cw once
                            case -3: case 1:
                                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                                    //transpose
                                    int tempRow = coloredPixels[pixelIndex].row;
                                    coloredPixels[pixelIndex].row = coloredPixels[pixelIndex].col;
                                    coloredPixels[pixelIndex].col = tempRow;

                                    //mirror y
                                    coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                                }
                                rotationCounter = 0;
                                break;
                            //if 3, rotate 270 degrees, equivalent to ccw once
                            case 3: case -1:
                                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {

                                    //transpose
                                    int tempRow = coloredPixels[pixelIndex].row;
                                    coloredPixels[pixelIndex].row = coloredPixels[pixelIndex].col;
                                    coloredPixels[pixelIndex].col = tempRow;

                                    //mirror x
                                    coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                                }
                                rotationCounter = 0;
                                break;
                        }
                }
                switch (hash(currKey)){
                    case MX:

                        //must mean that MX move was called and counter = 1, flipping twice is the same as doing nothing
                        if (flipXCounter){
                            flipXCounter = 0;
                            break;
                        }

                        //flipXcounter is 0 here
                        flipXCounter = 1;
                        break;
                    case MY:

                        // MY was called, counter is 1, flipping twice is eq to doing nothing
                        if (flipYCounter) {
                            flipYCounter = 0;
                            break;
                        }

                        //counter is 0, thus flip
                        flipYCounter = 1;
                        break;
                }
                prevKey = currKey;
                break;
            //only defaults if w, a, s, d, cw, ccw, mx, my are not used
            default:
                printf("Invalid Key");
        }
        processed_frames += 1;
        if (processed_frames % 25 == 0) {
            // printf("horizontal; %d, verical: %d, flipx: %d, flipy: %d\n", horizontalCounter, verticalCounter, flipXCounter, flipYCounter);
            for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                coloredPixels[pixelIndex].col += horizontalCounter;
                coloredPixels[pixelIndex].row += verticalCounter;
            }
            horizontalCounter = 0;
            verticalCounter = 0;

            rotationCounter %= 4;
            switch(rotationCounter){
                
                //if 2 or negative 2, flip x and flip y
                case -2: case 2:
                    for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                        coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                        coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                    }
                    rotationCounter = 0;
                    break;
                //if -3, same as rotating cw once
                case -3: case 1:
                    for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                        //transpose
                        int tempRow = coloredPixels[pixelIndex].row;
                        coloredPixels[pixelIndex].row = coloredPixels[pixelIndex].col;
                        coloredPixels[pixelIndex].col = tempRow;

                        //mirror y
                        coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                    }
                    rotationCounter = 0;
                    break;
                //if 3, rotate 270 degrees, equivalent to ccw once
                case 3: case -1:
                    for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {

                        //transpose
                        int tempRow = coloredPixels[pixelIndex].row;
                        coloredPixels[pixelIndex].row = coloredPixels[pixelIndex].col;
                        coloredPixels[pixelIndex].col = tempRow;

                        //mirror x
                        coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                    }
                    rotationCounter = 0;
                    break;
            }

            if (flipXCounter){
                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                    coloredPixels[pixelIndex].row = height - 1 - coloredPixels[pixelIndex].row;
                }
            }
            flipXCounter = 0;

            if (flipYCounter) {
                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                    coloredPixels[pixelIndex].col = width - 1 - coloredPixels[pixelIndex].col;
                }
            }
            flipYCounter = 0;

            prevKey = "NA"; 

            for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex++){
                int index = coloredPixels[pixelIndex].row * width * 3 + coloredPixels[pixelIndex].col * 3;
                // printf("index: %d, pixelRow: %d, pixelCol: %d\n", index, coloredPixels[pixelIndex].row, coloredPixels[pixelIndex].col);
                frame_buffer[index] = coloredPixels[pixelIndex].red;
                frame_buffer[index + 1] = coloredPixels[pixelIndex].green;
                frame_buffer[index + 2] = coloredPixels[pixelIndex].blue;
            }
 
            verifyFrame(frame_buffer, width, height, grading_mode);
            for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex++){
                int index = coloredPixels[pixelIndex].row * width * 3 + coloredPixels[pixelIndex].col * 3;
                frame_buffer[index] = 255;
                frame_buffer[index + 1] = 255;
                frame_buffer[index + 2] = 255;
            }
        }
    }
    free (coloredPixels);
    return;
}
