#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <kipr/botball.h>

const int MOTOR_SPEED = 1500;

// Declaration of Funtions //
// G //
void gcode_G0(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
void gcode_G1(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
void gcode_G28(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
void gcode_G92(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
// M //
void gcode_M84(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
void gcode_M104(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
void gcode_M106(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
void gcode_M107(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
void gcode_M109(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
void gcode_M140(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER);
// End of Declarations //


// G //
void gcode_G0(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){
    char command[150] = "";
    strcpy(command,line);
    //printf("%s", command);

    int i = 0;
    int X = 0;
    int Y = 0;
    int Z = 0;
    int X_dev = 4;
    int Y_dev = 4;
    int Z1_dev = 10;
    int Z2_dev = 10;
    int x_stop = 0;
    int y_stop = 0;
    int z1_stop = 0;
    int z2_stop = 0;
    int servo_used = 0;
    char temp_value[10] = "";
    char x_value_char[10] = "";
    char y_value_char[10] = "";
    char z_value_char[10] = "";
    float x_value_mms = 0;
    float y_value_mms = 0;
    float z_value_mms_1 = 0;
    float z_value_mms_2 = 0;
    int x_position_ticks = 0;
    int y_position_ticks = 0;
    int z_position_ticks_1 = 0;
    int z_position_ticks_2 = 0;

    for(i = 0;i<strlen(command);i++){

        if(command[i] == 'X'){
            while(command[i] != ' ' && i<strlen(command)){
                i++;
                temp_value[0] = command[i];
                strcat(x_value_char, temp_value);
            }
            X = 1;
            x_value_mms = atof(x_value_char);
        }
        if(command[i] == 'Y'){
            while(command[i] != ' ' && i<strlen(command)){
                i++;
                temp_value[0] = command[i];
                strcat(y_value_char, temp_value);
            }
            y_value_mms = atof(y_value_char);
            Y = 1;
        }
        if(command[i] == 'Z'){
            while(command[i] != ' ' && i<strlen(command)){
                i++;
                temp_value[0] = command[i];
                strcat(z_value_char, temp_value);
            }
            z_value_mms_1 = atof(z_value_char);
            z_value_mms_2 = atof(z_value_char);
            Z = 1;
        }
    }
    if(X == 0){
        x_value_mms = *X_AXIS.previous;
        x_stop = 1;
    }
    if(Y == 0){
        y_value_mms = *Y_AXIS.previous;
        y_stop = 1;
    }
    if(Z == 0){
        z_value_mms_1 = *Z1_AXIS.previous;
        z_value_mms_2 = *Z2_AXIS.previous;
        z1_stop = 1;
        z2_stop = 1;
    }

    if(X == 1 && *X_AXIS.previous <= x_value_mms){
        x_position_ticks = 7.55905511811*(x_value_mms - *X_AXIS.previous) + 7.55905511811*(*X_AXIS.global_position);
        *X_AXIS.global_position = 7.55905511811/7.55905511811*(x_value_mms - *X_AXIS.previous) + *X_AXIS.global_position;
    }
    if(X == 1 && *X_AXIS.previous > x_value_mms){
        x_position_ticks = 7.55905511811*(x_value_mms - *X_AXIS.previous) + 7.55905511811*(*X_AXIS.global_position);
        *X_AXIS.global_position = 7.55905511811/7.55905511811*(x_value_mms - *X_AXIS.previous) + *X_AXIS.global_position;
    }
    if(Y == 1 && *Y_AXIS.previous <= y_value_mms){
        y_position_ticks = 7.55905511811*(y_value_mms - *Y_AXIS.previous) + 7.55905511811*(*Y_AXIS.global_position);
        *Y_AXIS.global_position = 7.55905511811/7.55905511811*(y_value_mms - *Y_AXIS.previous) + *Y_AXIS.global_position;
    }
    if(Y == 1 && *Y_AXIS.previous > y_value_mms){
        y_position_ticks = 7.55905511811*(y_value_mms - *Y_AXIS.previous) + 7.55905511811*(*Y_AXIS.global_position);
        *Y_AXIS.global_position = 7.55905511811/7.55905511811*(y_value_mms - *Y_AXIS.previous) + *Y_AXIS.global_position;
    }
    if(Z == 1 && *Z1_AXIS.previous <= z_value_mms_1){
        z_position_ticks_1 = 1030*(z_value_mms_1 - *Z1_AXIS.previous) + 1030*(*Z1_AXIS.global_position);
        z_position_ticks_2 = 1270*(z_value_mms_2 - *Z2_AXIS.previous) + 1270*(*Z2_AXIS.global_position);
        *Z1_AXIS.global_position = (z_value_mms_1 - *Z1_AXIS.previous) + *Z1_AXIS.global_position;
        *Z2_AXIS.global_position = (z_value_mms_2 - *Z2_AXIS.previous) + *Z2_AXIS.global_position;
    }
    if(Z == 1 && *Z1_AXIS.previous > z_value_mms_1){
        z_position_ticks_1 = 1030*(z_value_mms_1 - *Z1_AXIS.previous) + 1030*(*Z1_AXIS.global_position);
        z_position_ticks_2 = 1270*(z_value_mms_2 - *Z2_AXIS.previous) + 1270*(*Z2_AXIS.global_position);
        *Z1_AXIS.global_position = (z_value_mms_1 - *Z1_AXIS.previous) + *Z1_AXIS.global_position;
        *Z2_AXIS.global_position = (z_value_mms_2 - *Z2_AXIS.previous) + *Z2_AXIS.global_position;
    }

    if(Z){
        set_servo_position(PEN_EXTRUDER.SERVO_PORT,PEN_EXTRUDER.SERVO_POSITION_OPEN);
        servo_used = 1;
    }

    if(abs(x_position_ticks - *X_AXIS.encoder_position) > 7.55905511811*10 || abs(y_position_ticks - *Y_AXIS.encoder_position) > 7.55905511811*10){
        set_servo_position(PEN_EXTRUDER.SERVO_PORT,PEN_EXTRUDER.SERVO_POSITION_OPEN);
        servo_used = 1;
    }

    while(X || Y || Z){
        //printf("Loop!\n");
        //X//
        if(X == 1 && *X_AXIS.previous <= x_value_mms){
            if(x_position_ticks-X_dev <= *X_AXIS.encoder_position){
                freeze(X_AXIS.MOTOR_PORT);
                x_stop = 1;
            }
            else{
                move_at_velocity(X_AXIS.MOTOR_PORT,MOTOR_SPEED);
            }
            //printf("X desired position : %d, X actual position : %d\n",x_position_ticks+30,get_motor_position_counter(X_AXIS.MOTOR_PORT));
        }
        if(X == 1 && *X_AXIS.previous > x_value_mms){
            if(*X_AXIS.encoder_position <= x_position_ticks+X_dev){
                freeze(X_AXIS.MOTOR_PORT);
                x_stop = 1;
            }
            else{
                move_at_velocity(X_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            }
            //printf("X desired position : %d, X actual position : %d\n",x_position_ticks+30,get_motor_position_counter(X_AXIS.MOTOR_PORT));
        }

        //Y//
        if(Y == 1 && *Y_AXIS.previous <= y_value_mms){
            if(y_position_ticks-Y_dev <= *Y_AXIS.encoder_position){
                freeze(Y_AXIS.MOTOR_PORT);
                y_stop = 1;
            }
            else{
                move_at_velocity(Y_AXIS.MOTOR_PORT,MOTOR_SPEED);
            }
            //printf("Y desired position : %d, Y actual position : %d\n",y_position_ticks+30,get_motor_position_counter(Y_AXIS.MOTOR_PORT));
        }
        if(Y == 1 && *Y_AXIS.previous > y_value_mms){
            if(*Y_AXIS.encoder_position <= y_position_ticks+Y_dev){
                freeze(Y_AXIS.MOTOR_PORT);
                y_stop = 1;
            }
            else{
                move_at_velocity(Y_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            }
            //printf("Y desired position : %d, Y actual position : %d\n",y_position_ticks+30,get_motor_position_counter(Y_AXIS.MOTOR_PORT));
        }

        //Z//
        if(Z == 1 && *Z1_AXIS.previous <= z_value_mms_1){
            if(z_position_ticks_1-Z1_dev <= get_motor_position_counter(Z1_AXIS.MOTOR_PORT)){
                freeze(Z1_AXIS.MOTOR_PORT);
                z1_stop = 1;
            }
            else{
                move_at_velocity(Z1_AXIS.MOTOR_PORT,MOTOR_SPEED);
            }
            if(z_position_ticks_2-Z2_dev <= get_motor_position_counter(Z2_AXIS.MOTOR_PORT)){
                freeze(Z2_AXIS.MOTOR_PORT);
                z2_stop = 1;
            }
            else{
                move_at_velocity(Z2_AXIS.MOTOR_PORT,MOTOR_SPEED);
            }
            //printf("Z1 desired position : %d, Z1 actual position : %d\n",z_position_ticks+30,get_motor_position_counter(Z1_AXIS.MOTOR_PORT));
            //printf("Z2 desired position : %d, Z2 actual position : %d\n",z_position_ticks+30,get_motor_position_counter(Z2_AXIS.MOTOR_PORT));
        }

        if(Z == 1 && *Z1_AXIS.previous > z_value_mms_1){
            if(get_motor_position_counter(Z1_AXIS.MOTOR_PORT) <= z_position_ticks_1+Z1_dev){
                freeze(Z1_AXIS.MOTOR_PORT);
                z1_stop = 1;
            }
            else{
                move_at_velocity(Z1_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            }
            if(get_motor_position_counter(Z2_AXIS.MOTOR_PORT) <= z_position_ticks_2+Z2_dev){
                freeze(Z2_AXIS.MOTOR_PORT);
                z2_stop = 1;
            }
            else{
                move_at_velocity(Z2_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            }
            //printf("Z1 desired position : %d, Z1 actual position : %d\n",z_position_ticks+30,get_motor_position_counter(Z1_AXIS.MOTOR_PORT));
            //printf("Z2 desired position : %d, Z2 actual position : %d\n",z_position_ticks+30,get_motor_position_counter(Z2_AXIS.MOTOR_PORT));
        }

        //msleep(500);
        //Check if Everything has Finished
        if(x_stop && y_stop && z1_stop && z2_stop){
            break;         
        }
    }

    *X_AXIS.previous = x_value_mms;
    *Y_AXIS.previous = y_value_mms;
    *Z1_AXIS.previous = z_value_mms_1;
    *Z2_AXIS.previous = z_value_mms_2;

    if(servo_used){
        set_servo_position(PEN_EXTRUDER.SERVO_PORT,PEN_EXTRUDER.SERVO_POSITION_CLOSED);
        msleep(2000);
    }

    printf("%2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f\n",x_value_mms,y_value_mms,z_value_mms_1,z_value_mms_2,*X_AXIS.global_position,*Y_AXIS.global_position,*Z1_AXIS.global_position,*Z2_AXIS.global_position,get_motor_position_counter(X_AXIS.MOTOR_PORT)/112., get_motor_position_counter(Y_AXIS.MOTOR_PORT)/160., get_motor_position_counter(Z1_AXIS.MOTOR_PORT)/1030., get_motor_position_counter(Z2_AXIS.MOTOR_PORT)/1270.);
    printf("%2.2f %2.2f \n",*X_AXIS.encoder_position/7.55905511811, *Y_AXIS.encoder_position/7.55905511811);
    printf("\n");

}

void gcode_G1(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){

    set_servo_position(PEN_EXTRUDER.SERVO_PORT,PEN_EXTRUDER.SERVO_POSITION_CLOSED);

    char command[150] = "";
    strcpy(command,line);
    //printf("%s", command);

    int i = 0;
    int X = 0;
    int Y = 0;
    int Z = 0;
    int X_dev = 4;
    int Y_dev = 4;
    int Z1_dev = 10;
    int Z2_dev = 10;
    int x_stop = 0;
    int y_stop = 0;
    int z1_stop = 0;
    int z2_stop = 0;
    int servo_used = 0;
    char temp_value[10] = "";
    char x_value_char[10] = "";
    char y_value_char[10] = "";
    char z_value_char[10] = "";
    float x_value_mms = 0;
    float y_value_mms = 0;
    float z_value_mms_1 = 0;
    float z_value_mms_2 = 0;
    int x_position_ticks = 0;
    int y_position_ticks = 0;
    int z_position_ticks_1 = 0;
    int z_position_ticks_2 = 0;

    for(i = 0;i<strlen(command);i++){

        if(command[i] == 'X'){
            while(command[i] != ' ' && i<strlen(command)){
                i++;
                temp_value[0] = command[i];
                strcat(x_value_char, temp_value);
            }
            X = 1;
            x_value_mms = atof(x_value_char);
        }
        if(command[i] == 'Y'){
            while(command[i] != ' ' && i<strlen(command)){
                i++;
                temp_value[0] = command[i];
                strcat(y_value_char, temp_value);
            }
            y_value_mms = atof(y_value_char);
            Y = 1;
        }
        if(command[i] == 'Z'){
            while(command[i] != ' ' && i<strlen(command)){
                i++;
                temp_value[0] = command[i];
                strcat(z_value_char, temp_value);
            }
            z_value_mms_1 = atof(z_value_char);
            z_value_mms_2 = atof(z_value_char);
            Z = 1;
        }
    }

    if(X == 0){
        x_value_mms = *X_AXIS.previous;
        x_stop = 1;
    }
    if(Y == 0){
        y_value_mms = *Y_AXIS.previous;
        y_stop = 1;
    }
    if(Z == 0){
        z_value_mms_1 = *Z1_AXIS.previous;
        z_value_mms_2 = *Z2_AXIS.previous;
        z1_stop = 1;
        z2_stop = 1;
    }

    if(X == 1 && *X_AXIS.previous <= x_value_mms){
        x_position_ticks = 7.55905511811*(x_value_mms - *X_AXIS.previous) + 7.55905511811*(*X_AXIS.global_position);
        *X_AXIS.global_position = 7.55905511811/7.55905511811*(x_value_mms - *X_AXIS.previous) + *X_AXIS.global_position;
    }
    if(X == 1 && *X_AXIS.previous > x_value_mms){
        x_position_ticks = 7.55905511811*(x_value_mms - *X_AXIS.previous) + 7.55905511811*(*X_AXIS.global_position);
        *X_AXIS.global_position = 7.55905511811/7.55905511811*(x_value_mms - *X_AXIS.previous) + *X_AXIS.global_position;
    }
    if(Y == 1 && *Y_AXIS.previous <= y_value_mms){
        y_position_ticks = 7.55905511811*(y_value_mms - *Y_AXIS.previous) + 7.55905511811*(*Y_AXIS.global_position);
        *Y_AXIS.global_position = 7.55905511811/7.55905511811*(y_value_mms - *Y_AXIS.previous) + *Y_AXIS.global_position;
    }
    if(Y == 1 && *Y_AXIS.previous > y_value_mms){
        y_position_ticks = 7.55905511811*(y_value_mms - *Y_AXIS.previous) + 7.55905511811*(*Y_AXIS.global_position);
        *Y_AXIS.global_position = 7.55905511811/7.55905511811*(y_value_mms - *Y_AXIS.previous) + *Y_AXIS.global_position;
    }
    if(Z == 1 && *Z1_AXIS.previous <= z_value_mms_1){
        z_position_ticks_1 = 1030*(z_value_mms_1 - *Z1_AXIS.previous) + 1030*(*Z1_AXIS.global_position);
        z_position_ticks_2 = 1270*(z_value_mms_2 - *Z2_AXIS.previous) + 1270*(*Z2_AXIS.global_position);
        *Z1_AXIS.global_position = (z_value_mms_1 - *Z1_AXIS.previous) + *Z1_AXIS.global_position;
        *Z2_AXIS.global_position = (z_value_mms_2 - *Z2_AXIS.previous) + *Z2_AXIS.global_position;
    }
    if(Z == 1 && *Z1_AXIS.previous > z_value_mms_1){
        z_position_ticks_1 = 1030*(z_value_mms_1 - *Z1_AXIS.previous) + 1030*(*Z1_AXIS.global_position);
        z_position_ticks_2 = 1270*(z_value_mms_2 - *Z2_AXIS.previous) + 1270*(*Z2_AXIS.global_position);
        *Z1_AXIS.global_position = (z_value_mms_1 - *Z1_AXIS.previous) + *Z1_AXIS.global_position;
        *Z2_AXIS.global_position = (z_value_mms_2 - *Z2_AXIS.previous) + *Z2_AXIS.global_position;
    }

    if(abs(z_position_ticks_1 - get_motor_position_counter(Z1_AXIS.MOTOR_PORT)) > 200 && Z == 1){
        set_servo_position(PEN_EXTRUDER.SERVO_PORT,PEN_EXTRUDER.SERVO_POSITION_OPEN);
        servo_used = 1;
    }

    while(X || Y || Z){
        //printf("Loop!\n");
        //X//
        if(X == 1 && *X_AXIS.previous <= x_value_mms){
            if(x_position_ticks-X_dev <= *X_AXIS.encoder_position){
                freeze(X_AXIS.MOTOR_PORT);
                x_stop = 1;
            }
            else{
                move_at_velocity(X_AXIS.MOTOR_PORT,MOTOR_SPEED);
            }
            //printf("X desired position : %d, X actual position : %d\n",x_position_ticks+30,get_motor_position_counter(X_AXIS.MOTOR_PORT));
        }
        if(X == 1 && *X_AXIS.previous > x_value_mms){
            if(*X_AXIS.encoder_position <= x_position_ticks+X_dev){
                freeze(X_AXIS.MOTOR_PORT);
                x_stop = 1;
            }
            else{
                move_at_velocity(X_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            }
            //printf("X desired position : %d, X actual position : %d\n",x_position_ticks+30,get_motor_position_counter(X_AXIS.MOTOR_PORT));
        }

        //Y//
        if(Y == 1 && *Y_AXIS.previous <= y_value_mms){
            if(y_position_ticks-Y_dev <= *Y_AXIS.encoder_position){
                freeze(Y_AXIS.MOTOR_PORT);
                y_stop = 1;
            }
            else{
                move_at_velocity(Y_AXIS.MOTOR_PORT,MOTOR_SPEED);
            }
            //printf("Y desired position : %d, Y actual position : %d\n",y_position_ticks+30,get_motor_position_counter(Y_AXIS.MOTOR_PORT));
        }
        if(Y == 1 && *Y_AXIS.previous > y_value_mms){
            if(*Y_AXIS.encoder_position <= y_position_ticks+Y_dev){
                freeze(Y_AXIS.MOTOR_PORT);
                y_stop = 1;
            }
            else{
                move_at_velocity(Y_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            }
            //printf("Y desired position : %d, Y actual position : %d\n",y_position_ticks+30,get_motor_position_counter(Y_AXIS.MOTOR_PORT));
        }

        //Z//
        if(Z == 1 && *Z1_AXIS.previous <= z_value_mms_1){
            if(z_position_ticks_1-Z1_dev <= get_motor_position_counter(Z1_AXIS.MOTOR_PORT)){
                freeze(Z1_AXIS.MOTOR_PORT);
                z1_stop = 1;
            }
            else{
                move_at_velocity(Z1_AXIS.MOTOR_PORT,MOTOR_SPEED);
            }
            if(z_position_ticks_2-Z2_dev <= get_motor_position_counter(Z2_AXIS.MOTOR_PORT)){
                freeze(Z2_AXIS.MOTOR_PORT);
                z2_stop = 1;
            }
            else{
                move_at_velocity(Z2_AXIS.MOTOR_PORT,MOTOR_SPEED);
            }
            //printf("Z1 desired position : %d, Z1 actual position : %d\n",z_position_ticks+30,get_motor_position_counter(Z1_AXIS.MOTOR_PORT));
            //printf("Z2 desired position : %d, Z2 actual position : %d\n",z_position_ticks+30,get_motor_position_counter(Z2_AXIS.MOTOR_PORT));
        }

        if(Z == 1 && *Z1_AXIS.previous > z_value_mms_1){
            if(get_motor_position_counter(Z1_AXIS.MOTOR_PORT) <= z_position_ticks_1+Z1_dev){
                freeze(Z1_AXIS.MOTOR_PORT);
                z1_stop = 1;
            }
            else{
                move_at_velocity(Z1_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            }
            if(get_motor_position_counter(Z2_AXIS.MOTOR_PORT) <= z_position_ticks_2+Z2_dev){
                freeze(Z2_AXIS.MOTOR_PORT);
                z2_stop = 1;
            }
            else{
                move_at_velocity(Z2_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            }
            //printf("Z1 desired position : %d, Z1 actual position : %d\n",z_position_ticks+30,get_motor_position_counter(Z1_AXIS.MOTOR_PORT));
            //printf("Z2 desired position : %d, Z2 actual position : %d\n",z_position_ticks+30,get_motor_position_counter(Z2_AXIS.MOTOR_PORT));
        }

        //msleep(500);
        //Check if Everything has Finished
        //printf("x_stop : %d, y_stop : %d, z1_stop : %d, z2_stop : %d\n",x_stop,y_stop,z1_stop,z2_stop);
        if(x_stop && y_stop && z1_stop && z2_stop){
            break;         
        }
    }
    *X_AXIS.previous = x_value_mms;
    *Y_AXIS.previous = y_value_mms;
    *Z1_AXIS.previous = z_value_mms_1;
    *Z2_AXIS.previous = z_value_mms_2;

    if(servo_used){
        set_servo_position(PEN_EXTRUDER.SERVO_PORT,PEN_EXTRUDER.SERVO_POSITION_CLOSED);
        msleep(2000);
    }

    printf("%2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f\n",x_value_mms,y_value_mms,z_value_mms_1,z_value_mms_2,*X_AXIS.global_position,*Y_AXIS.global_position,*Z1_AXIS.global_position,*Z2_AXIS.global_position,get_motor_position_counter(X_AXIS.MOTOR_PORT)/112., get_motor_position_counter(Y_AXIS.MOTOR_PORT)/160., get_motor_position_counter(Z1_AXIS.MOTOR_PORT)/1030., get_motor_position_counter(Z2_AXIS.MOTOR_PORT)/1270.);
    printf("%2.2f %2.2f \n",*X_AXIS.encoder_position/7.55905511811, *Y_AXIS.encoder_position/7.55905511811);
    printf("\n");

}

void gcode_G28(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){
    char command[150] = "";
    strcpy(command,line);
    //printf("%s", command);
    set_servo_position(PEN_EXTRUDER.SERVO_PORT,PEN_EXTRUDER.SERVO_POSITION_OPEN);

    int i = 0;
    int X = 0;
    int Y = 0;
    int Z = 0;

    for(i = 0;i<strlen(command);i++){

        if(command[i] == 'X'){
            move_at_velocity(X_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            while(!digital(X_AXIS.SWITCH_PORT)){
            }
            freeze(X_AXIS.MOTOR_PORT);
            clear_motor_position_counter(X_AXIS.MOTOR_PORT);
            *X_AXIS.encoder_position = 0;
            X = 1;
        }
        if(command[i] == 'Y'){
            move_at_velocity(Y_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            while(!digital(Y_AXIS.SWITCH_PORT)){
            }
            freeze(Y_AXIS.MOTOR_PORT);
            clear_motor_position_counter(Y_AXIS.MOTOR_PORT);
            *Y_AXIS.encoder_position = 0;
            Y = 1;
        }
        if(command[i] == 'Z'){
            move_at_velocity(Z1_AXIS.MOTOR_PORT,-MOTOR_SPEED+300);
            move_at_velocity(Z2_AXIS.MOTOR_PORT,-MOTOR_SPEED);
            while(!digital(Z1_AXIS.SWITCH_PORT)){
            }
            freeze(Z1_AXIS.MOTOR_PORT);
            freeze(Z2_AXIS.MOTOR_PORT);
            clear_motor_position_counter(Z1_AXIS.MOTOR_PORT);
            clear_motor_position_counter(Z2_AXIS.MOTOR_PORT);
            Z = 1;
        }
    }
    if(!X && !Y && !Z){
        move_at_velocity(X_AXIS.MOTOR_PORT,-MOTOR_SPEED);
        while(!digital(X_AXIS.SWITCH_PORT)){
        }
        freeze(X_AXIS.MOTOR_PORT);

        move_at_velocity(Y_AXIS.MOTOR_PORT,-MOTOR_SPEED);
        while(!digital(Y_AXIS.SWITCH_PORT)){
        }
        freeze(Y_AXIS.MOTOR_PORT);
        msleep(250);
        move_at_velocity(Z1_AXIS.MOTOR_PORT,-MOTOR_SPEED+300);
        msleep(250);
        move_at_velocity(Z2_AXIS.MOTOR_PORT,-MOTOR_SPEED);
        while(!digital(Z1_AXIS.SWITCH_PORT)){
        }
        msleep(250);
        freeze(Z1_AXIS.MOTOR_PORT);
        msleep(250);
        freeze(Z2_AXIS.MOTOR_PORT);

        clear_motor_position_counter(X_AXIS.MOTOR_PORT);
        clear_motor_position_counter(Y_AXIS.MOTOR_PORT);
        *X_AXIS.encoder_position = 0;
        *Y_AXIS.encoder_position = 0;
        clear_motor_position_counter(Z1_AXIS.MOTOR_PORT);
        clear_motor_position_counter(Z2_AXIS.MOTOR_PORT);
    }
}


void gcode_G92(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){
}

// M //
void gcode_M84(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){
}
void gcode_M104(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){
}
void gcode_M106(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){
}
void gcode_M107(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){
}
void gcode_M109(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){
}
void gcode_M140(char * line, struct AXIS X_AXIS, struct AXIS Y_AXIS, struct AXIS Z1_AXIS, struct AXIS Z2_AXIS, struct EXTRUDER PEN_EXTRUDER){
}
