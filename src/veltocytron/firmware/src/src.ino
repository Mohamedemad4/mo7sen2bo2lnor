#include <ros.h>
#include "CytronMotorDriver.h"
#include <geometry_msgs/Twist.h>

ros::NodeHandle  nh;
CytronMD r(PWM_DIR, 10, 11);  // PWM 1 = Pin 3, DIR 1 = Pin 4.
CytronMD l(PWM_DIR, 9, 5); 
float linearToPWM(float x){
    if (x<1){
        return safe(x*255);
    }
    if (x>1){
        return 255;
    }
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
void shake_it(const geometry_msgs::Twist& received_msg){
 float x=received_msg.linear.x;
 float z=received_msg.angular.z;
 if (x!=0){
    r.setSpeed(linearToPWM(x));
    l.setSpeed(linearToPWM(x));
 }

 if (z!=0){
     handelAng(z);
 }
}
void handelAng(float z){
    if (z>0){ //postive angular vel,turn right
        r.setSpeed(angularToPWM(abs(z))); 
        l.setSpeed(-angularToPWM(abs(z)));
        //nh.loginfo("Pos Ang r+ l-");
    }
        if (z<0){ //neg angular vel,turn left
        r.setSpeed(-angularToPWM(abs(z))); 
        l.setSpeed(angularToPWM(abs(z)));
        //nh.loginfo("Neg Ang r- l+");
    }
}

ros::Subscriber<geometry_msgs::Twist> sub("/cmd_vel", shake_it );


void setup(){
        nh.initNode();
        nh.subscribe(sub);
}
void loop(){
     nh.spinOnce();
}