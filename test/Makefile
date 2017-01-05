all: create mkdir read write unlink rmdir link symlink

create: create.c
	gcc -Wall -Werror create.c -o create

mkdir: mkdir.c
	gcc -Wall -Werror mkdir.c -o mkdir

read: read.c
	gcc -Wall -Werror read.c -o read

write: write.c
	gcc -Wall -Werror write.c -o write

unlink: unlink.c
	gcc -Wall -Werror unlink.c -o unlink

rmdir: rmdir.c
	gcc -Wall -Werror rmdir.c -o rmdir

link: link.c
	gcc -Wall -Werror link.c -o link

symlink: symlink.c
	gcc -Wall -Werror symlink.c -o symlink

clean:
	rm -f create
	rm -f mkdir
	rm -f read
	rm -f write
	rm -f unlink
	rm -f rmdir
	rm -f link
	rm -f symlink
