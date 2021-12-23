all:
	-adduser dummy
	-sudo rm vulnerable-bo*
	gcc bo.c                       -o vulnerable-bo
	gcc bo2.c                      -o vulnerable-bo2
	gcc bo3.c -fno-stack-protector -o vulnerable-bo3
	gcc bo4.c  -o vulnerable-bo4
	sudo chown dummy:dummy vulnerable-bo*
	sudo chmod 755 vulnerable-bo*
	sudo chmod u+s vulnerable-bo*	
	sudo chmod u-s vulnerable-bo
	sudo chmod 755 vulnerable-bo
