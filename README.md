# 🖥️ Mini Système d'Exploitation – Simulation avec Disque Virtuel

## 📌 Présentation

Ce projet consiste à concevoir un **mini système d'exploitation** en langage **C**, incluant des fonctionnalités telles que :

- Gestion des fichiers (création, suppression, lecture, écriture)
- Gestion des utilisateurs avec droits d'accès simplifiés
- Système de connexion sécurisé
- Interpréteur de commandes (shell)
- Disque dur simulé à l'aide d’un **fichier de disque virtuel**

Le système fonctionne entièrement en mémoire et stocke les fichiers et métadonnées dans une structure simulée correspondant à un vrai **système de fichiers**.

---

## 🧱 Architecture du Système

Le système est conçu selon une architecture **en couches**, conformément aux bonnes pratiques des systèmes d'exploitation :

1. **Couche physique** : le disque dur est simulé par un **fichier local** (disque virtuel)
2. **Gestion du système de fichiers** : tables de fichiers, d’inodes et de blocs
3. **Gestion des utilisateurs** : table des utilisateurs avec permissions de base
4. **Interface utilisateur** : ligne de commande avec interpréteur simple
5. **Procédure de login** : vérification des identifiants et ouverture de session

---

## ⚙️ Prérequis

Avant d’exécuter le programme, assurez-vous de respecter les conditions suivantes :

- Utiliser **Linux ou macOS** (Windows n'est pas supporté, le projet ayant été développé sous macOS)
- Compiler avec `make`
- Il est recommandé d’exécuter :
  ```bash
  make clean

---

### ▶️ Lancer le programme
```bash
./os_simulation
```

---

## ⚙️ Fonctionnalités Clés

### 📁 Gestion de fichiers
- Création et suppression de fichiers
- Lecture et écriture dans les fichiers
- Affichage du contenu des fichiers

### 👤 Gestion des utilisateurs
- Création et suppression d'utilisateurs
- Connexion sécurisée via login et mot de passe
- Droits d'accès personnalisés (lecture / écriture)

### 💾 Disque virtuel
- Utilisation d’un fichier `.vdisk` pour simuler un disque dur
- Gestion d’un système de fichiers simple :
  - Bloc d’allocation
  - Table des fichiers
  - Métadonnées

### 💻 Interpréteur de commandes (Mini-shell)
- Interface ligne de commande pour interagir avec le système
- Commandes disponibles :
  - `login` – Se connecter à un compte utilisateur
  - `create` – Créer un fichier
  - `read` – Lire un fichier
  - `write` – Écrire dans un fichier
  - `ls` – Lister les fichiers disponibles
  - `exit` – Quitter le système

---

## 🛠️ Dépendances
- Standard C Library
- Outils GNU `make`, `gcc`

---

## 🧑‍💻 Auteurs
Projet réalisé dans le cadre d’un projet universitaire.  
Contact : [tung-duong.nguyen@univ-tlse3.fr]

---
