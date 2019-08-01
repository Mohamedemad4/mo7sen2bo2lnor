#include <ros.h>
#include "CytronMotorDriver.h"
//#include "utilsVel.h"
#include "Stepper.h"
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int16.h>
ros::NodeHandle  nh;
float timeout;
Stepper Collect(200, 13,12,3,2);
CytronMD r_cy(PWM_DIR, 10, 11);  // PWM 1 = Pin 3, DIR 1 = Pin 4.
CytronMD l_cy(PWM_DIR, 9, 5); 
#include "utilsVel.h"
float linearToPWM(float x){
    if (x<1){
        return safe(x*255);
    }
    if (x>1){
        return 255;
    }
    if(x==1){
        return 255;
    }
    return 255;
}


float angularToPWM(float x){
    if (x<.5){
        return safe(x*255);
    }
    if (x>.5){
        return 255;
    }
}

float safe(float spd){
    if (spd<128){
        return 128;
    }
    return spd;
}


void stopMotors(){
    r_cy.setSpeed(0);
    l_cy.setSpeed(0);
    }   
void handelAng(float z){
    if (z>0){ //postive angular vel,turn right
        r_cy.setSpeed(angularToPWM(abs(z))); 
        l_cy.setSpeed(-angularToPWM(abs(z)));
        //nh.loginfo("Pos Ang r_cy+ l_cy-");
    }
        if (z<0){ //neg angular vel,turn left
        r_cy.setSpeed(-angularToPWM(abs(z))); 
        l_cy.setSpeed(angularToPWM(abs(z)));
        //nh.loginfo("Neg Ang r- l_cy+");
    }
}
void shake_it(const geometry_msgs::Twist& received_msg){
 float x=received_msg.linear.x;
 float z=received_msg.angular.z;
 if (x!=0){
    if (x>0){
        float PWMSpeed=linearToPWM(x);
        //nh.loginfo("PWM is");
        //nh.loginfo(linearToPWM);
        r_cy.setSpeed(PWMSpeed);
        l_cy.setSpeed(PWMSpeed);
    }
    if (x<0){
        float PWMSpeed=linearToPWM(abs(x));
        //nh.loginfo("PWM is");
        //nh.loginfo(-PWMSpeed);
        r_cy.setSpeed(-PWMSpeed);
        l_cy.setSpeed(-PWMSpeed);
    }
 }
 if (z!=0){
     handelAng(z);
 }
 delay(timeout);
 stopMotors();

}

void move_stepper(const std_msgs::Int16& received_msg){
    digitalWrite(A4,1);
    digitalWrite(A5,1);
Collect.step(received_msg.data);
   digitalWrite(A4,0);
    digitalWrite(A5,0);
//delay(received_msg*10);
}
ros::Subscriber<geometry_msgs::Twist> subT("/cmd_vel", shake_it );
ros::Subscriber<std_msgs::Int16> subS("/move_stepper", move_stepper );



void setup(){
        nh.initNode();
        pinMode(A4,OUTPUT);
        pinMode(A5,OUTPUT);
        
        //nh.getHardware()->setBaud(115200)
        nh.subscribe(subT);
        nh.subscribe(subS);
        
        while(!nh.connected()) {nh.spinOnce();}
        if (!nh.getParam("~timeout", &timeout,1)){
            timeout=100;
        }
        Collect.setSpeed(20);

}
void loop(){
     nh.spinOnce();
}