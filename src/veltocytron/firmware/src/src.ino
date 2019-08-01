#include <ros.h>
#include "CytronMotorDriver.h"
#include <geometry_msgs/Twist.h>

ros::NodeHandle  nh;
float timeout;
CytronMD r(PWM_DIR, 10, 11);  // PWM 1 = Pin 3, DIR 1 = Pin 4.
CytronMD l(PWM_DIR, 9, 5); 
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
void shake_it(const geometry_msgs::Twist& received_msg){
 float x=received_msg.linear.x;
 float z=received_msg.angular.z;
 if (x!=0){
    if (x>0){
        float PWMSpeed=linearToPWM(x);
        //nh.loginfo("PWM is");
        //nh.loginfo(linearToPWM);
        r.setSpeed(PWMSpeed);
        l.setSpeed(PWMSpeed);
    }
    if (x<0){
        float PWMSpeed=linearToPWM(abs(x));
        //nh.loginfo("PWM is");
        //nh.loginfo(-PWMSpeed);
        r.setSpeed(-PWMSpeed);
        l.setSpeed(-PWMSpeed);
    }
 }

 if (z!=0){
     handelAng(z);
 }
 delay(timeout);
 stopMotors();

}
void stopMotors(){
    r.setSpeed(0);
    l.setSpeed(0);
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
        //nh.getHardware()->setBaud(115200)
        nh.subscribe(sub);
        while(!nh.connected()) {nh.spinOnce();}
        if (!nh.getParam("~timeout", &timeout,1)){
            timeout=100;
        }

}
void loop(){
     nh.spinOnce();
}