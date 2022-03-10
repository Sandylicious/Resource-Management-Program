/**
* Ce programme permet d'appliquer trois limites sur un processus selon les arguments saisis
* par l'utilisateur, une limite au temps CPU (en seconde), une limite sur la taille de la 
* memoire virtuelle (en octets) et une limite sur la taille d'un fichier que le processus 
* peut creer (en octets). Dans le cas ou la nouvelle valeur de la limite vaut -1, aucune 
* modification ne sera apportee a l'ancienne limite.
*
* @author Fang, Xin Ran
* @version 2022-02-27
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>

#define OPTION_P "-p"

/**
 * Cette methode applique une limite a l'une des ressources d'un processus, la meme valeur 
 * sera appliquee aux limite souple et limite stricte. Dans le cas ou la nouvelle valeur de 
 * la limite vaut -1, aucune modification ne sera apportee a l'ancienne limite.
 * 
 * @param	pid			pid du processus
 * @param 	resource		type de ressource a limiter
 * @param 	limit			nouvelle limite a appliquer
 */
void setPrlimit(pid_t pid, int resource, int limit);

/**
 * Cette methode applique trois limites sur un processus, une limite au temps CPU (en seconde), 
 * une limite sur la taille de la memoire virtuelle (en octets) et une limite sur la taille d'un 
 * fichier que le processus peut creer (en octets). Dans le cas ou la nouvelle valeur de la limite 
 * vaut -1, aucune modification ne sera apportee a l'ancienne limite.
 * 
 * @param	pid				pid du processus
 * @param 	cpu_limit		limite de temps CPU
 * @param 	vmem_limit		limite sur la taille de la memoire virtuelle du processus
 * @param 	fsize_limit		limite sur la taille d'un fichier que le processus peut creer
 */
void setThreePrlimit(pid_t pid, int cpu_limit, int vmem_limit, int fsize_limit);

int main(int argc, char *argv[])
{
   // Recuperer la valeur des trois limites saisie par utilisateur a l'argv[1]
	char *nb_temp;		// string qui enregistre temporairement chaque valeur extraite du argv[1]
   int limits[3], i;

   for (i = 0; i < 3; i++)
   {
      nb_temp = (char*) strtok(i ? NULL : argv[1], ",");
      limits[i] = atoi(nb_temp);
   }

   int cpu_limit = limits[0], vmem_limit = limits[1], fsize_limit = limits[2];

   pid_t pid;

	// Valider argv[2]
   if (strcmp(argv[2], OPTION_P) == 0)
   {
      // Utilisateur a choisi option -p et a saisi le PID d'un processus
		pid = (pid_t) atoi(argv[3]);

      setThreePrlimit(pid, cpu_limit, vmem_limit, fsize_limit);
   }
   else
   {
		// Utilisateur a saisi une ligne de commandes	
      pid_t p_fils = fork(), wait_result;
      int status;		// status du waitpid

      if (p_fils == -1)
         return 1;

      if (p_fils == 0)
      {
         pid = getpid();

         setThreePrlimit(pid, cpu_limit, vmem_limit, fsize_limit);

         char *newargv[] = { NULL };
         char *newenviron[] = { NULL };
			
			// Recuperer les arguments de la commande s'ils existent 
         if (argc >= 3)
         {
            for (i = 0; argv[i + 2] != NULL; i++)
               newargv[i] = argv[i + 2];

            newargv[i] = NULL;
         }

         if (execve(argv[2], newargv, newenviron) == -1)
            return 1;
      }
      else
      {
         do {
            wait_result = waitpid(p_fils, &status, WUNTRACED | WCONTINUED);

            if (wait_result == -1)
               return 1;

            if (WIFEXITED(status))
            {
               printf("%d", WEXITSTATUS(status));
               return 0;
            }
            else if (WIFSIGNALED(status))
            {
               printf("%d", WTERMSIG(status));
               return 1;
            }
            else if (WIFSTOPPED(status))
            {
               printf("%d", WSTOPSIG(status));
               return 1;
            }
         } while (!WIFEXITED(status) && !WIFSIGNALED(status));
      }
   }

   return 0;
}

void setPrlimit(pid_t pid, int resource, int limit)
{
   struct rlimit old, new;
   new.rlim_cur = new.rlim_max = (rlim_t) limit;

   if (prlimit(pid, resource, &new, &old) == -1)
      exit(1);
}

void setThreePrlimit(pid_t pid, int cpu_limit, int vmem_limit, int fsize_limit)
{
   if (cpu_limit != -1)
      setPrlimit(pid, RLIMIT_CPU, cpu_limit);

   if (vmem_limit != -1)
      setPrlimit(pid, RLIMIT_AS, vmem_limit);

   if (fsize_limit != -1)
      setPrlimit(pid, RLIMIT_FSIZE, fsize_limit);
}
