# singsing

## Description de l'outil

```
singsing cpu,vmem,fsize CMD_LINE
singsing cpu,vmem,fsize -p PID
```

`singsing` prend 3 entiers séparés par deux virgules (`,`) comme premier argument (aucun espace entre ces valeurs). Ces entiers représentent, respectivement,
- `cpu` : la limite de temps CPU, en secondes, consommable par le processus.
- `vmem` : la taille maximale, en octets, de la mémoire virtuelle du processus.
- `fsize` : la taille maximale, en octets, d'un fichier que le processus peut créer. 

Si l'une de ces valeurs vaut `-1`, alors `singsing` ne touchera pas à la ressource correspondante et la laissera à la valeur par défaut du système.

Les arguments suivants de `singsing` sont,
- soit une option `-p` suivie du `PID` d'un processus sur lequel seront appliquées les limitations de ressources,
- soit une ligne de commandes `CMD_LINE` qui sera exécutée par `singsing` dans un processus fils, qu'il aura créé et dont il aura limité les ressources au préalable.

Une fois les limitations mise en place, l'utilitaire `singsing` va,
- soit se terminer et retourner la valeur `0` si `-p` est utilisée,
- soit exécuter dans un processus fils `CMD_LINE`, attendre la fin de ce processus fils et récupérer son status de terminaison,
  - s'il s'est terminé normalement, `singsing` va afficher la valeur retournée par le fils et retourner `0` à son tour,
  - s'il a reçu un signal qui l'a terminé, le numéro de ce dernier sera affiché par `singsing` et la valeur `1` sera retournée.

Mis à part les valeurs ci-haut, aucun autre message ne doit être affiché par l'utilitaire `singsing`.

<p>

<details>

<summary>Exemple</summary>

Dans l'exemple suivant on utilise le programme [`bench`].

<pre>
<b>iam@groot:~/$</b> time tests/bench -cpu
real	0m11,471s
user	0m11,470s
sys	0m0,000s
<b>iam@groot:~/$</b> ./singsing -1,-1,-1 tests/bench -cpu
255
<b>iam@groot:~/$</b> echo $?
0
<b>iam@groot:~/$</b> ./singsing 20,-1,-1 tests/bench -cpu
255
<b>iam@groot:~/$</b> echo $?
0
<b>iam@groot:~/$</b> ./singsing 5,-1,-1 tests/bench -cpu
9
<b>iam@groot:~/$</b> echo $?
1
</pre>

</details>

</p>

### Traitement des erreurs et valeur de retour

- Si un appel système fait par `singsing`, peu importe lequel, échoue, alors `singsing` doit s'arrêter et retourner la valeur `1`. Aucun message d'erreur ne sera  affiché.
- Si `CMD_LINE` se termine à cause d'un signal reçu, `singsing` va afficher le numéro du signal et retourner `1`.
- Si `CMD_LINE` se termine normalement, `singsing` va afficher la valeur retournée par `CMD_LINE` et retourner `0`.
- Si `singsing` est exécutée avec l'option `-p` alors il retournera `0` (sauf si, évidemment, un des appels systèmes de `singsing` a échoué).


## Auteur

Fang, Xin Ran


