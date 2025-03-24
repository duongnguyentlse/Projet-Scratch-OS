## 🖥️ ScratchOS – Système d'Exploitation Virtuel Simplifié

**ScratchOS** est un mini système d'exploitation de type UNIX-like développé en C. Il simule la gestion d'un disque dur, de fichiers, d'utilisateurs, de droits d'accès et fournit une interface ligne de commande complète.

---

## 🚀 Compilation et Exécution

### 🔧 Compilation
```bash
make
```

### ▶️ Lancement du programme principal
```bash
./os_simulation <répertoire_du_disque>
```

### 💾 Formatage du disque virtuel
Avant toute utilisation :
```bash
./cmd_format <répertoire> <taille_en_octets>
# Exemple :
./cmd_format ./disk_dir 500000
```

### 🛠️ Installation du système
Un programme d’installation permet de :
- Formater le disque
- Créer l’utilisateur `root`
- Initialiser le fichier `passwd`

---

## ⚙️ Fonctionnalités Clés

### 📦 Disque virtuel
- Fichier `.vdisk` simulant un disque dur réel (nommé `d0`)
- Formatage obligatoire via `cmd_format`
- Système de fichiers simple :
  - Blocs
  - Super bloc
  - Table d'inodes (catalogue)

### 👤 Gestion des utilisateurs
- Table d’utilisateurs avec identifiant (UID)
- Utilisateur root (UID 0) par défaut
- Ajout/suppression d’utilisateurs (`adduser`, `rmuser`)
- Authentification sécurisée par login / mot de passe (haché)
- Droits d’accès simplifiés (lecture/écriture codés de 0 à 3)

### 📁 Gestion des fichiers
- Fichiers texte uniquement
- Création, suppression, édition, lecture, listage
- Import/export vers le système hôte :
  - `load` / `store`
- Droits par utilisateur (propriétaire vs autres)
- Suivi des dates de création / modification

### 💻 Interpréteur de commandes (Mini-shell)
Interface en ligne de commande interactive :
- Authentification avec 3 essais maximum
- Commandes disponibles :
  - `ls [-l]` – Liste les fichiers
  - `cat <fichier>` – Affiche le contenu
  - `cr <fichier>` – Crée un fichier
  - `edit <fichier>` – Modifie un fichier
  - `rm <fichier>` – Supprime un fichier
  - `load` / `store` – Transfert depuis/vers l’hôte
  - `chown <fichier> <utilisateur>` – Changement de propriétaire
  - `chmod <fichier> <droit>` – Modification des droits
  - `listusers` – Affiche les utilisateurs
  - `adduser`, `rmuser` – Gestion utilisateurs (root uniquement)
  - `quit` – Quitte le système et sauvegarde

---

## 📂 Structure du projet
- `cmd_format.c` – Formatage du disque
- `install.c` – Installation initiale du système
- `main.c` – Lancement du mini-shell
- `user.c`, `file.c`, `inode.c` – Gestion des entités
- `shell.c` – Interpréteur de commandes

---

## 🛠️ Dépendances
- **Langage** : C (avec GCC)
- **Outils** : `make`, `fseek`, `fwrite`, `fread`, etc.

---

## 👥 Auteurs
Projet réalisé dans le cadre du module "Projet avancé" de la Licence Informatique – Université de Toulouse.  
Encadrant : Vincent Dugat  
Étudiants : NGUYEN Tung Duong

---

## 📌 Remarques
- Toute reformatage du disque **efface toutes les données existantes**.
- Le mot de passe root est à définir lors de l’installation.
- L’utilisation correcte des droits et du système d’authentification est cruciale pour la sécurité des fichiers.

---

