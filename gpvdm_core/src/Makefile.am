OBJS=  sim_find_n0.o server.o sim_run.o pos.o config.o anal.o run_electrical_dll.o sim.o check_and_gen_dos.o
# -pg  -lefence  -lefence -lefence 
#echo 428000 > /proc/sys/vm/max_map_count  for when efence runs out of memroy 
AROPT=

inc=-I../include/ -L../

all: main.c $(OBJS)
	$(CC) -shared -o ../libgpvdm_core.@DLIB_EXT@ @GPVDM_CORE_LIB_FILES@ -L/home/rod/windll/compiled_dlls2/ @WIN_INTERNET@ -lzip
	@WIN_ICON@
	$(CC) main.c $(OBJS) -o ../gpvdm_core@EXE_EXT@ @WIN_RES_FILE@ $(inc) @COMPILE_FLAG@ -Wl,--whole-archive @LOCAL_LINK@ -Wl,--no-whole-archive @LDFLAGS@ @LIBS@ @OPEN_CL_LINK@ -lpthread -pg @SRC_DEBUG@
	ln -s -f gpvdm_core@EXE_EXT@ ../go@EXE_EXT@
	ln -s -f ./gpvdm_core/gpvdm_core@EXE_EXT@ ../../go@EXE_EXT@
	@echo "$(DESTDIR)$(bindir)" > bindir
	@echo "$(DESTDIR)$(datadir)" > datadir
	@echo "$(DESTDIR)$(datarootdir)" > datarootdir

	cp ../../sim.gpvdm ../base.gpvdm -f
	rm ../../base.gpvdm -f
	ln -s ./gpvdm_core/base.gpvdm ../../base.gpvdm
	rm ../../libgpvdm_core.@DLIB_EXT@ -f
	ln -s ./gpvdm_core/libgpvdm_core.@DLIB_EXT@ ../../libgpvdm_core.@DLIB_EXT@
	@echo "(gpvdm_build_finished)"
	
.c.o : 
	$(CC) -c $(DEFINE_FLAGS) $(inc) $(CPPFLAGS) $(warn) @COMPILE_FLAG@ $< -o $@

clean:
	rm *.o *.a *.dll *.exe ../../base.gpvdm -f
