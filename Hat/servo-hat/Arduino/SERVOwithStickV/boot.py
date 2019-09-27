import image
import lcd
import sensor
import sys
import time
import KPU as kpu
from fpioa_manager import fm
from machine import UART

fm.register(board_info.CONNEXT_A,fm.fpioa.UART1_RX)
fm.register(board_info.CONNEXT_B,fm.fpioa.UART1_TX)

uart_A = UART(UART.UART1, 115200)

lcd.init()
lcd.rotation(2)

try:
    img = image.Image("/sd/startup.jpg")
    lcd.display(img)
except:
    lcd.draw_string(lcd.width()//2-100,lcd.height()//2-4, "Error: Cannot find start.jpg", lcd.WHITE, lcd.RED)


task = kpu.load("/sd/999723f600dad044_mbnet10_quant.kmodel")

labels=["M5Stack Fire","M5StickC","M5Stick"] #You can check the numbers here to real names.

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224, 224))
sensor.run(1)

lcd.clear()


while(True):
    img = sensor.snapshot()
    fmap = kpu.forward(task, img)
    plist=fmap[:]
    pmax=max(plist)
    max_index=plist.index(pmax)
    a = lcd.display(img)
    uart_A.write('clear')
    uart_A.write('\r')
    if pmax > 0.95:
        lcd.draw_string(5, 60, "Accu:%.2f Type:%s"%(pmax, labels[max_index].strip()))     
        uart_A.write(labels[max_index])
        uart_A.write('\r')

a = kpu.deinit(task)
