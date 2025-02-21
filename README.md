# qemulator

Un emulatore di circuiti quantistici scritto in C.

---

Attualmente gli unici gate utilizzabili sono `H` (hadamard) e `In` (Identità), situati nel file `gates.h`.

Al momento si possono fare operazione basilari quali concatenazione (prodotto matriciale tra due operatori) e prodotto tensoriale tra operatori e misurazione. 
