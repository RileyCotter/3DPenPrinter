// 3D Printer Code for KIPR Wallaby //
// 

// Designed and coded by Riley Cotter 
// Under the supervision of Dr. David Miller 
// Aerospace and Mechanical Engineering
// Gallogly College of Engineeing
// University of Oklahoma

struct AXIS {
    int MOTOR_PORT; // Motor Port on KIPR Wallaby connected to Axis Motor
    int SWITCH_PORT; // Digital Port on KIPR Wallaby connected to Home Zeroing Switch
    int ENCODER_PORT; // Digital Port on KIPR Wallaby connected to Break-Beam Sensor
    float* previous; // Previous G-Code Axis Value
    float* global_position; // Global Position
    int counter_memory_id; // Memory Segment ID for Shared Memory (encoder counter)
    long* encoder_counter; // Shared Memory for communication between child process to parent.
    int position_memory_id; // Memory Segment ID for Shared Memory (encoder counter)
    int* encoder_position; // Shared Memory for communication between child process to parent.
    int counter_process_memory_id; // Memory Segment ID for Shared Memory (child process id) 
    int* counter_child_process; // Shared Memory for communication between child process to parent.
    int position_process_memory_id; // Memory Segment ID for Shared Memory (child process id) 
    int* position_child_process; // Shared Memory for communication between child process to parent.
} AXIS;

struct EXTRUDER {
    int SERVO_PORT; // Servo Port on KIPR Wallaby that pushes the feed button on the pen.
    int SERVO_POSITION_OPEN; // Servo Position when NOT pushing the button on the pen.
    int SERVO_POSITION_CLOSED; // Servo Position when pushing the button on the pen.
} EXTRUDER;

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <kipr/botball.h>
#include <signal.h>
#include <3dPrinter.h>

int main()
{
    // Declare variables for struct pointers //
    float previous_x = 0; // Previous gcode X coordinate (milimeters)
    float previous_y = 0; // Previous gcode Y coordinate (milimeters)
    float previous_z1 = 0; // Previous gcode Z coordinate (milimeters)
    float previous_z2 = 0; // Previous gcode Z coordinate (milimeters)
    float Global_X = 0; // Previous global ("where the program thinks the extrusion tip should be") X coordinate (encoder ticks)
    float Global_Y = 0; // Previous global ("where the program thinks the extrusion tip should be") Y coordinate (encoder ticks)
    float Global_Z1 = 0; // Previous global ("where the program thinks the extrusion tip should be") Z1 coordinate (encoder ticks)
    float Global_Z2 = 0; // Previous global ("where the program thinks the extrusion tip should be") Z2 coordinate (encoder ticks)
        
    struct AXIS X_AXIS; // Declaring X-Axis as a Axis Structure
    struct AXIS Y_AXIS; // Declaring Y-Axis as a Axis Structure
    struct AXIS Z1_AXIS; // Declaring Z1-Axis as a Axis Structure
    struct AXIS Z2_AXIS; // Declaring Z2-Axis as a Axis Structure
    struct EXTRUDER PEN_EXTRUDER; // Declaring Extruder as a Extruder Structure

    /* X-Axis Configuration */
    X_AXIS.MOTOR_PORT = 0;
    X_AXIS.SWITCH_PORT = 0;
    X_AXIS.ENCODER_PORT = 8;
    X_AXIS.previous = &previous_x;
    X_AXIS.global_position = &Global_X;
    X_AXIS.encoder_counter = 0;
    X_AXIS.encoder_position = 0;

    /* Y-Axis Configuration */
    Y_AXIS.MOTOR_PORT = 1;
    Y_AXIS.SWITCH_PORT = 1;
    Y_AXIS.ENCODER_PORT = 9;
    Y_AXIS.previous = &previous_y;
    Y_AXIS.global_position = &Global_Y;
    Y_AXIS.encoder_counter = 0;
    Y_AXIS.encoder_position = 0;

    /* Z1-Axis Configuration */
    Z1_AXIS.MOTOR_PORT = 2;
    Z1_AXIS.SWITCH_PORT = 2;
    Z1_AXIS.previous = &previous_z1;
    Z1_AXIS.global_position = &Global_Z1;

    /* Z2-Axis Configuration */
    Z2_AXIS.MOTOR_PORT = 3;
    Z2_AXIS.SWITCH_PORT = 3;
    Z2_AXIS.previous = &previous_z2;
    Z2_AXIS.global_position = &Global_Z2;
    
    /* Extruder Configuration */
    PEN_EXTRUDER.SERVO_PORT = 2; 
    PEN_EXTRUDER.SERVO_POSITION_OPEN = 1660; 
    PEN_EXTRUDER.SERVO_POSITION_CLOSED = 1770;

    int shared_segment_size = 0x6400;

    /* Allocate a shared memory segment for each shared memory variable.  */
    X_AXIS.counter_memory_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    Y_AXIS.counter_memory_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    X_AXIS.position_memory_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    Y_AXIS.position_memory_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    X_AXIS.counter_process_memory_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    Y_AXIS.counter_process_memory_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    X_AXIS.position_process_memory_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    Y_AXIS.position_process_memory_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    /* Attach the shared memory segment for each shared memory variable. */
    X_AXIS.encoder_counter = (long*) shmat (X_AXIS.counter_memory_id, 0, 0);
    Y_AXIS.encoder_counter = (long*) shmat (Y_AXIS.counter_memory_id, 0, 0);
    X_AXIS.encoder_position = (int*) shmat (X_AXIS.position_memory_id, 0, 0);
    Y_AXIS.encoder_position = (int*) shmat (Y_AXIS.position_memory_id, 0, 0);
    X_AXIS.counter_child_process = (int*) shmat (X_AXIS.counter_process_memory_id, 0, 0);
    Y_AXIS.counter_child_process = (int*) shmat (Y_AXIS.counter_process_memory_id, 0, 0);
    X_AXIS.position_child_process = (int*) shmat (X_AXIS.position_process_memory_id, 0, 0);
    Y_AXIS.position_child_process = (int*) shmat (Y_AXIS.position_process_memory_id, 0, 0);

    /* Start Motor Encoders */
    start_motor_encoder_counter (X_AXIS.encoder_counter, X_AXIS.ENCODER_PORT, X_AXIS.counter_child_process); // Starts program that counts X axis encoder ticks
    start_motor_encoder_counter (Y_AXIS.encoder_counter, Y_AXIS.ENCODER_PORT, Y_AXIS.counter_child_process); // Starts program that counts Y axis encoder ticks
    start_motor_encoder_position (X_AXIS.encoder_position, X_AXIS.encoder_counter, X_AXIS.MOTOR_PORT, X_AXIS.position_child_process); // Starts program that tracks X axis encoder ticks with direction given from motor
    start_motor_encoder_position (Y_AXIS.encoder_position, Y_AXIS.encoder_counter, Y_AXIS.MOTOR_PORT, Y_AXIS.position_child_process); // Starts program that tracks Y axis encoder ticks with direction given from motor
    msleep(2000);

    // Setup Switches as Inputs //
    set_digital_output(X_AXIS.SWITCH_PORT, 0); // Set switch to input
    set_digital_output(Y_AXIS.SWITCH_PORT, 0); // Set switch to input
    set_digital_output(Z1_AXIS.SWITCH_PORT, 0); // Set switch to input
    
    // Enable Servos //
    enable_servos();
    set_servo_position(PEN_EXTRUDER.SERVO_PORT, PEN_EXTRUDER.SERVO_POSITION_OPEN); // Intialize extrusion servo placement

    //*********Setup G-Code File Read**********//
    char file_name[30] = "TestBlock.gcode"; // ***************************************************************************************!!!__File Name To Change__!!!
    char file_location[50] = "/media/usb/"; // file location
    strcat(file_location,file_name); // strcat combines the two strings
    mount_USB(); // Mounts Inserted USB Stick
    FILE * fp; // Intitalize file pointer
    fp = fopen(file_location,"r"); // Open G-Code file
    char * line; // Initialize variable for line of G-Code
    
    //*********Run G-Code Loop**********//
    while(1){
        line = get_gcode(fp); // Gets line of code if avalible, return "END" if the file has reached the end.
        if(strcmp(line,"END") == 0){
            break; // If the file has reached its end, break out of the file reading while loop.
        }
        //printf("%s", line); // Prints line of code to screen if desired, for preformance comment out.
        
        process_gcode(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER); // Processes and excecutes the line of gcode
        
        // if statement checks to see if the left button is pressed on the wallaby. If so, it exists the program. //
        if(left_button() == 1){
            break;   
        }
        
        // if statement checks to see if the right button is pressed on the wallaby. If so, it pauses the program until it is pressed again. //
        if(right_button() == 1){
            msleep(2000);
            set_servo_position(PEN_EXTRUDER.SERVO_PORT, PEN_EXTRUDER.SERVO_POSITION_OPEN); // Sets extruder servo position to the open position
            // while loops waits until right button is pressed to continue the program.
            while(right_button() != 1){
            }
        }
    }

    /* Kill Each Child Proccesses */
    kill(*X_AXIS.counter_child_process,SIGKILL);
    kill(*Y_AXIS.counter_child_process,SIGKILL);
    kill(*X_AXIS.position_child_process,SIGKILL);
    kill(*Y_AXIS.position_child_process,SIGKILL);

    /* Detach the shared memory segment for each process.  */ 
    shmdt (X_AXIS.encoder_counter);
    shmdt (Y_AXIS.encoder_counter);
    shmdt (X_AXIS.encoder_position);
    shmdt (Y_AXIS.encoder_position);
    shmdt (X_AXIS.counter_child_process);
    shmdt (Y_AXIS.counter_child_process);
    shmdt (X_AXIS.position_child_process);
    shmdt (Y_AXIS.position_child_process);

    /* Deallocate the shared memory segment for each process.  */ 
    shmctl (X_AXIS.counter_memory_id, IPC_RMID, 0);
    shmctl (Y_AXIS.counter_memory_id, IPC_RMID, 0);
    shmctl (X_AXIS.position_memory_id, IPC_RMID, 0);
    shmctl (Y_AXIS.position_memory_id, IPC_RMID, 0);
    shmctl (X_AXIS.counter_process_memory_id, IPC_RMID, 0);
    shmctl (Y_AXIS.counter_process_memory_id, IPC_RMID, 0);
    shmctl (X_AXIS.position_process_memory_id, IPC_RMID, 0);
    shmctl (Y_AXIS.position_process_memory_id, IPC_RMID, 0); 

    //*********Close G-Code File Read**********//
    printf("Finished Printing!\n");
    fclose(fp); // Close g-code file
    unmount_USB(); // Unmounts USB stick
    return 0;
}
