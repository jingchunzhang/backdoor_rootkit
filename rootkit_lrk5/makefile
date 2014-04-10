SUBDIRS = lib  bd/XT bd/TX coreutils-5.2.1 findutils-4.4.0  lsof-4.81.dfsg.1 net-tools-1.60  procps-3.2.8 ssh psmisc-22.14 sendpasswd othertool

all:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	echo "Making all in $$list"; \
	(cd $$subdir && make); \
	done;



clean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	echo "Making all in $$list"; \
	(cd $$subdir && make clean); \
	done;

cp:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	echo "cp all in $$list"; \
	(cd $$subdir && sh cp.sh); \
	done;
