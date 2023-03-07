# AKSOBOX通信协议

带“ ”为char字符串类型，否则为int整型。

所有大小写必须一致，注意逗号！

## 1.设置（med）

### 1.1  内容

上位机向服务器此话题发送消息，STM32订阅此话题。

包括：药品名称，数量，剂量，频率，设置时间。

### 1.2 格式

topic: setting/device1

```
{
  "setMed":"随便填 eg:add",
  "medName":"药品名称",
  "medAll":药品总数,
  "medNum":药品单次服用颗数,
  "medTimes":药品每日服用次数,	（如果参数为1则后面只需要写hour1和minute1，为3则需要写到定时3）
  "setTime":"clock",
  "hour1":小时,
  "minute1":分钟,
  "hour2":小时,
  "minute2":分钟,
  "hour3":小时,
  "minute3":分钟
}
```

例子：

```
{
  "setMed":"add",
  "medName":"连花清瘟胶囊",
  "medAll":10,
  "medNum":2,
  "medTimes":3,
  "setTime":"clock",
  "hour1":6,
  "minute1":50,
  "hour2":11,
  "minute2":35,
  "hour3":18,
  "minute3":7				（个位数前不能有0）
}
```

设置当前仓内药物数量

```
{
  "setMedAll":数量
}
```

检测是否在线

```
{
  "hello":xx(任意)
}
```

回复topic:	message/device1

```
{
  "hello":1
}
```

远程吐药/或者按按键

```
{
  "launch":1
}
```

查询当前余量

```
{
  "check":1
}
```

回复topic:	message/device1

```
{
  "medicine":数量
}
```



## 2.通信（mage）

### 2.1  内容

STM32下位机向服务器此话题发送消息，上位机订阅此话题。

包括：当前状态

### 2.2 格式

机器连接到服务器后发送信息。

topic： public/akso

```
{
  "LoginSuc": 1,
  "DeviceId": "device1",(设备号，如device1即后续订阅话题都为xx/device1，device2为xx/device2)
  "MedicineCount":当前剩余药物数量
}
```

topic： message/device1

上位机发送设置信息后的机器反馈

```
{
  "SetSuc": 1/0		(1正确2错误)
}
```

机器吐药完成后发送的反馈消息

```
{
  "WorkState": 1
}
```

topic： heart/device1

```
{
  "Temperature": 温度,
  "MedicineCount": 当前剩余药品总量
}
```

## 3.错误(err)

### 3.1 内容

STM32下位机向服务器此话题发送消息，上位机订阅此话题。

包括：错误类型

### 3.2 格式

topic：err/device1

```
{
  "Err": "Empty"
}
```

