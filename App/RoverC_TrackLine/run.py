import struct
from pid import PID
import sensor, image, time, math
import utime
from machine import UART
from Maix import GPIO
from fpioa_manager import *
from modules import ws2812

rho_pid = PID(p=-0.4, i=0) #0.05
theta_pid = PID(p=0.30, i=0) #0.25

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

fm.register(34,fm.fpioa.UART1_TX)
fm.register(35,fm.fpioa.UART1_RX)
uart_out = UART(UART.UART1, 115200, 8, None, 1, timeout=1000, read_buf_len=4096)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
clock = time.clock()


base_cx = [40 for i in range(30, 60, 5)]
reimg = image.Image(size=(80,30)).to_grayscale()
crosswalk = False
while(True):
    a = reimg.clear()
    ex = 0
    clock.tick()
    a = img = sensor.snapshot().mean_pool(4,4) #80*60
    a = img.binary([(70, 100, -40, 15, -5, 50)], invert = True)
    #a = img.binary([(70, 100, -40, 20, -20, 50)], invert = True)
    a = img.draw_rectangle(0, 0, 80, 30, color = WHITE, fill = True)
    a = img.close(1)

    i = 0
    weight = 1
    cx_hexlist = []
    rx_hexlist = []
    lx_hexlist = []
    #len_list = []

    for y in range(30, 60, 5):
        flag = False
        for rx in range(base_cx[i], 80, 2):
            if img.get_pixel(rx, y) == BLACK:
                flag = True
                rx_hexlist.append(rx)
                break
        if flag == False:
            rx_hexlist.append(80)
        flag = False
        for lx in range(base_cx[i], 0, -2):
            if img.get_pixel(lx, y) == BLACK:
                flag = True
                lx_hexlist.append(lx)
                break
        if flag == False:
            lx_hexlist.append(0)

        if rx >= 75:
            rx = lx + 60

        base_cx[i] = (rx + lx) >> 1
        cx_hexlist.append(base_cx[i])
        #len_list.append(rx_hexlist[i] - lx_hexlist[i])
        #a = img.draw_circle(base_cx[i], y, 2, (255, 0, 0), fill = True)
        a = reimg.draw_circle(base_cx[i], y-30, 2, (255, 255, 255), fill = True)

    #min_len = 999
    #for m in len_list:
        #if m < min_len:
            #min_len = m

    #if min_len < 20:
        #if crosswalk == False:
            #crosswalk = True
        #else:
            #crosswalk = False
    #else:
        #crosswalk = False

    #if crosswalk == True:
        #if uart_out.read(4096):
            #crosswalk = False
            #rho_u = 999.999
            #theta_u = 999.999
            #int_rho_u = struct.unpack('<I', struct.pack('<f', rho_u))[0]
            #int_theta_u = struct.unpack('<I', struct.pack('<f', theta_u))[0]
            #rho_u_hexlist = [int_rho_u & 0xFF, (int_rho_u >> 8) & 0xFF, (int_rho_u >> 16) & 0xFF, (int_rho_u >> 24) & 0xFF]
            #theta_u_hexlist = [int_theta_u & 0xFF, (int_theta_u >> 8) & 0xFF, (int_theta_u >> 16) & 0xFF, (int_theta_u >> 24) & 0xFF]
            #a = uart_out.write(bytes(rho_u_hexlist))
            #a += uart_out.write(bytes(theta_u_hexlist))
            #a += uart_out.write(bytes(rx_hexlist))
            #a += uart_out.write(bytes(lx_hexlist))
            #a += uart_out.write(bytes(cx_hexlist))
            #print('send %d' %a)

    #a = img.draw_image(reimg, 0, 0)
    reline = reimg.get_regression([(50, 255)], x_stride = 1)
    if reline:
        #a = img.draw_line(reline.line(), color = (0, 255, 0), thickness = 4)
        #a = img.draw_line(40, 15, reline[0] + abs(reline[0] - reline[2]), 15, color = (255, 0, 0), thickness = 2)

        rho_err = abs(reline.rho())-40
        if reline.theta()>90:
            theta_err = reline.theta()-180
        else:
            theta_err = reline.theta()

        rho_u = rho_pid.get_pid(rho_err, 1)
        theta_u = theta_pid.get_pid(theta_err, 1)

        int_rho_u = struct.unpack('<I', struct.pack('<f', rho_u))[0]
        int_theta_u = struct.unpack('<I', struct.pack('<f', theta_u))[0]
        rho_u_hexlist = [int_rho_u & 0xFF, (int_rho_u >> 8) & 0xFF, (int_rho_u >> 16) & 0xFF, (int_rho_u >> 24) & 0xFF]
        theta_u_hexlist = [int_theta_u & 0xFF, (int_theta_u >> 8) & 0xFF, (int_theta_u >> 16) & 0xFF, (int_theta_u >> 24) & 0xFF]

        line_hexlist = [reline[0], reline[1], reline[2], reline[3]]

        if uart_out.read(4096):
            a = uart_out.write(bytes(rho_u_hexlist))
            a += uart_out.write(bytes(theta_u_hexlist))
            a += uart_out.write(bytes(rx_hexlist))
            a += uart_out.write(bytes(lx_hexlist))
            a += uart_out.write(bytes(cx_hexlist))
            print('send %d' %a)

        #ux = int(rho_u)
        #uw = int(theta_u)

        #if abs(uw) > 2:
            #ux = 0

        #print(ux, uw)

        #out = int(rho_u + theta_u)
        #l = 15 - out
        #r = 15 + out
        #print('out: %d, L: %d, R: %d, rho: %.4f, theta: %.4f' %(out, l, r, rho_u, theta_u))

        #print('theta: %d, theta_err: %d, theta_u: %.4f, rho: %d, rho_err: %d, rho_u: %.4f' %(reline.theta(), theta_err, theta_u, abs(reline.rho()), rho_err, rho_u))

