/**************************************************************************************
 * joy-conでプローブを操作するプログラム
 * 1. joicon_probe_controlがjoy_nodeから"/joy"をsubscribe
 * 2. "/joy"の指令値に応じてjoicon_probe_controlからprobe_v2_controlに"/probe_control/joycon_probe_control"をpublish
 * 3. プローブの姿勢を初期化するときのみ, "/probe_control/posture_initialization"をpublish
 * 
 * ~joyconのボタン操作について~
 * axes[左ローリング, 前ピッチング, 右ねじヨーイング]
 * buttons[右手人差し指, 右手親指]
 * 
 * ~姿勢制御について~
 * 姿勢変換はbryant角の順番以外を認めない
 * yaw_flag, pitch_flag, roll_flagを定義し, 
 * フラグを立てる場合は、yaw_flag→pitch_flag→roll_flagの順のみを受け付ける
 * ***********************************************************************************/
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Int16.h>
#include <sensor_msgs/Joy.h>
#include <vector>

class JoyconProbeControl
{
protected:
    ros::NodeHandle nh_;

    ros::Publisher joycon_probe_control_pub_;
    ros::Publisher posture_initialization_pub_;
    ros::Publisher running_mode_pub_;

    ros::Subscriber joy_node_sub_;

    geometry_msgs::Twist joycon_probe_control_;
    std_msgs::Bool posture_initialization_;
    std_msgs::Int16 running_mode_;
    std::vector<double> joy_axes_; //[左ローリング, 前ピッチング, 右ねじヨーイング]
    
public:
    JoyconProbeControl();

    void publishJoyconProbeControl();
    void publishPostureInitialization();
    void publishJoyRunningMode(const int& mode);
    void joynodeCallback(const sensor_msgs::Joy& msg);
    bool write_flag_;
};

JoyconProbeControl::JoyconProbeControl()
{
    joycon_probe_control_pub_ = nh_.advertise<geometry_msgs::Twist>("joy_probe_control", 1);
    posture_initialization_pub_ = nh_.advertise<std_msgs::Bool>("joy_posture_initialization", 1);
    running_mode_pub_ = nh_.advertise<std_msgs::Int16>("joy_running_mode", 1);

    joy_node_sub_ = nh_.subscribe("joy", 1, &JoyconProbeControl::joynodeCallback, this);

    joy_axes_.resize(3);
    write_flag_ = false;
}

/*probe_v2_controlに目標値をpublishする関数*/
void JoyconProbeControl::publishJoyconProbeControl()
{
    joycon_probe_control_pub_.publish(joycon_probe_control_);
}

/*プローブの姿勢を初期化する関数*/
void JoyconProbeControl::publishPostureInitialization()
{
    posture_initialization_.data = true;
    posture_initialization_pub_.publish(posture_initialization_);

}

void JoyconProbeControl::publishJoyRunningMode(const int& mode)
{
    running_mode_.data = mode;
    running_mode_pub_.publish(running_mode_);
}

void JoyconProbeControl::joynodeCallback(const sensor_msgs::Joy& msg)
{
    for(int i=0; i<3; i++)
    {
        joy_axes_.at(i) = msg.axes[i];
        if(abs(msg.axes[i]) < 0.5)
        {
            joy_axes_.at(i) = 0.0;
        }
    }
    if(msg.buttons[3] == 1)
    {
        publishJoyRunningMode(4); //スタート地点に走行
    }
    else if(msg.buttons[4] == 1)
    {
        publishJoyRunningMode(5); //上向きに走行
    }
    else if(msg.buttons[2] == 1)
    {
        publishJoyRunningMode(3); //下向きに走行
    }
    if(msg.buttons[1] == 1)
    {
        publishPostureInitialization();
    }
    else if(msg.buttons[0] == 0 && write_flag_)
    {
        joycon_probe_control_.linear.x = 0.0;
        joycon_probe_control_.linear.y = - joy_axes_.at(0);
        joycon_probe_control_.linear.z = joy_axes_.at(1);
        joycon_probe_control_.angular.x = 0.0;
        joycon_probe_control_.angular.y = 0.0;
        joycon_probe_control_.angular.z = 0.0;
        publishJoyconProbeControl();
    }
    else if(msg.buttons[0] == 1)
    {
        if(pow(joy_axes_.at(0), 2) + pow(joy_axes_.at(1), 2) + pow(joy_axes_.at(2), 2) < 0.1)
        {
            write_flag_ = true;
        }
        else if(write_flag_)
        {
            joycon_probe_control_.linear.x = 0.0;
            joycon_probe_control_.linear.y = 0.0;
            joycon_probe_control_.linear.z = 0.0;
            joycon_probe_control_.angular.x = joy_axes_.at(2);
            joycon_probe_control_.angular.y = - joy_axes_.at(1);
            joycon_probe_control_.angular.z = - joy_axes_.at(0);
            publishJoyconProbeControl();
            write_flag_ = false;
        }
    }


}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "joycon_probe_control");
    JoyconProbeControl joy1;
    ros::spin();
    return 0;
}