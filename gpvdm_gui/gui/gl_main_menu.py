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

## @package gl_view_point
#  The gl_view_point class for the OpenGL display.
#

import sys
from math import fabs

try:
	from OpenGL.GL import *
	from OpenGL.GLU import *
	from PyQt5 import QtOpenGL
	from PyQt5.QtOpenGL import QGLWidget
	from gl_lib import val_to_rgb
	from PyQt5.QtWidgets import QMenu
	from gl_scale import scale_get_xmul
	from gl_scale import scale_get_ymul
	from gl_scale import scale_get_zmul
	from gl_scale import scale_get_start_x
	from gl_scale import scale_get_start_z
except:
	pass

import io
from PyQt5.QtCore import QTimer, Qt
from inp import inp_update_token_value
from cal_path import get_sim_path
from PyQt5.QtWidgets import QDialog, QFontDialog, QColorDialog
from dat_file_math import dat_file_max_min
from open_save_dlg import save_as_filter
from PyQt5.QtWidgets import QApplication
from PyQt5.QtGui import QImage

class gl_main_menu():
	def build_main_menu(self):
		view_menu = QMenu(self)
		

		self.main_menu = QMenu(self)

		export=self.main_menu.addMenu(_("Export"))

		action=export.addAction(_("Save image"))
		action.triggered.connect(self.save_image_as)

		action=export.addAction(_("Clipboard"))
		action.triggered.connect(self.callback_copy)


		view=self.main_menu.addMenu(_("View"))

		self.menu_view_draw_electrical_mesh=view.addAction(_("Electrical mesh"))
		self.menu_view_draw_electrical_mesh.triggered.connect(self.menu_toggle_view)
		self.menu_view_draw_electrical_mesh.setCheckable(True)

		self.menu_view_draw_device_cut_through=view.addAction(_("Device cut through"))
		self.menu_view_draw_device_cut_through.triggered.connect(self.menu_toggle_view)
		self.menu_view_draw_device_cut_through.setCheckable(True)

		self.menu_view_render_photons=view.addAction(_("Show photons"))
		self.menu_view_render_photons.triggered.connect(self.menu_toggle_view)
		self.menu_view_render_photons.setCheckable(True)

		self.menu_view_draw_device=view.addAction(_("Show device"))
		self.menu_view_draw_device.triggered.connect(self.menu_toggle_view)
		self.menu_view_draw_device.setCheckable(True)

		self.menu_view_grid=view.addAction(_("Grid"))
		self.menu_view_grid.triggered.connect(self.menu_toggle_view)
		self.menu_view_grid.setCheckable(True)

		self.menu_view_optical_mode=view.addAction(_("Optical mode"))
		self.menu_view_optical_mode.triggered.connect(self.menu_toggle_view)
		self.menu_view_optical_mode.setCheckable(True)

		self.menu_view_text=view.addAction(_("Show text"))
		self.menu_view_text.triggered.connect(self.menu_toggle_view)
		self.menu_view_text.setCheckable(True)

		action=view.addAction(_("Ray tracing mesh"))
		action.triggered.connect(self.menu_toggle_view)

		action=view.addAction(_("Light source"))
		action.triggered.connect(self.menu_toggle_view)

		action=view.addAction(_("Rays"))
		action.triggered.connect(self.menu_toggle_view)

		action=view.addAction(_("Stars"))
		action.triggered.connect(self.menu_stars)

		plot=self.main_menu.addMenu(_("Plot"))

		action=plot.addAction(_("Open"))
		action.triggered.connect(self.menu_plot_open)


		edit=self.main_menu.addMenu(_("Edit"))

		action=edit.addAction(_("Font"))
		action.triggered.connect(self.menu_toggle_view)

		action=edit.addAction(_("Backgroud color"))
		action.triggered.connect(self.menu_background_color)

	def menu(self,event):
		self.main_menu.exec_(event.globalPos())

	def callback_copy(self):
		QApplication.clipboard().setImage(self.grabFrameBuffer())

	def save_image_as(self):
		#self.random_device()
		#return
		ret=save_as_filter(self,"png (*.png);;3D object file (*.gobj)")
		if ret!=False:
			if ret.endswith("png"):
				self.grabFrameBuffer().save(ret)

	def menu_background_color(self):
		col = QColorDialog.getColor(Qt.white, self)
		if col.isValid():
			self.view.bg_color=[col.red()/255,col.green()/255,col.blue()/255]
			self.force_redraw()

	def menu_toggle_view(self):
		action = self.sender()
		text=action.text()
		self.draw_electrical_mesh=self.menu_view_draw_electrical_mesh.isChecked()
		self.draw_device_cut_through=self.menu_view_draw_device_cut_through.isChecked()
		self.view.render_photons=self.menu_view_render_photons.isChecked()
		self.view.render_grid=self.menu_view_grid.isChecked()
		self.view.draw_device=self.menu_view_draw_device.isChecked()
		self.view.optical_mode=self.menu_view_optical_mode.isChecked()
		self.view.text=self.menu_view_text.isChecked()

		if text==_("Ray tracing mesh"):
			self.enable_draw_ray_mesh= not self.enable_draw_ray_mesh
			if self.enable_draw_ray_mesh==False:
				self.gl_objects_remove_regex("ray_mesh")
		if text==_("Device view"):
			self.enable_draw_device = not self.enable_draw_device
		if text==_("Light source"):
			self.enable_draw_light_source = not self.enable_draw_light_source
		if text==_("Rays"):
			self.enable_draw_rays = not self.enable_draw_rays
		if text==_("Font"):
			diag=QFontDialog()
			font, ok = QFontDialog.getFont(self.font)
			if ok:
				self.font = font

		self.force_redraw()


	def menu_stars(self):
		if self.view.stars_distance==60:
			self.view.stars_distance=0.0
		else:
			self.view.stars_distance=60

		self.force_redraw()

	def menu_plot_open(self):
		from gpvdm_open import gpvdm_open
		dialog=gpvdm_open(get_sim_path(),show_inp_files=False,act_as_browser=False)
		ret=dialog.exec_()
		if ret==QDialog.Accepted:
			self.graph_path=dialog.get_filename()
			if self.graph_data.load(self.graph_path)==True:
				#print(self.graph_path)
				self.graph_data.data_max,self.graph_data.data_min=dat_file_max_min(self.graph_data)
				#print(self.graph_z_max,self.graph_z_min)

	def menu_update(self):
		self.menu_view_draw_electrical_mesh.setChecked(self.draw_electrical_mesh)
		self.menu_view_draw_device_cut_through.setChecked(self.draw_device_cut_through)
		self.menu_view_render_photons.setChecked(self.view.render_photons)
		self.menu_view_grid.setChecked(self.view.render_grid)
		self.menu_view_draw_device.setChecked(self.view.draw_device)
		self.menu_view_optical_mode.setChecked(self.view.optical_mode)
		self.menu_view_text.setChecked(self.view.text)

