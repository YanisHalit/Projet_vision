# Projet_vision

Rayan Boucherrab 28610497
Yanis Halit      28600464


-Le choix des tests/bench à effectuer peuvent être décidés à partir du fichier "./src/main.c" en decommentant.

-Les résultats des benchs sont stockées dans le dossier "bench_txt" dans la catégorie qui leur correspond: 
    "Sans SWP" où l'on trouve les résultats des versions max et ouvertures sans SWP.

    "Pack_unpack_interne" ou l'on trouve les résultats des version max et ouvertures pour SWP 8/16/32 avec le pack unpack compris dans le bench.

    "Pack_unpack_externe" ou l'on trouve les résultats des version max et ouvertures pour SWP 8/16/32 avec le pack unpack non compris dans le bench.

    "Motion" ou l'on trouve l'ensemble des résultats de tests réalisés sur la séquence "car3"


-Le programme s'execute avec un simple "$ make && ./exe/motion_acc.exe"


-Vous pourrez trouver dans le fichier PLOT l'ensemble des commandes à entrer dans gnuplot pour afficher les graphiques de performances à partir des fichiers récuperés il est cependant nécéssaire de se trouver dans le bon répértoire ou d'y entrer le bon chemin d'accès.

-Les test de benchs dans motions.c ne stockent pas les images obtenues dans le dossier results, seul la version V1 nous les y renvoie.



Merci.