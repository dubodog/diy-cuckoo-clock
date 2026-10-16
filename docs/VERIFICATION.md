# 验证记录

日期：2026-10-16。固件与本次迁移前源文件内容一致，未改变功能。

## 1. 主机逻辑测试：通过

命令：`python3 tools/test_firmware.py`

覆盖24小时时段规则、08—20点13个完整周期、次数准确、不重复触发、夜间T静音、S设整点不立即响、08点恢复、21点抑制、播放器失败后请求收鸟关门、手动关闭及开门中取消、双门镜像步进及鸟门互锁、UART包校验和。

使用C++桩模拟RTC、舵机及播放器。不能证明真实门角度、供电、插头或低价播放器固件兼容。

## 2. Arduino Uno编译：通过

命令：`arduino-cli compile --fqbn arduino:avr:uno firmware/cuckoo_clock`

实际本机调用另指定已存在的开发环境配置文件；该配置含个人路径，不上传。

- Arduino AVR Core 1.8.8
- Servo 1.3.0
- RTClib 2.1.4
- Adafruit BusIO 1.17.4
- Flash：15,506 / 32,256字节
- RAM：722 / 2,048字节

未连接实体UNO上传或测试。

## 3. Anatomy最小运行验证：通过（范围有限）

版本：`e9488296a44211c42701ff71705b7901470ec78f`，Node 24.14.0。
在上游源码目录执行：

```sh
node examples/dial-indicator/build.mjs --light
node scripts/capture.mjs examples/dial-indicator/dial-indicator-light.html --out /tmp/anatomy-smoke.png --width 390
node --test scripts/audit-projection.test.mjs
```

结果：HTML构建成功；390宽视口中图卡358×330；读数18.0px；无横向滚动；console clean。已查看截图，表盘及结构可见。投影回归6/6通过。

首次截图命令误用了index-light.html，返回ERR_FILE_NOT_FOUND；按构建脚本真实输出名dial-indicator-light.html修正后成功。这不是技能运行故障。

未执行上游全量测试、复杂3D审计或本钟定制模型。截图属于官方表盘例子，不是布谷鸟钟效果图。

## 4. 实物状态：未验收

只有石英钟安装完成；三舵机、播放器、电源和RTC尚未整体连接。固定尺寸、门与单臂连接和实际针脚未全部确认。不能因编译或示意图通过而认定制作成功。

## 5. 公开内容检查

提交前检查所有待跟踪文件，排除private目录、订单截图、个人路径和凭据。原始照片与旧教程保留在本地归档。远端公开状态与自动测试在实际推送后另核验。

## 6. GitHub首发验证

仓库可见性PUBLIC，默认分支main。首次提交84ae0fe的[GitHub Actions主机测试](https://github.com/dubodog/diy-cuckoo-clock/actions/runs/38890745796)已成功。首发远端main与本地HEAD一致。公开跟踪27个文件，提交前隐私路径/凭据模式及Markdown本地链接检查通过；源固件内容逐字节一致。
