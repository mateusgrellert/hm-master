#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import Tkinter as tk
import ttk

class stats_frame(tk.Frame):
    def __init__(self,parent):
        tk.Frame.__init__(self, parent)
        self.parent = parent
        self.width = parent.report_w
        self.height = parent.report_h
        self.fpointer = 0
        self.report_label = ttk.Label(self)  

        self.initialize()


    def initialize(self):
        self.config(bg='white',bd=1,relief='sunken')
        self.grid(row=0, column=1, sticky='nw', padx=5)

    def display_report(self):
        if not self.parent.disp_stats: return
        self.path = self.parent.report_path
        self.parse_report()
        self.report_str_tk = tk.StringVar(value=self.report_str)
        self.report_label.destroy()
        self.report_label = ttk.Label(self,textvariable=self.report_str_tk, font=('Helvetica',11))
        self.report_label.pack()
        self.update()

    def parse_report(self):
        rep_file = self.open_rep_file(self.path)

        acum_size = {}
        acum_mode = {}
        self.report_str = ''
        total_pixels = 0
        
        while rep_file:
            line = rep_file.readline()
            if len(line) == 0:
                break
            line = line.replace(' ', '').strip('\n').split(';')
            lcu_size = 64*64*1.0

            if 'POC' in line:
                self.report_str += ' POC : ' + line[1] + '\n'*2
            elif line != ['']:
                if 'EOP' in line:
                    rep_file.close()
                    break
                
                if (line[2]+'x'+line[3]) not in acum_size:
                    acum_size[line[2]+'x'+line[3]] = (int(line[2])* int(line[3]))
                else:
                    acum_size[line[2]+'x'+line[3]] += (int(line[2])* int(line[3]))

                if (line[4]) not in acum_mode:
                    acum_mode[line[4]] = (int(line[2])* int(line[3]))
                else:
                    acum_mode[line[4]] += (int(line[2])* int(line[3]))
                
                total_pixels += (int(line[2])* int(line[3]))
            else:
                return

        for key in  sorted(acum_mode):
            self.report_str += ' ' + key + ' : ' + str('%.2f' % (acum_mode[key]*100.0/total_pixels)) + ' %  \n'
        self.report_str += '\n'
        for key in sorted(acum_size, reverse=True):
            self.report_str += ' ' + key + ' : ' + str('%.2f' % (acum_size[key]*100.0/total_pixels)) + ' %  \n'

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
