# qemulator

Un emulatore di circuiti quantistici scritto in C.

---

Attualmente gli unici gate utilizzabili sono `H` (hadamard) e `In` (Identità), situati nel file `gates.h`.

Al momento si possono fare operazione basilari quali concatenazione (prodotto matriciale tra due operatori) e prodotto tensoriale tra operatori e misurazione. 

---

Il file `main.c` contiene del codice di esempio su come utilizzare le funzionalità correnti per implementare l'algoritmo di Deutsch, compilabile tramite il comando `gcc -o <nome_eseguibile> main.c -lm`.