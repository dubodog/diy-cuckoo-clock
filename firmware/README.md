# UNO 固件

Arduino IDE打开`cuckoo_clock/cuckoo_clock.ino`，选择Arduino Uno和实际端口。需要Servo、RTClib（及其依赖Adafruit BusIO）；Wire、SoftwareSerial由AVR板包提供。

CLI参考（会下载开发依赖）：

```sh
arduino-cli core update-index
arduino-cli core install arduino:avr@1.8.8
arduino-cli lib install "Servo" "RTClib"
arduino-cli compile --fqbn arduino:avr:uno firmware/cuckoo_clock
```

实际验证版本见`docs/VERIFICATION.md`。本次只是整理既有程序，没有改变接线或角度。`Config.h`中的135/45、45/135、150/30都是待实物校准的设计角度，不能装好门后直接盲跑。

串口监视器9600波特，发送行尾换行：

| 指令 | 作用 |
|---|---|
| `P` | 输出RTC时间与状态 |
| `S 2026 10 16 8 30 0` | 示例：设置RTC；请换成当前实际时间 |
| `T` | 白天测试两次布谷；夜间/故障/时间无效时拒绝 |
| `C` | 请求收鸟后关门 |

S校时不会立刻触发整点报时。石英钟请用背面旋钮另调到相同时间。

播放器从`mp3/0001.mp3`读一次布谷；程序重复播放相应次数。BUSY应在播放期间为低。未开始或超时会进入故障并请求收鸟关门。

## 实体限制

舵机无位置反馈，所谓“到位”是程序角度到达目标，不代表检测到了门真的打开。掉电时无法执行收鸟/关门；开机即向舵机发初始角度，未验证任意断电姿态的恢复安全。首次台架测试先卸载门和鸟，并由成人核对实际运动范围。
