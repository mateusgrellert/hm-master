#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from PIL import Image, ImageTk
from player_display import *
from player_control import *
from stats_frame import *

import Tkinter as tk
import ttk


class yuv_player(tk.Frame):
    def __init__(self,parent):
        tk.Frame.__init__(self, parent)
        self.parent = parent
        self.running = False
        self.paused = False
        self.curr_frame = 0
        self.button_w, self.button_h = 30,30
        self.report_w, self.report_h = 130, 400
        self.initialize()
    
    def initialize(self):
        self.pack(fill=tk.BOTH, expand=1)
        self.update()

    def init_player(self):
        self.running = True
        
        self.cfg_path = self.parent.inp_data.input_video
        self.report_path = self.parent.inp_data.input_report
        self.stats_on_yuv = self.parent.inp_data.stats_on_yuv
        self.disp_stats = (self.parent.inp_data.input_report != None)

        self.parse_cfg()
        
        self.total_w = self.disp_w + self.report_w + 10
        self.total_h = self.disp_h + self.button_h + 10
        
        self.config(width=self.total_w,height=self.total_h)
        
        self.display = player_display(self)
        self.stats = stats_frame(self)

        self.controller = player_control(self)

        self.display.play_frame()

        self.stats.display_report()

        self.update()
        
    def play_frame(self, key=0):
        self.paused = True
        self.curr_frame += 1
        if self.curr_frame >= 0:
            self.display.play_frame()
            self.stats.display_report()
            self.update()
        else:
            self.curr_frame = 0
        
    def play_video(self):
        if self.paused:
            self.paused = False
        while not(self.paused) and self.display.curr_frame < self.numframes:
            self.display.play_frame()
            self.stats.display_report()
            self.update()
            
    def pause_video(self):
        self.paused = not(self.paused)
        if not(self.paused):
            self.play_video()
        
    def plot_and_play(self):
        if not(self.display.stats_on_yuv) and self.disp_stats:
            self.display.stats_on_yuv = True
        else:
            self.display.stats_on_yuv = False
        self.curr_frame -= 1
        self.play_frame()
            
    def rewind(self):
        self.curr_frame -= 2
        self.play_frame()
            
    def parse_cfg(self):
        cfg_file = open(str(self.cfg_path), 'r')
        cfg_lines = cfg_file.readlines()
        cfg_file.close()
        
        for line in cfg_lines:
			line = line.split()
			if 'InputFile' in line[0]:
				self.video_path = line[2].strip()
			if 'FrameSkip' in line[0]:
				self.fskip = int(line[2])
			if 'SourceWidth' in line[0]:
				self.disp_w = int(line[2])
			if 'SourceHeight' in line[0]:
				self.disp_h = int(line[2])
			if 'FramesToBeEncoded' in line[0]:
				self.numframes = int(line[2])
                
    def reset(self):
        self.cfg_path = self.parent.inp_data.input_video
        self.report_path = self.parent.inp_data.input_report
        self.stats_on_yuv = self.parent.inp_data.stats_on_yuv
        self.disp_stats = (self.parent.inp_data.input_report != None)
        self.parse_cfg()
        self.config(width=self.total_w,height=self.total_h)
        self.curr_frame = -1
        self.play_frame()
				
