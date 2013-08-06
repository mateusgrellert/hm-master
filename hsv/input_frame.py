#!/usr/bin/python
# -*- coding: iso-8859-1 -*-


import Tkinter as tk
import tkFileDialog as file_browser
import ttk

class input_frame(tk.Frame):
    def __init__(self,parent):
        tk.Frame.__init__(self, parent)   
        self.parent = parent
        self.input_video = None
        self.input_report = None
        self.stats_on_yuv = False
        self.initialize()

    def initialize(self):
        self.pack(fill=tk.BOTH)
        self.video_path = tk.StringVar()
        video_entry = ttk.Entry(self,textvariable=self.video_path)
		
        self.report_path = tk.StringVar()
        report_entry = ttk.Entry(self,textvariable=self.report_path)        
		
        video_button = ttk.Button(self,text=u"Browse",command=self.video_click)
        report_button = ttk.Button(self,text=u"Browse",command=self.report_click)
	
        video_str = tk.StringVar(value=u"Input Video CFG")
        video_label = tk.Label(self,textvariable=video_str,anchor="w")
		
        report_str = tk.StringVar(value=u"Input Stats")
        report_label = tk.Label(self,textvariable=report_str,anchor="w")
		
        video_label.grid(row=0,column=0, padx=1, sticky='W')
        video_entry.grid(row=0,column=1, padx=1, sticky='W')
        video_button.grid(row=0,column=2, padx=1,sticky='W')
		
        report_label.grid(row=1,column=0, padx=1, sticky='W')
        report_entry.grid(row=1,column=1, padx=1, sticky='W')
        report_button.grid(row=1,column=2, padx=1, sticky='W')
		
        load_button = ttk.Button(self,text=u"Load",command=self.parent.init_player)
        load_button.grid(row=2,column=1, padx=1,sticky='W')
        
        
        self.update()
	
    def video_click(self):
		self.input_video = file_browser.askopenfilename(filetypes=[('CFG File','.cfg')],title='Select the File')
		self.video_path.set(self.input_video)
    
    def report_click(self):
		self.input_report = file_browser.askopenfilename(filetypes=[('HSV File','.hsv')],title='Select the File')
		self.report_path.set(self.input_report)

