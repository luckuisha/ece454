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

typedef struct Pixel {
    int red;
    int green;
    int blue;
    int currentIndex;
    int renderedIndex;
} pixel;

//transpose for rotation
unsigned char *transposeImage(unsigned char *buffer_frame, unsigned width, unsigned height);

//movement functions
unsigned char *processMoveUp(unsigned char *buffer_frame, unsigned width, unsigned height, int offset);
unsigned char *processMoveRight(unsigned char *buffer_frame, unsigned width, unsigned height, int offset);
unsigned char *processMoveDown(unsigned char *buffer_frame, unsigned width, unsigned height, int offset);
unsigned char *processMoveLeft(unsigned char *buffer_frame, unsigned width, unsigned height, int offset);
unsigned char *processRotateCW(unsigned char *buffer_frame, unsigned width, unsigned height,
                               int rotate_iteration);
unsigned char *processRotateCCW(unsigned char *buffer_frame, unsigned width, unsigned height,
                                int rotate_iteration);
unsigned char *processMirrorX(unsigned char *buffer_frame, unsigned int width, unsigned int height, int _unused); 
unsigned char *processMirrorY(unsigned char *buffer_frame, unsigned width, unsigned height, int _unused);

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

/**
 * return transposed image
 * 
 * */
unsigned char *transposeImage(unsigned char *buffer_frame, unsigned width, unsigned height){
    for (int row = 0; row < height; row++){
        for (int column = row; column < width; column++){
            int original_position = row * width * 3 + column * 3;
            int transpose_position = column * height * 3 + row * 3;

            //hold non-transposed in temp storage
            int redTemp = buffer_frame[original_position];
            int greenTemp = buffer_frame[original_position + 1];
            int blueTemp = buffer_frame[original_position + 2];

            //swap non-transposed with transpose position
            buffer_frame[original_position] = buffer_frame[transpose_position];
            buffer_frame[original_position + 1] = buffer_frame[transpose_position + 1];
            buffer_frame[original_position + 2] = buffer_frame[transpose_position + 2];

            //swap transpose position with non-transposed
            buffer_frame[transpose_position] = redTemp;
            buffer_frame[transpose_position + 1] = greenTemp;
            buffer_frame[transpose_position + 2] = blueTemp;
        }
    }

    return buffer_frame;
}

/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param offset - number of pixels to shift the object in bitmap image up
 * @return - pointer pointing a buffer storing a modified 24-bit bitmap image
 * Note1: White pixels RGB(255,255,255) are treated as background. Object in the image refers to non-white pixels.
 * Note2: You can assume the object will never be moved off the screen
 **********************************************************************************************************************/
unsigned char *processMoveUp(unsigned char *buffer_frame, unsigned width, unsigned height, int offset) {

    // store shifted pixels to temporary buffer
    for (int row = 0; row < (height - offset); row++) {
        for (int column = 0; column < width; column++) {
            //  printf("row: %d, col: %d\n", row, column);
            int rendered_position = row * width * 3 + column * 3;
            int original_position = (row + offset) * width * 3 + column * 3;
            buffer_frame[rendered_position] = buffer_frame[original_position];
            buffer_frame[rendered_position + 1] = buffer_frame[original_position + 1];
            buffer_frame[rendered_position + 2] = buffer_frame[original_position + 2];
            
            buffer_frame[original_position] = 255;
            buffer_frame[original_position + 1] = 255;
            buffer_frame[original_position + 2] = 255;
        }
    }

    // return a pointer to the updated image buffer
    return buffer_frame;
}

/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param offset - number of pixels to shift the object in bitmap image left
 * @return - pointer pointing a buffer storing a modified 24-bit bitmap image
 * Note1: White pixels RGB(255,255,255) are treated as background. Object in the image refers to non-white pixels.
 * Note2: You can assume the object will never be moved off the screen
 **********************************************************************************************************************/
unsigned char *processMoveRight(unsigned char *buffer_frame, unsigned width, unsigned height, int offset) {

    // store shifted pixels to temporary buffer
    for (int row = 0; row < height; row++) {
        for (int column = width - 1; column > offset - 1; column--) {
            int rendered_position = row * width * 3 + column * 3;
            int original_position = row * width * 3 + (column - offset) * 3;
            buffer_frame[rendered_position] = buffer_frame[original_position];
            buffer_frame[rendered_position + 1] = buffer_frame[original_position + 1];
            buffer_frame[rendered_position + 2] = buffer_frame[original_position + 2];

            buffer_frame[original_position] = 255;
            buffer_frame[original_position + 1] = 255;
            buffer_frame[original_position + 2] = 255;
        }
    }

    // return a pointer to the updated image buffer
    return buffer_frame;
}

/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param offset - number of pixels to shift the object in bitmap image up
 * @return - pointer pointing a buffer storing a modified 24-bit bitmap image
 * Note1: White pixels RGB(255,255,255) are treated as background. Object in the image refers to non-white pixels.
 * Note2: You can assume the object will never be moved off the screen
 **********************************************************************************************************************/
unsigned char *processMoveDown(unsigned char *buffer_frame, unsigned width, unsigned height, int offset) {

    // store shifted pixels to temporary buffer
    for (int row = height - 1; row > offset - 1; row--) {
        for (int column = 0; column < width; column++) {
            int rendered_position = row * width * 3 + column * 3;
            int original_position = (row - offset) * width * 3 + column * 3;
            buffer_frame[rendered_position] = buffer_frame[original_position];
            buffer_frame[rendered_position + 1] = buffer_frame[original_position + 1];
            buffer_frame[rendered_position + 2] = buffer_frame[original_position + 2];

            buffer_frame[original_position] = 255;
            buffer_frame[original_position + 1] = 255;
            buffer_frame[original_position + 2] = 255;
        }
    }

    // return a pointer to the updated image buffer
    return buffer_frame;
}

/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param offset - number of pixels to shift the object in bitmap image right
 * @return - pointer pointing a buffer storing a modified 24-bit bitmap image
 * Note1: White pixels RGB(255,255,255) are treated as background. Object in the image refers to non-white pixels.
 * Note2: You can assume the object will never be moved off the screen
 **********************************************************************************************************************/
unsigned char *processMoveLeft(unsigned char *buffer_frame, unsigned width, unsigned height, int offset) {   

    // store shifted pixels to temporary buffer
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < (width - offset); column++) {
            int rendered_position = row * width * 3 + column * 3;
            int original_position = row * width * 3 + (column + offset) * 3;
            buffer_frame[rendered_position] = buffer_frame[original_position];
            buffer_frame[rendered_position + 1] = buffer_frame[original_position + 1];
            buffer_frame[rendered_position + 2] = buffer_frame[original_position + 2];

            buffer_frame[original_position] = 255;
            buffer_frame[original_position + 1] = 255;
            buffer_frame[original_position + 2] = 255;
        }
    }

    // return a pointer to the updated image buffer
    return buffer_frame;
}

/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param rotate_iteration - rotate object inside frame buffer clockwise by 90 degrees, <iteration> times
 * @return - pointer pointing a buffer storing a modified 24-bit bitmap image
 * Note: You can assume the frame will always be square and you will be rotating the entire image
 **********************************************************************************************************************/
unsigned char *processRotateCW(unsigned char *buffer_frame, unsigned width, unsigned height,
                               int rotate_iteration) {

    //to rotate an image cw, must transpose and flip horizontally
    return processMirrorX(
        transposeImage(buffer_frame, width, height), 
        width, 
        height, 
        rotate_iteration
    );
}

/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param rotate_iteration - rotate object inside frame buffer counter clockwise by 90 degrees, <iteration> times
 * @return - pointer pointing a buffer storing a modified 24-bit bitmap image
 * Note: You can assume the frame will always be square and you will be rotating the entire image
 **********************************************************************************************************************/
unsigned char *processRotateCCW(unsigned char *buffer_frame, unsigned width, unsigned height,
                                int rotate_iteration) {
    
    //to rotate an image ccw, must transpose and flip vertically
    return processMirrorY(
        transposeImage(buffer_frame, width, height), 
        width, 
        height, 
        rotate_iteration
    );
}

/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param _unused - this field is unused
 * @return
 **********************************************************************************************************************/
unsigned char *processMirrorX(unsigned char *buffer_frame, unsigned int width, unsigned int height, int _unused) {
    
    for (int row = 0; row < height / 2; row++) {
        for (int column = 0; column < width ; column++) {
            int top_position = row * width * 3 + column * 3;
            int bottom_position = (height - row - 1) * width * 3 + column * 3;

            //hold temporary values
            int redTemp = buffer_frame[top_position];
            int greenTemp = buffer_frame[top_position + 1];
            int blueTemp = buffer_frame[top_position + 2];

            //swap top with bottom
            buffer_frame[top_position] = buffer_frame[bottom_position];
            buffer_frame[top_position + 1] = buffer_frame[bottom_position + 1];
            buffer_frame[top_position + 2] = buffer_frame[bottom_position + 2];
            
            //swap bottom with temp
            buffer_frame[bottom_position] = redTemp;
            buffer_frame[bottom_position + 1] = greenTemp;
            buffer_frame[bottom_position + 2] = blueTemp;
        }
    }

    // return a pointer to the updated image buffer
    return buffer_frame;
}

/***********************************************************************************************************************
 * @param buffer_frame - pointer pointing to a buffer storing the imported 24-bit bitmap image
 * @param width - width of the imported 24-bit bitmap image
 * @param height - height of the imported 24-bit bitmap image
 * @param _unused - this field is unused
 * @return
 **********************************************************************************************************************/
unsigned char *processMirrorY(unsigned char *buffer_frame, unsigned width, unsigned height, int _unused) {
    
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width / 2; column++) {
            int left_position = row * height * 3 + column * 3;
            int right_position = row * height * 3 + (width - column - 1) * 3;

            //hold temporary values
            int redTemp = buffer_frame[left_position];
            int greenTemp = buffer_frame[left_position + 1];
            int blueTemp = buffer_frame[left_position + 2];

            //swap left with right
            buffer_frame[left_position] = buffer_frame[right_position];
            buffer_frame[left_position + 1] = buffer_frame[right_position + 1];
            buffer_frame[left_position + 2] = buffer_frame[right_position + 2];

            //swap right with temp
            buffer_frame[right_position] = redTemp;
            buffer_frame[right_position + 1] = greenTemp;
            buffer_frame[right_position + 2] = blueTemp;
        }
    }

    // return a pointer to the updated image buffer
    return buffer_frame;
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
                coloredPixels[colorCounter].currentIndex = index;
                coloredPixels[colorCounter].renderedIndex = -1;
                colorCounter += 1;
            }
        }
    }

    printf("color: %d\n ", coloredPixels[0].red);
    int processed_frames = 0;
    
    //value of key value pair
    int value;

    //number of times to move horizontally. positive to the right, negative to the left
    int horizontalCounter = 0;

    //number of times to move vertically. postive upwards, negative downwards
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
        switch(hash(sensor_values[sensorValueIdx].key)){
            case W:
                verticalCounter += value;
                // frame_buffer = processMoveUp(frame_buffer, width, height, value);
    //            printBMP(width, height, frame_buffer);
                break;
            case A: 
                horizontalCounter -= value;
                // frame_buffer = processMoveLeft(frame_buffer, width, height, value);
    //            printBMP(width, height, frame_buffer);
                break;
            case S:
                verticalCounter -= value;
                break;
            case D:
                horizontalCounter += value;
                break;
            case CW:
                rotationCounter += value;
            // frame_buffer = processRotateCW(frame_buffer, width, height, value);
    //            printBMP(width, height, frame_buffer);
                break;
            case CCW:
                rotationCounter -= value;
                // frame_buffer = processRotateCCW(frame_buffer, width, height, value);
    //            printBMP(width, height, frame_buffer);
                break;
            case MX:

                //must mean that MX move was called and counter = 1, flipping twice is the same as doing nothing
                if (flipXCounter){
                    flipXCounter = 0;
                    break;
                }

                //flipXcounter is 0 here
                flipXCounter = 1;
                // frame_buffer = processMirrorX(frame_buffer, width, height, value);
    //            printBMP(width, height, frame_buffer);
                break;
            case MY:

                // MY was called, counter is 1, flipping twice is eq to doing nothing
                if (flipYCounter) {
                    flipYCounter = 0;
                    break;
                }

                //counter is 0, thus flip
                flipYCounter = 1;
                // frame_buffer = processMirrorY(frame_buffer, width, height, value);
    //            printBMP(width, height, frame_buffer);
                break;

            //only defaults if w, a, s, d, cw, ccw, mx, my are not used
            default:
                printf("Invalid Key");
        }
        processed_frames += 1;
        if (processed_frames % 25 == 0) {

            //modulus the value so it does not over rotate, every 4th is the same spot
            rotationCounter %= 4;
            switch(rotationCounter){
                
                //if 2 or negative 2, flip x and flip y
                case -2: case 2:

                    //flip x counter logic
                    if (flipXCounter)
                        flipXCounter = 0;
                    else flipXCounter = 1;

                    //flip y counter logic
                    if (flipYCounter)
                        flipYCounter = 0;
                    else flipYCounter = 1;

                    rotationCounter = 0;

                    break;
                //if -3, same as rotating cw once
                case -3:
                    rotationCounter = 1;
                    break;
                //if 3, rotate 270 degrees, equivalent to ccw once
                case 3:
                    rotationCounter = -1;
                    break;
                default:
                    break;
            }

            printf("horizontal: %d, vertical: %d, rotate: %d, flipx: %d, flipy: %d \n", horizontalCounter, verticalCounter, rotationCounter, flipXCounter, flipYCounter);
// printf("width: %d, height: %d\n", width, height);
            //move to the left, thus subtract
            if (horizontalCounter < 0){

                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                    coloredPixels[pixelIndex].currentIndex -= horizontalCounter * -3;
                }

                // frame_buffer = processMoveLeft(frame_buffer, width, height, horizontalCounter * -1);
            }
            //move to the right so add
            else if (horizontalCounter > 0){
                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                    coloredPixels[pixelIndex].currentIndex += horizontalCounter * 3;
                }
                // frame_buffer = processMoveRight(frame_buffer, width, height, horizontalCounter);
            }

            //move down so add
            if (verticalCounter < 0){
                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                    coloredPixels[pixelIndex].currentIndex += verticalCounter * width * -3;
                }
                // frame_buffer = processMoveDown(frame_buffer, width, height, verticalCounter * -1);
            }
            //move up so subtract
            else if (verticalCounter > 0){
                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                    coloredPixels[pixelIndex].currentIndex -= verticalCounter * width * 3;
                }
                // frame_buffer = processMoveUp(frame_buffer, width, height, verticalCounter);
            }

            if (rotationCounter < 0){
                frame_buffer = processRotateCCW(frame_buffer, width, height, rotationCounter * -1);
            }
            else if (rotationCounter > 0){
                frame_buffer = processRotateCW(frame_buffer, width, height, rotationCounter);
            }

            if (flipXCounter){
                frame_buffer = processMirrorX(frame_buffer, width, height, flipXCounter);
            }

            if (flipYCounter) {
                for (int pixelIndex = 0; pixelIndex < colorCounter; pixelIndex ++) {
                    coloredPixels[pixelIndex].currentIndex -= verticalCounter * width * 3;
                }
                // frame_buffer = processMirrorY(frame_buffer, width, height, flipYCounter);
            }
 
            verifyFrame(frame_buffer, width, height, grading_mode);

            //reset counters
            horizontalCounter = 0;

            verticalCounter = 0;

            rotationCounter = 0;

            flipXCounter = 0;

            flipYCounter = 0;
        }
    }
    return;
}
