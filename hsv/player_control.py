#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from PIL import Image, ImageTk

import Tkinter as tk
import ttk

class player_control(tk.Frame):
    def __init__(self,parent):
        tk.Frame.__init__(self, parent)
        self.parent = parent
        self.stats_on_yuv = False
        self.button_w, self.button_h = parent.button_w, parent.button_h
        self.initialize()
    
    def initialize(self):
        self.grid(row=1, column=0, sticky = 'w')
        
        play_im = (Image.open('./pics/play.png')).resize((self.button_w,self.button_h))
        self.play_tk = ImageTk.PhotoImage(play_im)
        self.play_button = ttk.Button(self,image=self.play_tk,command=self.parent.play_video)
        
        pause_im = (Image.open('./pics/pause.png')).resize((self.button_w,self.button_h))
        self.pause_tk = ImageTk.PhotoImage(pause_im)
        self.pause_button = ttk.Button(self,image=self.pause_tk,command=self.parent.pause_video)
        
        step_im = (Image.open('./pics/step.png')).resize((self.button_w,self.button_h))
        self.step_tk = ImageTk.PhotoImage(step_im)
        self.step_button = ttk.Button(self,image=self.step_tk,command=self.parent.play_frame)
        
        step_b_im = (Image.open('./pics/step_back.png')).resize((self.button_w,self.button_h))
        self.step_b_tk = ImageTk.PhotoImage(step_b_im)
        self.step_b_button = ttk.Button(self,image=self.step_b_tk,command=self.parent.rewind)
        
        self.bind("<Right>", self.parent.play_frame)
        self.focus_set()
        self.play_button.pack(side='left')
        self.pause_button.pack(side='left')
        self.step_button.pack(side='left')
        self.step_b_button.pack(side='left')
        
        stats_box = ttk.Checkbutton(self, text="Plot Stats on YUV", command=self.parent.plot_and_play)
        stats_box.pack(side='left')
        
        self.update()
