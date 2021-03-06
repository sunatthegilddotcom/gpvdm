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

## @package gui_util
#  GUI utilities.
#


from cal_path import get_image_file_path
import os

#qt
from gui_enable import gui_get

if gui_get()==True:
	from PyQt5.QtWidgets import QTextEdit, QAction,QTableWidgetItem,QComboBox, QMessageBox, QDialog, QDialogButtonBox
	from PyQt5.QtWidgets import QListWidgetItem,QListView,QLineEdit,QWidget,QHBoxLayout,QPushButton
	from PyQt5.uic import loadUi
	from PyQt5.QtGui import QPixmap
	from PyQt5.QtCore import QSize, Qt, QTimer
	from QComboBoxLang import QComboBoxLang
	from PyQt5.QtGui import QIcon
	from gtkswitch import gtkswitch
	from gpvdm_select_material import gpvdm_select_material
	from gpvdm_select_emission import gpvdm_select_emission
	from leftright import leftright
	from gpvdm_select import gpvdm_select
	from QComboBoxLang import QComboBoxLang
	from QColorPicker import QColorPicker
	from QComboBoxNewtonSelect import QComboBoxNewtonSelect
	from QComboBoxShape import QComboBoxShape
	from QChangeLog import QChangeLog
	from QParasitic import QParasitic
	from generic_switch import generic_switch
	from shape_dos_switch import shape_dos_switch
	from mobility_widget import mobility_widget

#windows
from cal_path import get_ui_path

from icon_lib import icon_get


from str2bool import str2bool

class dlg_get_text():
	def __init__(self,text,default,image):
		#QDialog.__init__(self)
		self.ui = loadUi(os.path.join(get_ui_path(),"question.ui"))
		self.ui.label.setText(text)
		self.ui.text.setText(default)
		#pixmap = QPixmap(os.path.join(get_image_file_path(),image))
		icon=icon_get(image)
		self.ui.setWindowIcon(icon)
		self.ui.image.setPixmap(icon.pixmap(icon.actualSize(QSize(64, 64))))
		ret=self.ui.exec_()
		if ret==True:
			self.ret=self.ui.text.text()
		else:
			self.ret=None


def yes_no_dlg(parent,text):
	msgBox = QMessageBox(parent)
	msgBox.setIcon(QMessageBox.Question)
	msgBox.setText("Question")
	msgBox.setInformativeText(text)
	msgBox.setStandardButtons(QMessageBox.Yes| QMessageBox.No )
	msgBox.setDefaultButton(QMessageBox.No)
	reply = msgBox.exec_()
	if reply == QMessageBox.Yes:
		return True
	else:
		return False

def widget_get_value(widget):
	if type(widget)==QLineEdit:
		return widget.text()
	elif type(widget)==gtkswitch:
		return widget.get_value()
	elif type(widget)==leftright:
		return widget.get_value()
	elif type(widget)==gpvdm_select:
		return widget.text()
	elif type(widget)==gpvdm_select_material:
		return widget.text()
	elif type(widget)==gpvdm_select_emission:
		return widget.text()
	elif type(widget)==QComboBox:
		return widget.itemText(widget.currentIndex())
	elif type(widget)==QComboBoxLang:
		return widget.currentText_english()
	elif type(widget)==QColorPicker:
		return [str(widget.r),str(widget.g),str(widget.b)]
	elif type(widget)==QChangeLog:
		return widget.toPlainText().split("\n")
	elif type(widget)==QComboBoxNewtonSelect:
		return widget.currentText()
	elif type(widget)==QComboBoxShape:
		return widget.currentText()
	elif type(widget)==QParasitic:
		return widget.text()
	elif type(widget)==generic_switch:
		return widget.get_value()
	elif type(widget)==shape_dos_switch:
		return widget.get_value()
	elif type(widget)==mobility_widget:
		return widget.get_values()
	else:
		return None

def widget_set_value(widget,value):

	widget.blockSignals(True)
	if type(widget)==QLineEdit:
		widget.setText(value)
	elif type(widget)==gtkswitch:
		widget.set_value(str2bool(value))
	elif type(widget)==leftright:
		widget.set_value(str2bool(value))
	elif type(widget)==gpvdm_select:
		widget.setText(value)
	elif type(widget)==gpvdm_select_material:
		widget.setText(value)
	elif type(widget)==gpvdm_select_emission:
		widget.setText(value)
	elif type(widget)==QComboBox:
		all_items  = [widget.itemText(i) for i in range(widget.count())]
		for i in range(0,len(all_items)):
			if all_items[i] == value:
				widget.setCurrentIndex(i)
				break
	elif type(widget)==QComboBoxLang:
		widget.setValue_using_english(value)
	elif type(widget)==QColorPicker:
		pass
	elif type(widget)==QChangeLog:
		widget.setText(value)
	elif type(widget)==QComboBoxNewtonSelect:
		widget.setValue(value)
	elif type(widget)==QComboBoxShape:
		widget.setValue(value)
	elif type(widget)==QParasitic:
		widget.setValue(value)
	elif type(widget)==generic_switch:
		widget.set_value(value)
	elif type(widget)==shape_dos_switch:
		widget.set_value(value)
	elif type(widget)==mobility_widget:
		widget.set_values(value)
	else:
		print("ooops")

	widget.blockSignals(False)


