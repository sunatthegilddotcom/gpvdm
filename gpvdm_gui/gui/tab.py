# 
#   General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#   model for 1st, 2nd and 3rd generation solar cells.
#   Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
#
#   https://www.gpvdm.com
#   Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License v2.0, as published by
#   the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program; if not, write to the Free Software Foundation, Inc.,
#   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# 

## @package tab
#  The main tab class, used to display material properties.
#

import os

from token_lib import tokens
from undo import undo_list_class
from tab_base import tab_base
from str2bool import str2bool
from util import latex_to_html
from help import help_window

from PyQt5.QtCore import pyqtSignal

from PyQt5.QtWidgets import QTextEdit,QWidget, QScrollArea,QVBoxLayout,QLabel,QHBoxLayout,QPushButton, QSizePolicy, QTableWidget, QTableWidgetItem,QComboBox,QGridLayout,QLineEdit
from PyQt5.QtCore import QSize, Qt
from PyQt5.QtGui import QPixmap, QIcon

from icon_lib import icon_get

from PyQt5.QtCore import QTimer

import i18n
_ = i18n.language.gettext

import functools
from error_dlg import error_dlg

from file_watch import get_watch
from inp_viewer import inp_viewer
from inp import inp

class tab_class(QWidget,tab_base):

	changed = pyqtSignal()

	def __init__(self,file_name):
		QWidget.__init__(self)
		self.editable=True
		self.icon_file=""
		self.widget_list=[]
		self.scroll=QScrollArea()
		self.main_box_widget=QWidget()
		self.vbox=QVBoxLayout()
		self.hbox=QHBoxLayout()
		self.hbox.setAlignment(Qt.AlignTop)
		self.file_name=file_name

		self.tab=inp_viewer()
		self.tab.file_name=file_name
		self.vbox.addWidget(self.tab)

		self.f=inp()
		self.f.load(self.file_name)

		#if self.lines==False:
		#	error_dlg(self,_("File not found.")+" "+filename)
		#	return
		self.tab.populate(self.f.lines)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.vbox.addWidget(spacer)
		self.main_box_widget.setLayout(self.vbox)
		self.scroll.setWidgetResizable(True)
		self.scroll.setWidget(self.main_box_widget)

		self.icon_widget=QWidget()
		self.icon_widget_vbox=QVBoxLayout()
		self.icon_widget.setLayout(self.icon_widget_vbox)
		
		if self.icon_file!="":
			self.image=QLabel()
			icon=icon_get(self.icon_file)
			self.image.setPixmap(icon.pixmap(icon.actualSize(QSize(32, 32))))
			self.icon_widget_vbox.addWidget(self.image)

			spacer2 = QWidget()
			spacer2.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Expanding)
			self.icon_widget_vbox.addWidget(spacer2)
		
			self.hbox.addWidget(self.icon_widget)

		self.hbox.addWidget(self.scroll)
		
		self.setLayout(self.hbox)

		self.tab.changed.connect(self.callback_edit)

		get_watch().add_call_back(self.file_name,self.update)

	def update():
		self.f.load(self.filename)
		self.tab.update_lines(self,self.f.lines)

	def callback_edit(self):
		self.f.lines=self.tab.f.lines
		self.f.save()
		self.changed.emit()

	def help(self):
		help_window().get_help(self.file_name)

	def set_edit(self,editable):
		self.tab.editable=editable



