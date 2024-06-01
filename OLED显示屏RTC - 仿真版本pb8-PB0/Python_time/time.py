import re

import serial
import time

def get_time():
    Time = time.strftime('%Y%m%d%H%M%S', time.localtime())
    return Time
# 串口打开函数
def open_ser():
    port = 'com7'  # 串口号
    baudrate = 9600  # 波特率
    try:
        global ser
        ser = serial.Serial('COM7', 9600, 8, 'N', 1) # 'COM7', 9600, bytesize=8, parity='N', stopbits=1
        if (ser.isOpen() == True):
            print("串口打开成功")
    except Exception as exc:
        print("串口打开异常", exc)


# 数据发送
def send_msg():
    try:
        # send_datas = input("请输入要发送的数据\n")
        # ser.write(str(send_datas).encode("gbk"))
        # print("已发送数据:",send_datas)
        time = get_time()
        Time_list = re.findall(".{2}",time)
        new_time = " ".join(Time_list)
        d = bytes.fromhex(new_time)
        print("d:",d)
        print("Time_list:", Time_list)
        ser.write(d)
        print("已发送数据:", new_time)
    except Exception as exc:
        print("发送异常", exc)


# 接收数据
def read_msg():
    try:
        while True:
            data = ser.read(ser.in_waiting).decode('gbk')
            if data != '':
                break
        print("已接受到数据:", data)
    except Exception as exc:
        print("读取异常", exc)


# 关闭串口
def close_ser():
    try:
        ser.close()
        if ser.isOpen():
            print("串口未关闭")
        else:
            print("串口已关闭")
    except Exception as exc:
        print("串口关闭异常", exc)


if __name__ == '__main__':
    i=0
    open_ser()  # 打开串口
    send_msg()  # 写数据
    while(i<20):
        print(read_msg())
        i++
    close_ser()  # 关闭串口
