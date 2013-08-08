#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from PIL import Image, ImageTk
import time
import Tkinter as tk
import ttk

class player_display(tk.Frame):
    def __init__(self,parent):
        tk.Frame.__init__(self, parent)
        self.parent = parent
        self.path = parent.video_path
        self.rep_fpointer = 0
        self.stats_on_yuv = False
        self.rep_path = parent.report_path
        self.width = parent.disp_w
        self.height = parent.disp_h
        self.label_image = ttk.Label(self)  
        self.initialize()

    def initialize(self):
        self.grid(row=0, column=0, sticky='w')

    def play_frame(self):
        self.path = self.parent.video_path
        self.rep_path = self.parent.report_path
        self.width = self.parent.disp_w
        self.height = self.parent.disp_h
        
        ycbcr = self.getYCbCr()
        
        yuv = ''.join([ycbcr[0][i] + ycbcr[1][i] + ycbcr[2][i] for i in range(0, len(ycbcr[0]))])
            
        self.frame = Image.fromstring('YCbCr',(self.width,self.height),yuv)
        if self.width > 1280 or self.height > 720:
            print self.width
            self.frame = self.frame.resize((1280, 720), Image.ANTIALIAS)
            
        self.label_image.destroy()
        self.tkpi = ImageTk.PhotoImage(self.frame)
        self.label_image = ttk.Label(self, image=self.tkpi)

        self.label_image.pack()
        self.update()
            
    def upscale(self,layer):
        chroma_rows = [layer[i:(i+self.width)] for i in range(0,len(layer),self.width)]
        output = ''
        for row in chroma_rows:
            new_row = ''.join([c+c for c in row])
            output += new_row*2
        return output
            
    def getYCbCr(self):
        video = open(self.path, 'rb')
        video_res = self.width*self.height
        video.seek(self.parent.curr_frame*(video_res + video_res/2))
        yuv = (())
        
        y = video.read(video_res)
        cb = self.upscale(video.read(video_res/4))
        cr = self.upscale(video.read(video_res/4))
        
        video.close()
                
        if self.stats_on_yuv:
            return self.plot_stats_yuv([y,cb,cr])
        else:
            return [y,cb,cr]
        
    def plot_stats_yuv(self, ycbcr):
        color_map = {'SKIP': [248,72], 'INTER': [8,248], 'INTRA': [8,8]}
        rep_file = self.open_rep_file(self.rep_path)
        
        y,u,v = [list(l) for l in ycbcr]
        
        while rep_file:
            line = rep_file.readline()
            if len(line) == 0:
                break
                
            line = line.replace(' ', '').strip('\n').split(';')
            if 'EOP' in line:
                rep_file.close()
                break
            elif len(line) > 3:
                pu_x = int(line[0])
                pu_y = int(line[1])
                pu_w = int(line[2])
                pu_h = int(line[3])
                
                pu_mode = line[4]
                
                # write borders
                for h in range(0,pu_h):
                    y[(pu_x + pu_w - 1) + (pu_y + h-1)*self.width] = chr(0)
                for w in range(0,pu_w):
                    y[(pu_x + w) + (pu_y + pu_h-1)*self.width] = chr(0)
                # write chroma components
                for h in range(0, pu_h):
                    for w in range(0, pu_w):
                        u[pu_x + w-1 + (pu_y + h-1)*self.width] = chr(color_map[pu_mode][0])
                        v[pu_x + w-1 + (pu_y + h-1)*self.width] = chr(color_map[pu_mode][1])
                        
        return [y,u,v]
    
    def open_rep_file(self,path):
        rep_file = open(str(path), 'r')
        prev_line = 0
        
        while rep_file:
            line = rep_file.readline()
            if len(line) == 0:
                break
            line = line.replace(' ', '').strip('\n').split(';')
            
            if 'POC' in line and int(line[1]) == self.parent.curr_frame:
                rep_file.seek(prev_line)
                return rep_file
            prev_line = rep_file.tell()
        return None
