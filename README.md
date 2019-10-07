# 基于STC微控制器的CW Keyer

这个CW Keyer使用STC15W104微控制器，用于牛牛QRP电台，当然也能用于其他类似的电台上。

## 使用说明

### 开机上电

上电时，Keyer自动识别是手键（Straight Key）还是自动键（Paddle），识别的结果通过侧音用莫尔斯码报告：

* 自动键 - P
* 手键 - S

报告电键类型之后，进入到工作模式。

### 自动发报

短按SW键即开始自动发报，报文如下：

```
CQ CQ DE <CALLSIGN> <CALLSIGN> PSE K
```

其中，``<CALLSIGN>``在设置功能中设定，如果没有设定，默认为``NNK``。

自动发报过程中，按电键或SW键即可停止发报。

### 设置

长按（超过2秒），Keyer会发出``?``的莫尔斯码，进入到设置模式。

设置模式下，需用自动键输入相关命令。不支持手键输入。

设置模式中的命令有：

* V - 报告固件版本号。版本号由三位数字构成。
* 0 - 重置所有设置项，回到初始值。
* W - 报告自动键的速度，单位为WPM。默认为15 WPM。
* E - 减小自动键的速度，最小值为5 WPM。
* T - 增大自动键的速度，最大值为50 WPM。
* A/B - 设置自动键为Iambic A模式/Iambic B模式。默认为B模式。
* F - 设置侧音频率。这个命令会报告当前的频率值，单位是Hz。之后，可以通过E/T来降低/升高侧音频率，最后按SW键确认。频率范围是400Hz至1500Hz，步进是50Hz。默认为700Hz。
* D - 设置收发切换的延时。这个命令会报告当前的延时值，单位是毫秒。之后，可以通过E/T来减少或增大延时，最后按SW键确认。延时范围是0毫秒至2秒，步进是50毫秒。默认为600毫秒。
* C - 设置呼号，用于自动发报。这个命令会提示``?``，听到提示后，用自动键正常拍法呼号即可。拍法完成后，按SW键确认。确认时会播报记录的呼号。呼号有长度限制，不能超过11个字符。
* L/R - 设置自动键为左手模式/右手模式。默认为右手模式。

完成设置之后，按一次SW键，即保存所有变更，并回到工作模式。

### 关于升级固件

固件可以用USB转TTL串口线来进行升级。

首先，断开电源，进行连线：USB转TTL串口线的RxD接自动键的Dah；TxD接自动键的Dit；GND接自动键的GND。并将USB转TTL串口线接入计算机的USB插口。

之后，用固件升级程序，完成固件升级：

* 先在计算机上启动固件升级程序，按照提示操作，开始升级固件。程序进入等待状态后，进入下一步。
* 这时，给电台上电。固件升级程序会自动检测上电过程，完成固件升级。

固件升级完毕后，即可断开USB转TTL串口线，插入电键，重新给机器上电，开始使用新版本的固件。
