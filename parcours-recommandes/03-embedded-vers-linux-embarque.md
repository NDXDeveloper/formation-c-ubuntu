🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Parcours Recommandé : Embedded → Linux Embarqué

## 🎯 Profil Cible

Vous êtes développeur embarqué avec une expérience sur microcontrôleurs (Arduino, STM32, ESP32, PIC, AVR...). Vous programmez en C (ou C++) pour des systèmes bare-metal ou avec des RTOS légers (FreeRTOS, Zephyr). Vous maîtrisez les concepts hardware (GPIO, interruptions, timers, SPI, I2C, UART) mais vous n'avez jamais travaillé avec un système d'exploitation complet comme Linux.

**Prérequis :**
- Expérience en développement C pour microcontrôleurs (1+ an)
- Compréhension des concepts hardware (registres, périphériques, interruptions)
- Connaissance de base de l'électronique numérique
- Expérience avec au moins un microcontrôleur (Arduino, STM32...)
- Notions de compilation croisée (cross-compilation)

**Niveau de départ :** Expert en embedded bare-metal, débutant en Linux

---

## 🚀 Objectifs de la Transition

À l'issue de ce parcours, vous serez capable de :

1. **Comprendre les différences fondamentales** entre bare-metal et Linux embarqué
2. **Développer pour Linux embarqué** (Raspberry Pi, BeagleBone, i.MX, SAMA5D...)
3. **Écrire des drivers Linux** (modules kernel, device tree)
4. **Cross-compiler** pour différentes architectures (ARM, ARM64, RISC-V)
5. **Créer des distributions Linux embarquées** personnalisées (Yocto, Buildroot)
6. **Optimiser** pour les contraintes embarquées (mémoire, boot time, consommation)
7. **Utiliser eBPF** pour le débogage et monitoring embarqué

**La grande différence :**
- **Bare-metal :** Vous contrôlez directement le hardware, pas d'OS
- **Linux embarqué :** Vous avez un OS complet avec scheduler, MMU, système de fichiers, networking...

---

## 📚 Transition : Du Bare-Metal vers Linux

### Pourquoi passer à Linux Embarqué ?

**Avantages de Linux embarqué :**
- ✅ Écosystème logiciel riche (bibliothèques, drivers, outils)
- ✅ Networking complet (TCP/IP stack, WiFi, Bluetooth)
- ✅ Système de fichiers (SQLite, logs, configuration)
- ✅ Multitâche préemptif avec MMU
- ✅ Outils de développement puissants
- ✅ Support communautaire énorme

**Quand rester en bare-metal :**
- ❌ Contraintes temps réel strictes (< 1ms)
- ❌ Très faible consommation critique (nW)
- ❌ Hardware trop limité (< 2MB RAM)
- ❌ Boot time ultra-rapide requis (< 1s)

**Le sweet spot de Linux embarqué :**
- Processeurs ARM Cortex-A (A7, A53, A72...)
- 32MB+ de RAM (idéalement 128MB+)
- Stockage 256MB+ (eMMC, SD, NAND)
- Applications IoT, gateways, HMI, vision industrielle

---

### Concepts Nouveaux pour un Développeur Embedded

| Bare-Metal | Linux Embarqué |
|------------|----------------|
| Pas d'OS, code au reset | Bootloader → Kernel → Init → Applications |
| Registres directs | Device drivers + Device Tree |
| Polling ou interruptions | Système d'événements, select/poll/epoll |
| Une seule "tâche" (ou RTOS simple) | Processus multiples, multithreading |
| Mémoire statique ou heap simple | MMU, mémoire virtuelle, swap |
| Pas de système de fichiers | ext4, FAT, SquashFS, overlayfs |
| Communication série simple | Sockets, pipes, D-Bus, MQTT... |
| Flash directe | OTA updates, A/B partitions |

---

## 📚 Parcours Modulaire Adapté

### Phase 1 : Rafraîchissement et Mise à Niveau C (2-3 semaines)

#### 🟢 Module 1 : L'Écosystème C sous Linux

Vous connaissez le C, mais le C pour Linux a quelques différences avec le C embarqué.

**Sections essentielles :**
- 1.2 La relation symbiotique entre C et UNIX/Linux
- 1.3 Normes du langage (vous utilisez probablement C99, Linux utilise C11/C17)
- 1.4 C vs C++ vs Rust : Quand choisir C ?

**Différences importantes à noter :**
- Sur microcontrôleur : `int` peut faire 16 bits (AVR)
- Sur Linux : `int` fait 32 bits, `long` peut faire 64 bits sur ARM64
- Les tailles des types sont plus standardisées sous Linux

---

#### 🟢 Module 2 : Fondamentaux du Langage *(révision rapide)*

Vous connaissez déjà la plupart de ces concepts, mais quelques sections méritent attention :

**Chapitre 4 : Types de données**
- 4.5 Endianness et portabilité ⭐ (important pour le networking et les protocoles)

**Chapitre 6 : Les Fonctions**
- 6.2 La Pile : Comprendre les Stack Frames
- 6.5 La fonction main et ses paramètres (argc, argv)

**Nouveauté pour vous :** La fonction `main()` peut recevoir des arguments en ligne de commande, ce qui n'existe pas en bare-metal !

---

#### 🟡 Module 3 : La Gestion de la Mémoire *(Priorité : MOYENNE)*

**Différence majeure avec bare-metal :**

En bare-metal, vous gérez la mémoire de manière très contrôlée :
```c
// Bare-metal : allocation statique ou pool fixe
static uint8_t buffer[1024];
```

Sous Linux, vous avez la mémoire virtuelle et l'allocation dynamique :
```c
// Linux : allocation dynamique avec malloc
uint8_t *buffer = malloc(1024);  
if (buffer == NULL) {  
    // Gestion d'erreur
}
// ... utilisation ...
free(buffer);
```

**Chapitres à approfondir :**
- Chapitre 7 : Les Pointeurs (révision)
- Chapitre 9 : Allocation Dynamique
  - 9.1 Stack vs Heap ⭐ (avec MMU, c'est différent)
  - 9.3 malloc, calloc, realloc, free
  - 9.5 Fuites mémoire

**Nouveauté importante :** Sous Linux, vous avez une MMU qui protège la mémoire. Un accès invalide génère un Segmentation Fault au lieu de corrompre silencieusement la mémoire comme en bare-metal.

---

### Phase 2 : Environnement de Développement Linux Embarqué (2-3 semaines)

#### 🔵 Module 1 - Section 2.5 : DevContainers *(Priorité : HAUTE)* ⭐

**Pourquoi DevContainers pour l'embarqué ?**

En développement embarqué, chaque projet a sa toolchain spécifique :
- SDK ARM pour STM32
- ESP-IDF pour ESP32
- Yocto SDK pour projet custom

Les DevContainers vous permettent d'avoir un environnement reproductible avec toutes les toolchains nécessaires.

**Sections à maîtriser :**
- 2.5.1 Qu'est-ce qu'un DevContainer ?
- 2.5.2 Structure du fichier .devcontainer.json
- 2.5.3 Configuration GCC/GDB/CMake/Valgrind

**Exemple concret :** Un DevContainer avec :
- Cross-compiler ARM
- QEMU pour émulation ARM
- GDB multiarch
- Outils Yocto/Buildroot

---

#### 🔴 Module 5 - Chapitre 13 : Build Systems *(Priorité : CRITIQUE)* 🔥

**Transition importante :**

En embedded bare-metal, vous utilisez souvent :
- IDE propriétaire (STM32CubeIDE, MPLAB...)
- Makefiles simples
- Scripts maison

En Linux embarqué, le standard est :

**13.1 CMake : Le standard moderne** ⭐
- 13.1.2 Installation et premiers pas
- 13.1.3 Structure d'un CMakeLists.txt
- 13.1.5 Gestion des dépendances
- 13.1.6 Build types (Debug, Release, MinSizeRel)

**Pourquoi CMake ?**
- Support natif de la cross-compilation
- Intégration avec Yocto et Buildroot
- Standard de l'industrie pour Linux embarqué

**13.2 GNU Make : Fondamentaux**
- Toujours utile, surtout pour les Makefiles kernel

---

### Phase 3 : Programmation Système Linux (4-5 semaines)

#### 🔴 Module 6 : Programmation Système Linux *(Priorité : CRITIQUE)* 🔥🔥🔥

**C'est ici que tout change par rapport au bare-metal.**

**Chapitre 16 : Gestion des Fichiers et I/O** ⭐⭐⭐

**Différence fondamentale :**

```c
// Bare-metal : accès registre direct
GPIOA->ODR |= (1 << 5);  // Set PA5

// Linux : tout est fichier !
int fd = open("/sys/class/gpio/gpio5/value", O_WRONLY);  
write(fd, "1", 1);  
close(fd);  
```

**Sections essentielles :**
- 16.1 Descripteurs de fichiers ⭐
- 16.2 Appels système (open, read, write, close)
- 16.3 Différence appels système vs bibliothèque
- 16.5 I/O bufferisé vs non bufferisé

**Nouveauté majeure :** Sous Linux, vous n'accédez plus directement aux registres. Vous passez par :
- Des fichiers dans `/sys/` (sysfs)
- Des fichiers dans `/dev/` (device files)
- Des drivers noyau

---

**Chapitre 17 : Processus et Signaux** ⭐⭐

**Concept nouveau pour embedded :**

En bare-metal, vous avez typiquement une boucle infinie :
```c
while(1) {
    // Main loop
}
```

Sous Linux, vous avez des processus séparés :

**Sections essentielles :**
- 17.1 Création de processus (fork, exec)
- 17.2 Les PIDs et hiérarchie
- 17.5 Gestion des signaux (SIGTERM, SIGKILL...)

**Cas d'usage embarqué :**
- Un processus pour l'interface utilisateur
- Un processus pour la communication réseau
- Un processus pour l'acquisition de données
- Communication entre processus via pipes ou sockets

---

**Chapitre 20 : Réseau (Sockets)** ⭐

**Grande différence avec bare-metal :**

En bare-metal avec ESP32 ou module WiFi :
```c
// API propriétaire
esp_wifi_connect();  
esp_http_client_perform();  
```

Sous Linux, vous utilisez les sockets POSIX standard :
```c
int sock = socket(AF_INET, SOCK_STREAM, 0);  
connect(sock, ...);  
send(sock, data, len, 0);  
```

**Sections à maîtriser :**
- 20.1 Modèle Client/Serveur
- 20.3 Sockets TCP
- 20.4 Sockets UDP
- 20.9 Non-blocking I/O et epoll

**Avantage énorme :** Le code réseau que vous écrivez fonctionne de la même manière sur PC, serveur, et système embarqué Linux.

---

**Chapitre 18 : Threads et Concurrence (POSIX)** ⭐⭐

**Transition depuis RTOS :**

Si vous avez utilisé FreeRTOS :
```c
// FreeRTOS
xTaskCreate(task_function, "Task", 128, NULL, 1, &taskHandle);
```

Sous Linux avec pthreads :
```c
// POSIX threads
pthread_t thread;  
pthread_create(&thread, NULL, thread_function, NULL);  
```

**Sections essentielles :**
- 18.1 Introduction à pthread
- 18.2 Création et terminaison de threads
- 18.6 Mutex et sections critiques
- 18.8 Variables de condition

**Attention :** Les threads Linux ne sont PAS temps-réel par défaut. Si vous avez besoin de temps-réel, vous devrez utiliser PREEMPT_RT ou configurer les priorités avec `sched_setscheduler()`.

---

### Phase 4 : Cross-Compilation et Outils Embarqués (3-4 semaines)

#### 🔴 Module 8 - Chapitre 31 : Cross-compilation et Déploiement *(Priorité : CRITIQUE)* 🔥

**Vous connaissez déjà la cross-compilation, mais sous Linux c'est plus sophistiqué.**

**31.1 Cross-compilation : principes**
- Différence host vs target
- Toolchain (compiler, linker, libraries)

**31.2 Compiler pour ARM depuis x86** ⭐⭐⭐

**Exemple concret :**
```bash
# Installer la toolchain ARM
sudo apt install gcc-arm-linux-gnueabihf

# Compiler pour ARM 32 bits
arm-linux-gnueabihf-gcc -o app app.c

# Vérifier
file app
# app: ELF 32-bit LSB executable, ARM, version 1 (SYSV)
```

**31.3 CMake et cross-compilation** ⭐

Créer un fichier toolchain CMake :
```cmake
# arm-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)  
set(CMAKE_SYSTEM_PROCESSOR arm)  
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)  
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)  
```

Puis compiler :
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake ..  
make  
```

**31.4 QEMU pour tester les binaires** ⭐⭐⭐

**Outil magique pour l'embarqué :**

QEMU vous permet d'exécuter des binaires ARM sur votre PC x86 :
```bash
# Installer QEMU
sudo apt install qemu-user-static

# Exécuter un binaire ARM sur x86
qemu-arm-static ./app
```

Vous pouvez même émuler un système complet :
```bash
# Émuler un Raspberry Pi
qemu-system-arm -M versatilepb -kernel kernel.img -append "root=/dev/sda2"
```

**31.8 Conteneurisation : Docker multi-stage** ⭐

Créer des images Docker pour votre application embarquée :
```dockerfile
# Stage 1 : Build
FROM arm32v7/ubuntu:22.04 as builder  
RUN apt-get update && apt-get install -y build-essential cmake  
COPY . /app  
WORKDIR /app  
RUN cmake . && make  

# Stage 2 : Runtime minimal
FROM arm32v7/ubuntu:22.04  
COPY --from=builder /app/mon_app /usr/bin/  
CMD ["/usr/bin/mon_app"]  
```

---

#### 🟡 Module 8 - Chapitres 29-30 : Tests et CI/CD *(Priorité : MOYENNE)*

**Différence avec le développement embarqué classique :**

En bare-metal, les tests sont souvent :
- Tests sur cible réelle uniquement
- Difficiles à automatiser
- Pas de CI/CD

En Linux embarqué moderne :
- Tests unitaires sur PC (avec mocks)
- Tests d'intégration dans QEMU
- CI/CD complet (GitHub Actions, GitLab CI)

**30.1 GitHub Actions pour projets C** ⭐

Exemple de workflow pour Linux embarqué :
```yaml
name: Build ARM

on: [push]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install ARM toolchain
        run: sudo apt-get install -y gcc-arm-linux-gnueabihf
      - name: Build
        run: |
          mkdir build && cd build
          cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake ..
          make
      - name: Test in QEMU
        run: qemu-arm-static ./build/mon_app
```

---

### Phase 5 : Drivers Linux et Noyau (4-6 semaines)

> **Note :** Les drivers Linux, le Device Tree, et les modules kernel ne sont pas couverts dans les chapitres de la formation principale. Cette section fournit une introduction autonome à ces sujets essentiels pour le Linux embarqué. Pour approfondir, consultez les livres recommandés en fin de parcours (notamment "Linux Device Drivers" et "Mastering Embedded Linux Programming").

#### 🔴 Drivers et Device Tree *(Priorité : CRITIQUE pour l'embarqué)* 🔥🔥🔥

**C'est ici que ça devient passionnant pour un développeur embedded !**

**Concepts à maîtriser :**

**1. Device Tree** ⭐⭐⭐

En bare-metal, vous configurez les périphériques en C :
```c
// Configuration UART en bare-metal
UART1->BRR = 9600;  
UART1->CR1 |= USART_CR1_UE;  
```

Sous Linux, on utilise le Device Tree (.dts/.dtb) :
```dts
&uart1 {
    status = "okay";
    pinctrl-names = "default";
    pinctrl-0 = <&uart1_pins>;
    current-speed = <115200>;
};
```

Le Device Tree décrit le hardware au noyau sans recompiler.

**2. Modules Kernel (Drivers)** ⭐⭐⭐

**Accès hardware sous Linux :**

Vous ne pouvez plus accéder directement aux registres depuis l'espace utilisateur. Vous devez créer un driver noyau.

**Structure minimale d'un driver :**
```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init mon_driver_init(void)
{
    printk(KERN_INFO "Mon driver initialisé\n");
    return 0;
}

static void __exit mon_driver_exit(void)
{
    printk(KERN_INFO "Mon driver déchargé\n");
}

module_init(mon_driver_init);  
module_exit(mon_driver_exit);  

MODULE_LICENSE("GPL");  
MODULE_AUTHOR("Votre Nom");  
MODULE_DESCRIPTION("Un driver exemple");  
```

**Compilation du module :**
```makefile
obj-m += mon_driver.o

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

**Chargement du module :**
```bash
sudo insmod mon_driver.ko  
dmesg | tail  # Voir les messages du driver  
sudo rmmod mon_driver  
```

**3. GPIO, I2C, SPI sous Linux** ⭐⭐⭐

**GPIO :**
- Via sysfs : `/sys/class/gpio/`
- Via libgpiod (API moderne)
- Via ioctl sur `/dev/gpiochip*`

**I2C :**
- Via `i2c-dev` : `/dev/i2c-1`
- API ioctl standard

**SPI :**
- Via `spidev` : `/dev/spidev0.0`
- API ioctl standard

**Exemple I2C :**
```c
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int file = open("/dev/i2c-1", O_RDWR);  
ioctl(file, I2C_SLAVE, 0x48);  // Adresse device  

uint8_t reg = 0x00;  
write(file, &reg, 1);  

uint8_t data[2];  
read(file, data, 2);  

close(file);
```

**Ressources pour approfondir :**
- "Linux Device Drivers" (LDD3) - le livre de référence
- Kernel documentation : `Documentation/driver-api/`
- Exemples dans `drivers/` du kernel Linux

---

### Phase 6 : Construction de Distributions Embarquées (3-4 semaines)

> **Note :** Yocto et Buildroot ne sont pas couverts dans les chapitres de la formation principale. Cette section fournit une introduction pratique à ces outils de build essentiels en Linux embarqué.

#### 🔴 Yocto et Buildroot *(Priorité : HAUTE)* 🔥

**Au-delà du développement d'applications, vous devez créer le système complet.**

**1. Buildroot** ⭐⭐ (Plus simple, idéal pour débuter)

Buildroot génère une distribution Linux embarquée complète.

**Workflow typique :**
```bash
# Télécharger Buildroot
git clone https://git.buildroot.net/buildroot  
cd buildroot  

# Configurer pour Raspberry Pi 3
make raspberrypi3_64_defconfig

# Personnaliser
make menuconfig

# Build (peut prendre 1-2h la première fois)
make

# Résultat : image SD card prête
ls output/images/sdcard.img
```

**Ce que Buildroot génère :**
- Bootloader (U-Boot)
- Kernel Linux compilé
- Root filesystem avec vos applications
- Toolchain pour cross-compiler

**2. Yocto Project** ⭐⭐⭐ (Plus puissant, standard industriel)

Yocto est plus complexe mais beaucoup plus flexible.

**Concepts Yocto :**
- **Layers** : modules réutilisables (meta-raspberrypi, meta-ti...)
- **Recipes** : instructions de build (fichiers .bb)
- **BitBake** : moteur de build

**Exemple simple :**
```bash
# Cloner Poky (référence Yocto)
git clone git://git.yoctoproject.org/poky  
cd poky  
source oe-init-build-env  

# Éditer conf/local.conf
echo 'MACHINE = "raspberrypi3-64"' >> conf/local.conf

# Build (très long : 2-4h+)
bitbake core-image-minimal
```

**Ajout de votre application :**
```bash
# Créer une recipe
meta-custom/recipes-app/mon-app/mon-app_1.0.bb
```

**Différence Buildroot vs Yocto :**
- **Buildroot** : Simple, rapide, config monolithique
- **Yocto** : Complexe, modulaire, industriel, maintenable à long terme

**Pour débuter :** Commencez par Buildroot  
**Pour la production :** Passez à Yocto  

---

### Phase 7 : Optimisation Embarquée (2-3 semaines)

#### 🟡 Module 7 - Chapitre 27 : Optimisation et Performance

**Contraintes spécifiques à l'embarqué :**

**1. Taille du binaire** ⭐⭐⭐

**Techniques d'optimisation :**
```bash
# Compiler avec -Os (optimize for size)
arm-linux-gnueabihf-gcc -Os -o app app.c

# Stripper les symboles
arm-linux-gnueabihf-strip app

# Taille avant/après
ls -lh app
```

**Options CMake :**
```cmake
set(CMAKE_BUILD_TYPE MinSizeRel)  
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -ffunction-sections -fdata-sections")  
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")  
```

**2. Boot time** ⭐⭐⭐

**Objectif :** Réduire le temps de démarrage

**Techniques :**
- Kernel minimal (désactiver drivers inutiles)
- Init rapide (remplacer systemd par busybox init)
- Compression (LZO vs gzip)
- DMA et accès mémoire optimisés

**Exemple de kernel minimal :**
```bash
# Configuration kernel
make menuconfig
# Désactiver : USB, son, drivers réseau inutiles, debug symbols...
```

**3. Consommation mémoire** ⭐⭐

**Profiling mémoire :**
```bash
# Voir l'utilisation mémoire
free -h  
cat /proc/meminfo  

# Par processus
ps aux --sort=-rss | head

# Avec Valgrind (sur PC ou QEMU)
valgrind --tool=massif ./app  
ms_print massif.out.12345  
```

**4. Consommation électrique** ⭐

**Techniques :**
- CPU frequency scaling (cpufreq)
- Suspend/resume
- Power management dans les drivers

```bash
# Voir les frequencies disponibles
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies

# Mettre en mode économie
echo powersave > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
```

---

### Phase 8 : Débogage Embarqué Avancé (2-3 semaines)

#### 🔴 Module 5 - Chapitre 15 : Débogage *(Priorité : CRITIQUE)* 🔥

**1. GDB avec gdbserver** ⭐⭐⭐

**Débogage à distance :**

Sur la cible (Raspberry Pi, BeagleBone...) :
```bash
# Sur la cible
gdbserver :1234 ./mon_app
```

Sur votre PC :
```bash
# Sur le PC
arm-linux-gnueabihf-gdb mon_app
(gdb) target remote 192.168.1.100:1234
(gdb) break main
(gdb) continue
```

**2. JTAG avec OpenOCD** ⭐⭐

Pour le débogage hardware-level :
```bash
# OpenOCD pour ARM
openocd -f interface/jlink.cfg -f target/stm32f4x.cfg

# Dans un autre terminal
arm-none-eabi-gdb
(gdb) target remote :3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

**3. Tracing avec ftrace** ⭐

Le kernel Linux inclut un système de tracing puissant :
```bash
# Activer ftrace
echo 1 > /sys/kernel/debug/tracing/tracing_on

# Tracer les appels système
echo 1 > /sys/kernel/debug/tracing/events/syscalls/enable

# Voir les traces
cat /sys/kernel/debug/tracing/trace
```

**4. eBPF pour l'embarqué** ⭐⭐

**Module 6 - Chapitre 21 : Introduction à eBPF**

eBPF fonctionne aussi sur ARM/Linux embarqué !

**Use cases embarqués :**
- Tracer les appels système sans overhead
- Monitorer les performances I/O
- Analyser le comportement réseau
- Profiling sans instrumentation

**Exemple simple :**
```bash
# Installer bpftrace (si disponible sur la cible)
apt install bpftrace

# Tracer les open()
bpftrace -e 'tracepoint:syscalls:sys_enter_open { printf("%s %s\n", comm, str(args->filename)); }'
```

---

## 📊 Chronologie Recommandée

### Parcours Standard (20-24 semaines)

**Semaines 1-3 : Mise à niveau et environnement**
- Module 1 : Écosystème C Linux
- Module 2 : Révision rapide
- Section 2.5 : DevContainers
- Chapitre 13 : CMake

**Semaines 4-7 : Programmation système Linux**
- Module 6 - Chapitre 16 : Fichiers et I/O
- Module 6 - Chapitre 17 : Processus et signaux
- Module 6 - Chapitre 20 : Sockets (bases)
- **Projet :** Application Linux basique (monitoring, logger...)

**Semaines 8-11 : Cross-compilation et outils**
- Module 8 - Chapitre 31 : Cross-compilation
- QEMU et test d'applications
- GitHub Actions pour ARM
- **Projet :** Pipeline CI/CD pour ARM

**Semaines 12-17 : Drivers et kernel (CRUCIAL)**
- Device Tree (documentation kernel)
- Modules kernel basiques
- Drivers GPIO, I2C, SPI
- **Projet :** Driver simple pour un périphérique

**Semaines 18-21 : Distributions embarquées**
- Buildroot : premiers pas
- Yocto : introduction
- Personnalisation du kernel
- **Projet :** Distribution custom pour Raspberry Pi

**Semaines 22-24 : Optimisation et finalisation**
- Optimisations (taille, boot time, mémoire)
- Débogage avancé (gdbserver, JTAG)
- eBPF pour l'embarqué
- **Projet final :** Voir section ci-dessous

---

### Parcours Accéléré (14-16 semaines)

Pour ceux qui peuvent y consacrer 20h+/semaine :

**Semaines 1-2 :** Fondations et environnement  
**Semaines 3-5 :** Programmation système  
**Semaines 6-8 :** Cross-compilation et CI/CD  
**Semaines 9-12 :** Drivers et kernel (focus intense)  
**Semaines 13-14 :** Buildroot ou Yocto  
**Semaines 15-16 :** Projet final  

---

## 🎓 Compétences Acquises

### Avant cette formation

- ✓ Vous développez en C pour microcontrôleurs
- ✓ Vous accédez directement aux registres hardware
- ✓ Vous utilisez des IDE propriétaires
- ✓ Vous gérez tout manuellement (pas d'OS)
- ✓ Vous compilez pour une cible spécifique

**Mais :**
- ❌ Pas de multitâche avancé
- ❌ Pas de networking complet
- ❌ Pas de système de fichiers
- ❌ Outils de debug limités
- ❌ Écosystème logiciel restreint

---

### Après cette formation

✅ **Architecture Linux embarqué**
- Bootloader, kernel, rootfs, applications
- Device Tree et configuration hardware
- MMU et mémoire virtuelle

✅ **Développement d'applications Linux**
- Programmation système POSIX
- Networking complet (TCP/IP, sockets)
- Multithreading et IPC
- Système de fichiers

✅ **Drivers Linux**
- Modules kernel
- Device Tree bindings
- Drivers GPIO, I2C, SPI, UART
- Platform drivers

✅ **Cross-compilation et build systems**
- Toolchains ARM, ARM64, RISC-V
- CMake pour cross-compilation
- QEMU pour émulation
- CI/CD pour embarqué

✅ **Distributions personnalisées**
- Buildroot pour systèmes simples
- Yocto pour production industrielle
- Customisation du kernel Linux
- Optimisation boot time et taille

✅ **Débogage avancé**
- GDB avec gdbserver
- JTAG et OpenOCD
- ftrace et perf
- eBPF pour le tracing

---

## 💼 Débouchés Professionnels

Cette formation vous ouvre les portes vers :

**Postes accessibles :**
- **Ingénieur Linux Embarqué**
- **Développeur de Drivers Linux**
- **Ingénieur BSP (Board Support Package)**
- **Ingénieur Yocto / Buildroot**
- **Architecte Systèmes Embarqués**
- **Ingénieur IoT (Edge Computing)**

**Domaines d'application :**
- Automotive (infotainment, ADAS)
- Industrie 4.0 (contrôleurs, HMI)
- IoT et Edge Computing (gateways)
- Télécommunications (équipements réseau)
- Médical (dispositifs connectés)
- Défense et aérospatial

**Évolution salariale typique :**
- Développeur embedded (bare-metal) : 35-50k€
- Ingénieur Linux Embarqué : 45-65k€
- Ingénieur BSP / Driver : 50-75k€
- Architecte Systèmes Embarqués : 60-85k€+

**Avantage compétitif :** La demande pour des ingénieurs Linux embarqué dépasse largement l'offre. C'est un skillset rare et très valorisé.

---

## 🛠️ Matériel Recommandé

### Pour Débuter (Budget : 50-100€)

**Raspberry Pi 4 (4GB RAM)** ⭐⭐⭐
- Parfait pour débuter
- Support communautaire énorme
- Compatible Yocto et Buildroot
- GPIO, I2C, SPI, UART disponibles

**Alternative :** BeagleBone Black (plus orienté industriel)

### Pour Approfondir (Budget : 100-200€)

**Carte de développement i.MX ou SAMA5D**
- Plus proche de l'industriel
- Meilleur support Device Tree
- Interfaces industrielles (CAN, etc.)

**Adaptateur JTAG** (J-Link EDU, ST-Link)
- Pour le débogage hardware-level

### Outils Logiciels (Gratuits)

✓ QEMU (émulation)
✓ GDB multiarch
✓ VS Code + extensions C/C++
✓ Docker (pour DevContainers)
✓ Cross-toolchains (arm-linux-gnueabihf, aarch64-linux-gnu)

---

## 🚀 Projet de Validation de Compétences

### **Projet Final : Gateway IoT Industrielle**

**Objectif :** Créer un système complet de passerelle IoT avec distribution Linux personnalisée.

**Hardware :** Raspberry Pi 4 (ou équivalent)

**Fonctionnalités :**

**1. Distribution custom avec Buildroot ou Yocto**
- Kernel Linux optimisé (boot time < 10s)
- Root filesystem minimal
- Support GPIO, I2C, SPI
- Networking (Ethernet, WiFi optionnel)

**2. Application de collecte de données**
- Lecture de capteurs via I2C/SPI
- Driver kernel custom pour un périphérique
- Stockage dans SQLite
- Envoi via MQTT vers broker

**3. Interface utilisateur**
- Interface web simple (serveur HTTP en C)
- API REST pour configuration
- Logs système accessible

**4. Sécurité**
- OTA updates (partitions A/B)
- Authentification
- Chiffrement des communications

**5. Monitoring et debug**
- Programme eBPF pour tracer les appels I2C
- Métriques système exportées
- Core dumps configurés

**6. CI/CD complet**
- GitHub Actions pour build ARM
- Tests dans QEMU
- Génération d'image SD prête à flasher
- Documentation automatique

**Compétences validées :**
- ✅ Buildroot ou Yocto
- ✅ Device Tree
- ✅ Driver kernel
- ✅ Cross-compilation
- ✅ Programmation système
- ✅ Networking embarqué
- ✅ eBPF
- ✅ Optimisation embarquée
- ✅ CI/CD

**Durée estimée :** 3-4 semaines

**Bonus :** Ajoutez un système de mise à jour OTA sécurisé (SWUpdate, Mender...)

---

## 📚 Ressources Complémentaires Embarquées

### Livres Essentiels

1. **"Mastering Embedded Linux Programming"** par Chris Simmonds ⭐⭐⭐
   - LE livre pour Linux embarqué
   - Couvre Yocto, Buildroot, drivers, Device Tree

2. **"Linux Device Drivers" (LDD3)** par Corbet, Rubini, Kroah-Hartman
   - La référence pour les drivers
   - Un peu vieux mais toujours pertinent

3. **"Embedded Linux Primer"** par Christopher Hallinan
   - Excellent pour débuter

4. **"Linux Kernel Development"** par Robert Love
   - Pour comprendre l'interne du kernel

### Sites et Ressources

**Documentation officielle :**
- Yocto Project : docs.yoctoproject.org
- Buildroot : buildroot.org/docs.html
- Kernel.org : Documentation/

**Communautés :**
- eLinux.org : wiki collaboratif
- Bootlin (ex-Free Electrons) : formations gratuites
- Raspberry Pi forums

**Chaînes YouTube :**
- Bootlin
- Chris Simmonds (Embedded Linux)

---

## 🎯 Conseils pour Réussir

### 1. **Utilisez votre expérience embedded**

Vous avez déjà l'essentiel : la compréhension du hardware. Faites des parallèles :
- GPIO en bare-metal → GPIO via sysfs/libgpiod sous Linux
- Timer interrupt → Threads avec sleep() sous Linux
- UART polling → read() bloquant sous Linux
- FreeRTOS tasks → pthreads sous Linux

### 2. **Commencez par Raspberry Pi**

Le Raspberry Pi est idéal pour débuter :
- Énorme communauté
- Documentation complète
- Peu de risque de bricker
- Compatible avec tout (Yocto, Buildroot...)

### 3. **Maîtrisez QEMU tôt**

QEMU accélère énormément le développement :
- Pas besoin de flasher à chaque test
- Debugging plus rapide
- Émulation de différentes architectures

```bash
# Tester votre appli ARM sur PC
qemu-arm-static -L /usr/arm-linux-gnueabihf/ ./mon_app
```

### 4. **Apprenez par la pratique**

Pour chaque concept :
1. Lisez la théorie
2. Écrivez un mini-programme de test
3. Testez sur QEMU
4. Testez sur cible réelle
5. Intégrez dans un projet plus grand

### 5. **Contribuez à l'écosystème**

Une fois à l'aise :
- Corrigez des bugs dans Buildroot
- Ajoutez des packages Yocto
- Améliorez la documentation
- Partagez vos Device Tree bindings

### 6. **Comprenez le Device Tree**

Le Device Tree est crucial. Investissez du temps pour le comprendre :
```bash
# Décompiler un DTB pour l'étudier
dtc -I dtb -O dts -o mon_device_tree.dts mon_device_tree.dtb

# Regardez les exemples dans le kernel
ls arch/arm/boot/dts/
```

---

## ✅ Checklist de Progression

### Semaines 1-3 : Fondations
- [ ] J'ai installé une toolchain ARM
- [ ] J'ai cross-compilé mon premier programme
- [ ] J'ai testé dans QEMU
- [ ] J'ai écrit un Makefile / CMakeLists.txt

### Semaines 4-7 : Système
- [ ] Je comprends les descripteurs de fichiers
- [ ] J'ai écrit un programme avec fork/exec
- [ ] J'ai créé un serveur socket TCP simple
- [ ] J'ai interfacé avec sysfs (/sys/class/gpio)

### Semaines 8-11 : Cross-compilation
- [ ] J'ai un pipeline CI/CD qui build pour ARM
- [ ] J'ai testé mon code dans QEMU automatiquement
- [ ] J'ai créé une image Docker pour ma toolchain

### Semaines 12-17 : Drivers et kernel
- [ ] J'ai compilé le kernel Linux
- [ ] J'ai écrit un module kernel "Hello World"
- [ ] J'ai modifié un Device Tree
- [ ] J'ai accédé à un GPIO depuis un driver

### Semaines 18-21 : Distributions
- [ ] J'ai généré une image Buildroot
- [ ] J'ai booté mon image sur Raspberry Pi
- [ ] J'ai personnalisé le kernel
- [ ] J'ai ajouté ma propre application

### Semaines 22-24 : Finalisation
- [ ] J'ai optimisé le boot time (< 10s)
- [ ] J'ai utilisé gdbserver pour déboguer
- [ ] J'ai écrit un outil eBPF de monitoring
- [ ] J'ai terminé mon projet final

---

## 🎉 Conclusion

Cette formation transforme un développeur embedded bare-metal en ingénieur Linux embarqué capable de :
- **Maîtriser** l'écosystème Linux embarqué
- **Développer** des applications système complexes
- **Créer** des drivers et personnaliser le kernel
- **Construire** des distributions Linux sur mesure
- **Optimiser** pour les contraintes embarquées
- **Déboguer** au niveau le plus bas

**Votre avantage unique :** Vous combinez la connaissance du hardware (que beaucoup de développeurs Linux n'ont pas) avec la puissance de Linux (que beaucoup de développeurs embedded n'ont pas). C'est une combinaison rare et extrêmement valorisée.

**Durée réaliste :** 20-24 semaines à raison de 10-15h/semaine. Mais chaque semaine vous rapproche d'une expertise qui ouvre des portes vers des projets passionnants et des salaires élevés.

**Premier pas :** Cette semaine, installez une toolchain ARM, cross-compilez un "Hello World", et testez-le dans QEMU. C'est parti pour l'aventure Linux embarqué ! 🚀

---


⏭️ [Débutant Complet → Ingénieur C Full-Stack](/parcours-recommandes/04-debutant-vers-fullstack.md)
