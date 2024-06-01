import re
import time

import serial


def get_time():
    Time = time.strftime('%Y%m%d%H%M%S', time.localtime())
    return Time


# 串口打开函数
def open_ser():
    try:
        global ser
        ser = serial.Serial('COM10', 9600, 8, 'N', 1)  # 'COM5', 9600, bytesize=8, parity='N', stopbits=1
        if (ser.isOpen() == True):
            print("串口打开成功")
    except Exception as exc:
        print("串口打开异常", exc)


def get_timemsg():
    cutime = get_time()
    Time_list = re.findall(".{2}", cutime)
    print(Time_list)
    return Time_list


def send_timemsg():
    while True:
        count = ser.inWaiting()  # 获取串口缓冲区数据
        # 数据的接收
        if count > 0:  # 有数据来喽，该发时间了
            Time_list = get_timemsg()
            for i in Time_list:
                d = bytes.fromhex(i)

                ser.write(d)
                time.sleep(0.144)
            return 0


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
    open_ser()
    send_timemsg()
    close_ser()
