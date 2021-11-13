# joycon_probe_control
ジョイスティックでプローブの制御をするプログラム

## 起動方法
### アクセス権限の設定
なし

### ノードの起動
roslaunch joycon_probe_control joycon_probe_control.launch


## 主なトピック名
### joy_node
* ジョイスティックに割り振られたボタン配置(sensor_msgs::Joy型) <br>
/probe_control/joy <br>
### joycon_probe_control_node
* ジョイスティックでプローブを制御(geometry_msgs::Twist型) <br>
/probe_control/joy_probe_control <br>


## 補足: joy_nodeのインストール方法
sudo apt-get install ros-melodic-joy <br>
sudo apt-get install ros-melodic-joystick-drivers <br>

参考url: https://qiita.com/srs/items/9114bb3c27a148e0b855 <br>


