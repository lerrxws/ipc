BFILES= zadanie proc_serv2 proc_d proc_t proc_p1 proc_p2
OEFILS= proc_pr.err proc_pr.out proc_serv1.err proc_serv1.out proc_t.err proc_t.out proc_s.err proc_s.out serv2.txt

all:$(BFILES)

zadanie: zadanie.cpp
	g++ zadanie.cpp -o zadanie
proc_serv2: proc_serv2.cpp
	g++ proc_serv2.cpp -o proc_serv2
proc_d: proc_d.cpp
	g++ proc_d.cpp -o proc_d
proc_t: proc_t.cpp
	g++ proc_t.cpp -o proc_t
proc_p1: proc_p1.cpp
	g++ proc_p1.cpp -o proc_p1
proc_p2: proc_p2.cpp
	g++ proc_p2.cpp -o proc_p2		
clean:
	rm -f $(BFILES) $(OEFILS)