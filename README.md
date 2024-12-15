README pre Program Zadanie
Úvod

Program Zadanie je komplexný UNIXový program na demonštráciu medziprocesovej komunikácie a synchronizácie. Využíva rôzne UNIXové konštrukty ako sú rúry (pipes), semafory, zdieľaná pamäť a signály pre koordináciu viacerých procesov.
Funkcie

Program obsahuje nasledujúce hlavné funkcie:

    Vytvorenie a správa procesov: Inicializuje a spravuje procesy ako sú Serv1, Serv2, P1, P2, T, D, Pr, S.
    Medziprocesová komunikácia: Využíva rúry pre prenos dát medzi procesmi.
    Synchronizácia procesov: Používa semafory na synchronizáciu prístupu k zdieľaným zdrojom.
    Spracovanie signálov: Na koordináciu procesov a reakciu na udalosti.

Komponenty

    Rúry (Pipes): Na jednosmernú komunikáciu medzi procesmi.
    Semafor (Semaphore): Na synchronizáciu procesov.
    Zdieľaná Pamäť (Shared Memory): Na zdieľanie dát medzi procesmi.
    Signály (Signals): Na koordináciu a ukončenie procesov.

Štruktúra Kódu

Kód je rozdelený do viacerých častí:

    Hlavný Program (main): Inicializuje zdroje a spúšťa ostatné procesy.
    Procesy (P1, P2, T, D, Pr, S, Serv1, Serv2): Každý proces má určenú úlohu v rámci programu.
    Pomocné Funkcie: Na vytváranie signálov, inicializáciu semaforov a iné úlohy.

Inštalácia a Spustenie

    Kompilácia: Kompilujte všetky zdrojové súbory pomocou príkazu make alebo ekvivalentného nástroja na váš systém.
    Spustenie: Program spustite pomocou príkazu ./Zadanie <port1> <port2>, kde <port1> a <port2> sú čísla portov pre TCP a UDP komunikáciu.

Bezpečnosť a Chybové stavy

    Ošetrenie chýb: Program obsahuje kontrolu chýb a vhodné ošetrenie pre prípad neúspechu systémových volaní.
    Bezpečné ukončenie: Program podporuje bezpečné ukončenie všetkých spustených procesov pomocou signálov.

Závislosti

Program vyžaduje UNIXové prostredie s podporou systémových volaní pre prácu s rúrami, semaformi, zdieľanou pamäťou a signálmi.
