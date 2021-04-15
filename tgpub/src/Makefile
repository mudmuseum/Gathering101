CC      = gcc
PROF    =
NOCRYPT = 
C_FLAGS = -O -Wall -g3 -ggdb -Dbsdi $(PROF) $(NOCRYPT) 
L_FLAGS = -O $(PROF) -lcrypt -lnsl

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
          db.o fight.o handler.o interp.o magic.o save.o special.o update.o \
          kav_info.o kav_fight.o kav_wiz.o puck_fight.o puck_move.o puck_wiz.o \
	  puck_comm.o rot_wiz.o board.o clan.o class_demon.o class_vampire.o \
	  class_ww.o class_mage.o class_highlander.o

merc: $(O_FILES)
	rm -f Gathering
	$(CC) -o Gathering $(O_FILES) $(L_FLAGS)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean :
	rm -f *.o
	rm -f Gathering
	rm -f *.*~
