# -*- coding: utf-8 -*-
import wx
import wx.grid
import serial
import json
import hashlib
import time
import md5
import qrcode
import Image
import ctypes
import sys
import shutil
import os
import threading
#from reportlab.pdfgen import canvas
#from reportlab.lib.pagesizes import A4, landscape
#from datetime import datetime

class printUI(wx.Frame):
    def __init__(self,title,pos,size):
        wx.Frame.__init__(self, None, -1, title, pos, size,style=wx.MINIMIZE_BOX | wx.SYSTEM_MENU | wx.CAPTION | wx.CLIP_CHILDREN | wx.CLOSE_BOX)
        self.icon = wx.Icon('itead.ico', wx.BITMAP_TYPE_ICO)
        self.SetIcon(self.icon)
        self.Center()
        self.panel = wx.Panel(self, -1)
        self.panel.SetBackgroundColour("white")
        self.panel.SetForegroundColour("brown")

        self.Arr_7766 = []
        self.Arr_6530 = []

        self.load_file_com = [
              "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9","COM10",
              "COM11","COM12","COM13","COM14","COM15","COM16","COM17","COM18","COM19","COM20",
              "COM21","COM22","COM23","COM24","COM25","COM26","COM27","COM28","COM29","COM30",
              "COM31", "COM32", "COM33","COM34", "COM35", "COM36", "COM37", "COM38", "COM39", "COM40",
            "COM41", "COM42", "COM43", "COM44", "COM45", "COM46", "COM47", "COM48", "COM49", "COM50",]

        self.uart_open_flag = 0

        self.print_data_list = []
        self.qrcode_num = 0

        self.serial_UI =self.Serial_UI()


    def Pbutton_UI(self):
        self.print_to_printer_button = wx.Button(self.panel, label=u'打印机打印(无)', pos=wx.Point(6, 250), size=wx.Size(380, 50), style=0)
        self.Bind(wx.EVT_BUTTON, self.printer_print, self.print_to_printer_button)

        self.print_to_pdf_button = wx.Button(self.panel, label=u'PDF打印', pos=wx.Point(6, 310),size=wx.Size(380, 50), style=0)
        self.Bind(wx.EVT_BUTTON, self.pdf_print, self.print_to_pdf_button)

    def Qrcode_UI(self):
        font = wx.Font(13, wx.SWISS, wx.NORMAL, wx.BOLD)

        self.qr_cnt = wx.StaticText(self.panel,-1,u"已扫二维码个数：",pos=wx.Point(10, 70))
        self.qr_num = wx.StaticText(self.panel, -1, str(self.qrcode_num), pos=wx.Point(108, 70))
        self.qr_unit = wx.StaticText(self.panel, -1, u"个", pos=wx.Point(140, 70))

        self.qr_cnt = wx.StaticText(self.panel, -1, u"当前扫描二维码：", pos=wx.Point(10, 100))

        self.bmp = wx.Image("qrcode.bmp", wx.BITMAP_TYPE_BMP).ConvertToBitmap()
        self.bmpbutton = wx.BitmapButton(self.panel, -1, self.bmp, pos=wx.Point(10, 128))
        self.Bind(wx.EVT_BUTTON, self.OnClick, self.bmpbutton)
        self.bmpbutton.SetDefault()


    def Serial_UI(self):
        font = wx.Font(13,wx.SWISS, wx.NORMAL, wx.BOLD)

        self.serial_com = wx.StaticText(self.panel, -1, u'STM32串口：', pos=wx.Point(10, 20))
        self.serial_com_choise = wx.Choice(self.panel, choices=self.load_file_com, pos=wx.Point(90, 16),size=wx.Size(70, 87), style=0)
        self.serial_com_choise.SetSelection(0)

        self.ac6530_com = wx.StaticText(self.panel, -1, u'6530串口：', pos=wx.Point(165, 20))
        self.ac6530_com_choise = wx.Choice(self.panel, choices=self.load_file_com, pos=wx.Point(230, 16),size=wx.Size(70, 87), style=0)
        self.ac6530_com_choise.SetSelection(0)

        self.open_button = wx.Button(self.panel, label=u'打开串口', pos=wx.Point(310, 14), size=wx.Size(80, 28), style=0)
        self.Bind(wx.EVT_BUTTON, self.open_uart, self.open_button)

        #self.clear_button = wx.Button(self.panel, label=u'清空计数', pos=wx.Point(218, 14), size=wx.Size(170, 28), style=0)
        #self.Bind(wx.EVT_BUTTON, self.clear_log, self.clear_button)

    #def bmp_to_pdf(self):

    def OnClick(self,event):
        print

    def clear_log(self,event):
        self.qrcode_num = 0
        self.qr_num.SetLabel(str(self.qrcode_num))
        self.print_data_list = []

    def printer_print(self,event):
        print "None"
        #try:
        #    for i in xrange(len(self.print_data_list)):
        #        self.create_code(self.print_data_list[i])
        #        self.print_to_printer()
        #except:
        #    raise Exception("printer no ready")

    def bmp_to_pdf(self,data,x,y):
        code_size_x = 45  #52
        code_size_y = 45  #52


        self.create_code(data)
        self.f_pdf.setFont("Courier",7)
        self.f_pdf.drawImage('.\\picture\\'+data[48:]+'.bmp', 40+77*y, self.A4_y-(90*(x+1)), code_size_x, code_size_y)
        #self.f_pdf.drawString(44+77*y,self.A4_y-40-(90*(x)), str(id))

    def change_qccode(self):
        self.bmp = wx.Image("qrcode.bmp", wx.BITMAP_TYPE_BMP).ConvertToBitmap()
        self.bmpbutton.SetBitmap(self.bmp)
        self.panel.Refresh()

    def open_uart(self,event):
        try:
            if self.uart_open_flag == 0:
                try:
                    self.ser = serial.Serial(self.load_file_com[self.serial_com_choise.GetSelection()],9600,bytesize=8, parity='N', stopbits=1, timeout=0, writeTimeout=0, xonxoff=0,rtscts=0)
                    self.ac6530_ser = serial.Serial(self.load_file_com[self.ac6530_com_choise.GetSelection()], 9600,bytesize=8, parity='N', stopbits=1, timeout=0, writeTimeout=0,xonxoff=0, rtscts=1)
                    #if (self.ser > 0 and self.ac6530_ser>0):
                    if self.ser > 0:
                        self.open_button.SetLabel(u"关闭串口")
                        self.uart_open_flag = 1
                        print u"串口打开成功！"
                except:
                    wx.MessageBox(u"串口打开失败！", 'Confirm', wx.OK)
                    print u"串口打开失败！"
            else:
                self.ser.close()
                self.ac6530_ser.close()
                print u"串口已关闭！"
                self.open_button.SetLabel(u"打开串口")
                self.uart_open_flag = 0
        except:
            print "uart open error"

    def create_code(self,data):
        img = qrcode.make(data)
        (x,y) = img.size
        x_s = 100
        y_s = y*x_s/x
        out = img.resize((x_s, y_s), Image.ANTIALIAS)  # resize image with high-quality
        out.save('qrcode.bmp')

        img2 = qrcode.make(data)
        (x, y) = img2.size
        x_s = 130
        y_s = y * x_s / x
        out = img2.resize((x_s, y_s), Image.ANTIALIAS)  # resize image with high-quality
        out.save('.\\picture\\'+data[48:]+'.bmp')

        img3 = qrcode.make(data)
        (x,y) = img.size
        x_s = 230
        y_s = y*x_s/x
        out = img3.resize((x_s, y_s), Image.ANTIALIAS)  # resize image with high-quality
        out.save('print.bmp')

    def uart_read(self):
        data = ''
        timeout = 0
        rece_len = 0

        while True:
            if self.uart_open_flag:
                n = self.ser.inWaiting()
                if n>0:
                    data += self.ser.read(n)
                    rece_len += n
                else:
                    timeout = timeout + 1
                    time.sleep(0.001)
                    if (timeout > 20):
                        break

        return data

    def ac6530_uart_read(self):
        ac6530_data = ''
        timeout = 0
        ac6530_rece_len = 0

        while True:
            if self.uart_open_flag:
                n = self.ac6530_ser.inWaiting()
                if n>0:
                    ac6530_data += self.ac6530_ser.read(n)
                    ac6530_rece_len += n
                else:
                    timeout = timeout + 1
                    time.sleep(0.001)
                    if timeout > 20:
                        break

        return ac6530_data

    def write_data_to_file(self,data):
        s = data+'\r\n'

        fp = open("data.txt","a+")
        fp.write(s)
        fp.close()

    def data_parsing(self,data):
        if len(data)>0:
            print "recv stm32 data:"
            print data

        if data == 'i':
        #if data.find("FETCH:CURRent:AC?\r\n") != -1:
            print "come i"
            self.ac6530_ser.flushOutput()
            s = "MEASure:CURRent:AC?\r\n"
            self.ac6530_ser.write(s)
            print "write i"
        if data == 'v':
        #elif data.find("FETCH:VOLTage:AC?\r\n") != -1:
            print "come v"
            #self.ac6530_ser.flushOutput()
            s = "MEASure:VOLTage:AC?\r\n"
            self.ac6530_ser.write(s)
           # self.ac6530_ser.flush()
            print "write v"
        if data == 'p':
        #elif data.find("FETCH:POWer:AC?\r\n") != -1:
            print "come p"
            #self.ac6530_ser.flushOutput()
            s = "MEASure:POWer:AC?\r\n"
            self.ac6530_ser.write(s)
          #  self.ac6530_ser.flush()
            print "write p"
        if data[0] >= '0' and data[0] <= '9':
            print "data write file\r\n"
            self.write_data_to_file(data)
            #self.Arr_7766.append(data)
        print 'out\r\n'

    def print_to_printer(self):
        try:
            tscdll = ctypes.WinDLL('TSCLib.dll')
            tscdll.openport("Gprinter  GP-9025T")

            tscdll.setup('30.0', '40.0', '4', '14', '0', '2', '0')#纸宽度、高度、打印速度、打印浓度、传感器类别0、偏移距离
            tscdll.clearbuffer()

            tscdll.sendcommand("DIRECTION 1")

            tscdll.downloadpcx("print.bmp", "PR.BMP")
            tscdll.sendcommand("PUTBMP 10,95,\"PR.BMP\"")#1mm=8dots,x起点,y起点
            tscdll.printlabel("1", "1")
            tscdll.clearbuffer()
            tscdll.closeport()
        except:
            wx.MessageBox(u"打印机无响应！", 'Confirm', wx.OK)
            #print sys.exc_info()[0], sys.exc_info()[1]
            raise Exception("error")

    '''
    def print_to_printer(self):
        try:
            tscdll = ctypes.WinDLL('TSCLib.dll')
            tscdll.openport("Gprinter  GP-3120TU")

            tscdll.setup('19.0', '19.0', '4', '8', '0', '2', '0')
            tscdll.clearbuffer()

            tscdll.sendcommand("DIRECTION 1")

            tscdll.downloadpcx("print.bmp", "PR.BMP")
            tscdll.sendcommand("PUTBMP 2,2,\"PR.BMP\"")
            tscdll.printlabel("1", "1")
            tscdll.clearbuffer()
            tscdll.closeport()
        except:
            wx.MessageBox(u"打印机无响应！", 'Confirm', wx.OK)
            #print sys.exc_info()[0], sys.exc_info()[1]
            raise Exception("error")
    '''

    def recv_thread_handle(self):
            while True:
                try:
                    data = self.uart_read()
                    self.data_parsing(data)
                    ac6530_data = self.ac6530_uart_read()
                    if(len(ac6530_data)>0):
                        print "ac6530 return data:"
                        print ac6530_data
                        self.ser.write(ac6530_data)
                        print "write to stm32"

                    data = ''
                    ac6530_data = ''
                except:
                    pass


if __name__ == '__main__':
    app = wx.App()
    frame = printUI(u"READ6530",(100,150),(400,400))
    frame.Show(True)

    recv_thread = threading.Thread(target=frame.recv_thread_handle)
    recv_thread.setDaemon(True)
    recv_thread.start()
    app.MainLoop()

    
