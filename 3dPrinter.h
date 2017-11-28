#include <gcode.h>
#include <stdio.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <kipr/botball.h>

// Function executes g-code command, passed letter & number "ie: G28", and passed the full line of g-code //
void execute_command(const char * letter, const char * number,char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){

    // switch statement checks and sorts for command letter //
    switch (letter[0]){

        case 'G':
            if(strcmp(number, "0") == 0){
                gcode_G0(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else if(strcmp(number, "1") == 0){
                gcode_G1(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else if(strcmp(number, "28") == 0){
                gcode_G28(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else if(strcmp(number, "92") == 0){
                gcode_G92(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else{
                printf("Command %s%s does not have a function!\n", letter, number); // If command is not coded
            }

            break;

        case 'M':

            if(strcmp(number, "84") == 0){
                gcode_M84(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else if(strcmp(number, "104") == 0){
                gcode_M104(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else if(strcmp(number, "106") == 0){
                gcode_M106(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else if(strcmp(number, "107") == 0){
                gcode_M107(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else if(strcmp(number, "109") == 0){
                gcode_M109(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else if(strcmp(number, "140") == 0){
                gcode_M140(line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER);
            }
            else{
                printf("Command %s%s does not have a function!\n", letter, number); // If command is not coded
            }

            break;

        case 'T':
            printf("Command %s%s does not have a function!\n", letter, number); // If command is not coded

            break;
    }

}


// Function gets and returns g-code on line at a time. //
char * get_gcode(FILE * fp){
    char * line = NULL; // Variable to hold line.
    size_t len = 0;

    if (fp == NULL){
        exit(EXIT_FAILURE); // If file does not open, Exit Failure!
    }

    if (getline(&line, &len, fp) == -1){
        return "END"; // Returns "END" at the files end. (ie. getline return NULL)
    }

    if (line){
        free(line);
    }

    return line;
}


// Function processes g-code on line at a time. //
void process_gcode(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){

    char char_line[150];
    strcpy(char_line,line);
    char letter[1];
    letter[0] = char_line[0];
    char number[10];
    int i = 0;
    for(i=0;i<sizeof(line);i++){
        if(char_line[i] == letter[0]){
            i++;
            break;
        }
    }

    int j = 0;
    while(1){
        if(isdigit(char_line[i])){
            number[j] = char_line[i];
            j++;
        }
        else{
            break;   
        }
        i++;
    }

    if(strcmp(letter,"G") == 0 || strcmp(letter,"M") == 0 || strcmp(letter,"T") == 0){
        execute_command(letter, number, line, X_AXIS, Y_AXIS, Z1_AXIS, Z2_AXIS, PEN_EXTRUDER); // Executes line of code if command letter is seen, see <gcode.h>
    }

}


// Function Mounts USB stick on to the Wallaby, this allows for g-code files to be processed. //
void mount_USB(){

    // If statement checks to see if directory exists, if it dosen't, it creates the directory. //
    DIR* dir = opendir("/media/usb"); // opendir makes sure the usb directory exists, returns 1 if found directory.
    if (dir){
        printf("Found USB Directory!\n");
        closedir(dir); 
    }
    else{
        printf("Creating USB Directory /media/usb!\n");
        system("sudo  mkdir /media/usb"); // Make Directory to Mount Usb
    }     

    // If statment checks to see if usb has been successfully mounted.
    if(!system("sudo mount /dev/sda1 /media/usb")){
        printf("USB Mount Successful!\n");
    }
    else{
        printf("USB Mount Failed!\n");
        printf("Make Sure USB is Inserted or start over the Wallaby!\n");
        exit(EXIT_FAILURE);
    }
}


// This function unmounts the USB. //
void unmount_USB(){
    system("sudo umount /media/usb"); // Unmount Usb
}


// This function starts a new program that keeps track of the motor position //
void start_motor_encoder_counter (long* encoder_counter, int digital_port, int* child_process_id){
    
    int sensor = digital(digital_port);
    int split;
    split = fork();
    if(split == 0){
        *child_process_id = getpid();
        while(1){
            if(sensor != digital(digital_port)){
                *encoder_counter = *encoder_counter + 1;
                sensor = digital(digital_port);
            }
        }
    }
}


void start_motor_encoder_position (int* encoder_position, long* encoder_counter, int MOTOR_PORT, int* child_process_id){

    clear_motor_position_counter(MOTOR_PORT);
    
    int split;
    int old_encoder_counter = *encoder_counter;
    int current_encoder_counter;
    int old_position = get_motor_position_counter(MOTOR_PORT);
    int current_position = get_motor_position_counter(MOTOR_PORT);
    split = fork();
    if(split == 0){
        *child_process_id = getpid();

        while(1){
            current_position = get_motor_position_counter(MOTOR_PORT);
            if(current_position > old_position){
                current_encoder_counter = *encoder_counter;
                *encoder_position = *encoder_position + (current_encoder_counter - old_encoder_counter);
                old_position = current_position;
                old_encoder_counter = current_encoder_counter;  
            }
            else if(current_position < old_position){
                current_encoder_counter = *encoder_counter;
                *encoder_position = *encoder_position - (current_encoder_counter - old_encoder_counter);
                old_position = current_position;
                old_encoder_counter = current_encoder_counter;  
            }
        }

    }
}


void move_encoder_relative_position (long* encoder_counter, int ticks, int MOTOR_PORT){

    int desired_position = *encoder_counter + abs(ticks) - 4;
    if(ticks > 0){
        move_at_velocity(MOTOR_PORT,1500);
        while(*encoder_counter < desired_position){
        }
    }
    else if(ticks < 0){
        move_at_velocity(MOTOR_PORT,-1500);
        while(*encoder_counter < desired_position){
        }
    }
    else{   
    }
    freeze(MOTOR_PORT);
}


void move_encoder_to_position (int* encoder_position, int target_position, int MOTOR_PORT){

    int desired_position = target_position - *encoder_position;
    if(desired_position > 0){
        move_at_velocity(MOTOR_PORT,1500);
        while(*encoder_position < target_position - 4){
        }
    }
    else if(desired_position < 0){
        move_at_velocity(MOTOR_PORT,-1500);
        while(*encoder_position > target_position + 4){
        }
    }
    else{   
    }
    freeze(MOTOR_PORT);
}

