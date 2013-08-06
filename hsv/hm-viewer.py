#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from PIL import Image, ImageTk
from input_frame import *
from yuv_player import *

import Tkinter as tk
import tkMessageBox
import tkFileDialog as file_browser
import ttk

class main_window(tk.Tk):
    def __init__(self,parent):
        tk.Tk.__init__(self,parent)
        self.parent = parent
        self.player = 0
        self.initialize()
        
    def initialize(self):
        self.inp_data = input_frame(self)
        self.player = yuv_player(self)
        #self.resizable(False,False)
    
    def init_player(self):
        if not self.inp_data.input_video:
            tkMessageBox.showinfo("Error!", "First set an input cfg file")
            return
            
        if self.player.running:
            self.player.reset()
        else:
            self.player.init_player()
        self.update()



if __name__ == "__main__":

	app = main_window(None)
	app.title('HST - HEVC Statistics Viewer')
	app.mainloop()
