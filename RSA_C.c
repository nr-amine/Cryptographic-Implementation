#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

// convenient variables
long long p, q, n_mod, phi_rsa, e_key, d_key;

//Helper functions

long long mult_mod(long long a, long long b, long long m) {
    long long result = (a % m * b % m) % m;
    if (result < 0) return result + m;
    return result;
}

long long pow_mod(long long a, long long k, long long m) {
    if (k == 0) return 1;
    if (k == 1) return a % m;
    
    long long half = pow_mod(a, k / 2, m);
    long long half_sq = mult_mod(half, half, m);
    
    if (k % 2 == 0) {
        return half_sq;
    } else {
        return mult_mod(half_sq, a, m);
    }
}

void bezout(long long a, long long b, long long *d, long long *u, long long *v) {
    if (b == 0) {
        *d = a;
        *u = 1;
        *v = 0;
    } else {
        long long d1, u1, v1;
        bezout(b, a % b, &d1, &u1, &v1);
        *d = d1;
        *u = v1;
        *v = u1 - (a / b) * v1;
    }
}

long long inv(long long a, long long zn) {
    long long d, u, v;
    bezout(a, zn, &d, &u, &v);
    if (d != 1) {
        printf("Erreur: a n'est pas inversible modulo zn\n");
        return -1; 
    }
    return (u % zn + zn) % zn;
}

int is_prime(long long n) {
    if (n < 2) return 0;
    long long limit = (long long)sqrt((double)n);
    for (long long i = 2; i <= limit; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

long long random_number_gen() {
    return (rand() % 20000) + 20000;
}

long long prime_gen(long long bound) {
    while (1) {
        long long rndt = random_number_gen();
        if (rndt <= bound && is_prime(rndt)) {
            return rndt;
        }
    }
}

void generate_keys() {
    long long bound = 99999;
    p = prime_gen(bound);
    do {
        q = prime_gen(bound);
    } while (q == p);

    n_mod = p * q;
    phi_rsa = (p - 1) * (q - 1);
    e_key = 65537;
    d_key = inv(e_key, phi_rsa);
}

int char_to_code(char c) {
    int ascii = (int)c;
    if (ascii >= 97 && ascii <= 122)
        return ascii - 97;
    else if (ascii >= 65 && ascii <= 90)
        return ascii - 65 + 26;
    else {
        printf("Caractère invalide (seulement a-z et A-Z): %c\n", c);
        return -1;
    }
}

char code_to_char(int n) {
    if (n >= 0 && n <= 25)
        return (char)(n + 97);
    else if (n >= 26 && n <= 51)
        return (char)(n - 26 + 65);
    else {
        printf("Code invalide (doit être entre 0 et 51)\n");
        return '?';
    }
}

long long mot_to_int(const char *mot) {
    if (strlen(mot) != 5) {
        printf("Erreur: Le mot doit contenir exactement 5 lettres\n");
        return -1;
    }
    
    long long acc = 0;
    for (int i = 0; i < 5; i++) {
        int code = char_to_code(mot[i]);
        if (code == -1) return -1;
        acc = acc * 52 + code;
    }
    return acc;
}

void int_to_mot(long long n, char *buffer) {
    for (int i = 4; i >= 0; i--) {
        int code = n % 52;
        buffer[i] = code_to_char(code);
        n = n / 52;
    }
    buffer[5] = '\0';
}

//User Interface

void afficher_cles() {
    printf("\n=== Clés RSA générées ===\n");
    printf("Nombre premier p: %lld\n", p);
    printf("Nombre premier q: %lld\n", q);
    printf("Modulo n = p×q: %lld\n", n_mod);
    printf("φ(n) = (p-1)×(q-1): %lld\n", phi_rsa);
    printf("Clé publique (e, n): (%lld, %lld)\n", e_key, n_mod);
    printf("Clé privée (d, n): (%lld, %lld)\n", d_key, n_mod);
    printf("========================\n\n");
}

long long coder_mot(const char *mot) {
    long long m = mot_to_int(mot);
    if (m == -1) return 0;
    
    long long c = pow_mod(m, e_key, n_mod);
    printf("Mot '%s' → Code numérique: %lld → Chiffré: %lld\n", mot, m, c);
    return c;
}

void decoder_nombre(long long c) {
    long long m = pow_mod(c, d_key, n_mod);
    char mot[6];
    int_to_mot(m, mot);
    printf("Chiffré %lld → Code numérique: %lld → Mot: '%s'\n", c, m, mot);
}

void menu_principal() {
    printf("\n╔════════════════════════════════════╗\n");
    printf("║   Système de cryptage RSA          ║\n");
    printf("╚════════════════════════════════════╝\n");
    printf("1. Afficher les clés RSA\n");
    printf("2. Coder un mot (5 lettres)\n");
    printf("3. Décoder un nombre\n");
    printf("4. Test complet (codage + décodage)\n");
    printf("5. Régénérer de nouvelles clés\n");
    printf("6. Quitter\n");
    printf("Votre choix: ");
}

void clean_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Helper to safely read a string
void read_string(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        } else {
            clean_stdin();
        }
    }
}

void test_complet() {
    printf("\n=== Test complet ===\n");
    printf("Entrez un mot de 5 lettres: ");
    char mot[10];
    read_string(mot, sizeof(mot));
    
    if (strlen(mot) != 5) {
        printf("Erreur: Le mot doit contenir exactement 5 lettres\n");
    } else {
        printf("\n--- Codage ---\n");
        long long c = coder_mot(mot);
        printf("\n--- Décodage ---\n");
        decoder_nombre(c);
        printf("\n✓ Test réussi!\n");
    }
}

int main() {
    srand(time(NULL)); 
    generate_keys();
    
    printf("\n");
    printf("╔════════════════════════════════════╗\n");
    printf("║  Bienvenue dans le système RSA!    ║\n");
    printf("╚════════════════════════════════════╝\n");
    afficher_cles();
    
    int running = 1;
    while (running) {
        menu_principal();
        
        int choix;
        if (scanf("%d", &choix) != 1) {
            printf("Erreur de saisie. Réessayez.\n");
            clean_stdin();
            continue;
        }
        clean_stdin();
        
        switch (choix) {
            case 1:
                afficher_cles();
                break;
            case 2: {
                printf("Entrez un mot de 5 lettres: ");
                char mot[10];
                read_string(mot, sizeof(mot));
                coder_mot(mot);
                break;
            }
            case 3: {
                printf("Entrez le nombre à décoder: ");
                long long c;
                if (scanf("%lld", &c) == 1) {
                    clean_stdin(); 
                    decoder_nombre(c);
                } else {
                    printf("Entrée invalide.\n");
                    clean_stdin();
                }
                break;
            }
            case 4:
                test_complet();
                break;
            case 5:
                printf("\n=== Génération de nouvelles clés ===\n");
                generate_keys();
                printf("Nouvelles clés générées!\n");
                break;
            case 6:
                printf("\nAu revoir!\n");
                running = 0;
                break;
            default:
                printf("Choix invalide. Réessayez.\n");
        }
    }
    
    return 0;
}