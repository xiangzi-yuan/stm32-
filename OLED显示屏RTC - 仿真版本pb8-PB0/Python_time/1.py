import re

import serial
import binascii,time

def get_time():
    Time = time.strftime('%Y%m%d%H%M%S', time.localtime())
    return Time

# 配置串口基本参数并建立通信
ser = serial.Serial('COM5', 9600, 8, 'N', 1)

# 数据帧
cutime = get_time()
Time_list = re.findall(".{2}", cutime)

print(Time_list)
for i in Time_list:
    d = bytes.fromhex(i)
# 简单的发送16进制字符
    result = ser.write(d)
    time.sleep(1)

# 串口发送数据


# 停止、等待数据，这一步非常关键。timeout压根没用
time.sleep(2)
count=ser.inWaiting()

# 数据的接收
if count>0:

    data=ser.read(count)
    print(data)
    if data!=b'':
        # 将接受的16进制数据格式如b'h\x12\x90xV5\x12h\x91\n4737E\xc3\xab\x89hE\xe0\x16'
        #                      转换成b'6812907856351268910a3437333745c3ab896845e016'
        #                      通过[]去除前后的b'',得到我们真正想要的数据
        print("receive",str(binascii.b2a_hex(data))[2:-1])

ser.close()
