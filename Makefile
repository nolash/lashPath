INCLUDE = -Isrc/include -Iinclude
LIB = -Llib
LIBDIR = lib/
SOURCEDIR = src/
TESTDIR = tests/
DEPS = deps/


obj: obj_deps obj_core

obj_core:
	$(CC) -Wall $(INCLUDE) -g -o lash_game_path_simple.o -c $(SOURCEDIR)liblashgame/lash_game_path_simple.c
	$(CC) -Wall $(INCLUDE) -g -o lash_game_map.o -c $(SOURCEDIR)liblashgame/lash_game_map.c
	
obj_deps:
	$(CC) -Wall $(INCLUDE) -g -o lash_game_standard.o -c $(SOURCEDIR)liblashgame/lash_game_standard.c
	$(CC) -Wall $(INCLUDE) -g -o lash_tree3.o -c $(SOURCEDIR)liblash/lash_tree3.c
	$(CC) -Wall $(INCLUDE) -g -o lash_tree3_dump.o -c $(SOURCEDIR)liblash/lash_tree3_dump.c
	$(CC) -Wall $(INCLUDE) -g -o lash_debug_log.o -c $(SOURCEDIR)liblash/lash_debug_log.c
	$(CC) -Wall $(INCLUDE) -g -o lash_debug_timer.o -c $(SOURCEDIR)liblash/lash_debug_timer.c

static: obj
	$(AR) rcs $(LIBDIR)liblash.a lash_tree3.o lash_tree3_dump.o lash_debug_log.o lash_debug_timer.o
	$(AR) rcs $(LIBDIR)liblashgame.a lash_game_standard.o
	$(AR) rcs $(LIBDIR)liblashgamemap.a lash_game_path_simple.o lash_game_map.o

shared:
	$(CC) -Wall $(INCLUDE) -o lash_tree_lib.o -fPIC -c $(SOURCEDIR)liblash/lash_tree3.c
	$(CC) -Wall $(INCLUDE) -o lash_tree_dump_lib.o -fPIC -c $(SOURCEDIR)liblash/lash_tree3_dump.c
	$(CC) -shared -Wl,-soname,liblash.so.0 -o $(LIBDIR)liblash.so lash_tree_lib.o lash_tree_dump_lib.o
	$(CC) -Wall $(INCLUDE) -o lash_debug_log_lib.o -fPIC -c $(SOURCEDIR)liblash/lash_debug_log.c
	$(CC) -Wall $(INCLUDE) -o lash_debug_timer_lib.o -fPIC -c $(SOURCEDIR)liblash/lash_debug_timer.c
	$(CC) -shared -Wl,-soname,liblashdebug.so.0 -o $(LIBDIR)liblashdebug.so lash_debug_log_lib.o lash_debug_timer_lib.o
	
	$(CC) -Wall $(INCLUDE) -o lash_game_standard_lib.o -fPIC -c $(SOURCEDIR)liblashgame/lash_game_standard.c
	$(CC) -Wall $(INCLUDE) -o lash_game_map_lib.o -fPIC -c $(SOURCEDIR)liblashgame/lash_game_map.c
	$(CC) -Wall $(INCLUDE) -o lash_game_path_lib.o -fPIC -c $(SOURCEDIR)liblashgame/lash_game_path_simple.c
	$(CC) -shared -Wl,-soname,liblashgame.so.0 -o $(LIBDIR)liblashgame.so lash_game_standard_lib.o -lm
	$(CC) -shared -Wl,-soname,liblashgamemap.so.0 -o $(LIBDIR)liblashgamemap.so lash_game_map_lib.o lash_game_path_lib.o

test: static
	$(CC) -Wall $(INCLUDE) -g -o $(TESTDIR)findpath.o -c $(TESTDIR)src/findpath.c
	$(CC) -Wall $(LIB) -g -static -o $(TESTDIR)findpath_bin $(TESTDIR)findpath.o -llashgamemap -llashgame -llash -lm

	$(CC) -Werror $(INCLUDE) -g -o $(TESTDIR)pathsimpledemo.o -c $(TESTDIR)src/pathsimpledemo.c
	$(CC) -Wall $(LIB) -g -static -o $(TESTDIR)pathsimpledemo_bin $(TESTDIR)pathsimpledemo.o -llashgamemap -llashgame -llash -lm

	$(CC) -Wall $(INCLUDE) -g -o $(TESTDIR)pathsimple.o -c $(TESTDIR)src/pathsimple.c
	$(CC) -Wall $(LIB) -g -static -o $(TESTDIR)pathsimple_bin $(TESTDIR)pathsimple.o -llashgamemap -llashgame -llash -lm 

	$(CC) -Wall $(INCLUDE) -g -o $(TESTDIR)pathcheckmodifier.o -c $(TESTDIR)src/pathcheckmodifier.c
	$(CC) -Wall $(LIB) -g -static -o $(TESTDIR)pathcheckmodifier_bin $(TESTDIR)pathcheckmodifier.o -llashgamemap -llashgame -llash -lm
