//
//  main.c
//  ödev 11
//
//  Created by Mevsim Kuru on 26.11.2026.
//
#include <stdio.h>

/* Mevsim: Gantt semasini yazdirir ve baglam degisimini sayar.
   Sifirla parametresi 1 ise yeni algoritma icin sayaclari sifirlar. */
void gantt(int sifirla, int p, long long bas, long long son, long long *degisim)
{
    static int sonSurec = 0;
    static int satirBlok = 0;
    if (sifirla) {
        sonSurec = 0;
        satirBlok = 0;
        *degisim = 0;
        return;
    }
    if (satirBlok == 6) {
        printf("\n");
        satirBlok = 0;
    }
    if (p == 0)
        printf("[BOS %lld-%lld] ", bas, son);
    else {
        printf("[P%d %lld-%lld] ", p, bas, son);
        if (sonSurec != 0 && sonSurec != p)
            (*degisim)++;
        sonSurec = p;
    }
    satirBlok++;
}

/* Busra: Bitis zamanlarini ve Gantt sayaclarini sifirlar. */
void sifirla(int n, long long bitis[], long long *degisim)
{
    int i;
    for (i = 0; i < n; i++)
        bitis[i] = 0;
    gantt(1, 0, 0, 0, degisim);
}

/* MEVSIM - 1. ALGORITMA: FCFS
   Bitmemis surecleri gelis zamanina ve numarasina gore calistirir. */
void fcfs(int n, int gelis[], int cpu[], long long bitis[], long long *degisim)
{
    int i, j, sec;
    long long saat = 0;
    sifirla(n, bitis, degisim);
    for (j = 0; j < n; j++) {
        sec = -1;
        for (i = 0; i < n; i++) {
            if (bitis[i] == 0 && (sec == -1 || gelis[i] < gelis[sec]))
                sec = i;
        }
        if (saat < gelis[sec]) {
            gantt(0, 0, saat, gelis[sec], degisim);
            saat = gelis[sec];
        }
        gantt(0, sec + 1, saat, saat + cpu[sec], degisim);
        saat += cpu[sec];
        bitis[sec] = saat;
    }
}

/* BUSRA - 2. ALGORITMA: SJF
   Gelmis sureclerden CPU suresi en kisa olani kesintisiz calistirir. */
void sjf(int n, int gelis[], int cpu[], long long bitis[], long long *degisim)
{
    int i, sec, erken, tamam = 0;
    long long saat = 0;
    sifirla(n, bitis, degisim);
    while (tamam < n) {
        sec = -1;
        erken = -1;
        for (i = 0; i < n; i++) {
            if (bitis[i] != 0) continue;
            if (erken == -1 || gelis[i] < gelis[erken]) erken = i;
            if (gelis[i] <= saat) {
                if (sec == -1 || cpu[i] < cpu[sec] ||
                    (cpu[i] == cpu[sec] && gelis[i] < gelis[sec]))
                    sec = i;
            }
        }
        if (sec == -1) {
            gantt(0, 0, saat, gelis[erken], degisim);
            saat = gelis[erken];
        } else {
            gantt(0, sec + 1, saat, saat + cpu[sec], degisim);
            saat += cpu[sec];
            bitis[sec] = saat;
            tamam++;
        }
    }
}

/* MEVSIM VE BUSRA - 3. ALGORITMA: ROUND ROBIN
   Surecleri P1'den Pn'ye tur tabanli olarak calistirir. */
void roundRobin(int n, int gelis[], int cpu[], int quantum,
                long long bitis[], long long *degisim)
{
    int kalan[n];
    int i, sure, calisti, erken, tamam = 0;
    long long saat = 0;
    /* Mevsim: Baslangic ve tur icinde sureclerin calistirilmasi. */
    sifirla(n, bitis, degisim);
    for (i = 0; i < n; i++) kalan[i] = cpu[i];
    while (tamam < n) {
        calisti = 0;
        for (i = 0; i < n; i++) {
            if (kalan[i] > 0 && gelis[i] <= saat) {
                sure = quantum;
                if (kalan[i] < sure) sure = kalan[i];
                gantt(0, i + 1, saat, saat + sure, degisim);
                saat += sure;
                kalan[i] -= sure;
                calisti = 1;
                /* Busra: Bitis kontrolu ve bos kalan turun yonetimi. */
                if (kalan[i] == 0) {
                    bitis[i] = saat;
                    tamam++;
                }
            }
        }
        if (!calisti) {
            erken = -1;
            for (i = 0; i < n; i++) {
                if (kalan[i] > 0 && (erken == -1 || gelis[i] < gelis[erken]))
                    erken = i;
            }
            gantt(0, 0, saat, gelis[erken], degisim);
            saat = gelis[erken];
        }
    }
}

/* Busra: Donus ve bekleme surelerini hesaplar. */
void hesapla(int n, int gelis[], int cpu[], long long bitis[],
             long long donus[], long long bekleme[])
{
    int i;
    for (i = 0; i < n; i++) {
        donus[i] = bitis[i] - gelis[i];
        bekleme[i] = donus[i] - cpu[i];
    }
}

/* Mevsim: Iki ortalamayi pointer parametreleriyle dondurur. */
void ortalamalar(int n, long long donus[], long long bekleme[],
                 double *ortDonus, double *ortBekleme)
{
    int i;
    *ortDonus = 0;
    *ortBekleme = 0;
    for (i = 0; i < n; i++) {
        *ortDonus += (double)donus[i] / n;
        *ortBekleme += (double)bekleme[i] / n;
    }
}

/* Busra ve Mevsim: Sonuc tablosunu ve ozeti yazdirir. */
void sonuclar(int n, int gelis[], int cpu[], long long bitis[],
              long long degisim, double *ortBekleme, long double *toplamBekleme)
{
    int i;
    long long donus[n], bekleme[n];
    double ortDonus;
    hesapla(n, gelis, cpu, bitis, donus, bekleme);
    ortalamalar(n, donus, bekleme, &ortDonus, ortBekleme);
    *toplamBekleme = 0;
    printf("\n\nSurec\tGelis\tCPU\tBitis\tDonus\tBekleme\n");
    for (i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%lld\t%lld\t%lld\n", i + 1, gelis[i], cpu[i],
               bitis[i], donus[i], bekleme[i]);
        *toplamBekleme += bekleme[i];
    }
    printf("Ortalama bekleme: %.2f\n", *ortBekleme);
    printf("Ortalama donus: %.2f\n", ortDonus);
    printf("Baglam degisimi: %lld\n", degisim);
}

/* Busra ve Mevsim: Ortalama beklemeleri karsilastirir. */
void karsilastir(double f, double s, double r, long double tf, long double ts, long double tr)
{
    printf("\nKARSILASTIRMA\nFCFS: %.2f | SJF: %.2f | Round Robin: %.2f\n", f, s, r);
    if (tf <= ts && tf <= tr) printf("En dusuk: FCFS\n");
    else if (ts <= tr) printf("En dusuk: SJF\n");
    else printf("En dusuk: Round Robin\n");
}

/* Busra ve Mevsim: Verileri okur ve uc algoritmayi sirayla calistirir. */
int main(void)
{
    int n, i, quantum;
    long long degisim;
    double f, s, r;
    long double tf, ts, tr;
    printf("Surec sayisi: ");
    if (scanf("%d", &n) != 1 || n <= 0) return 1;
    int gelis[n], cpu[n];
    long long bitis[n];
    for (i = 0; i < n; i++) {
        printf("P%d gelis zamani: ", i + 1);
        if (scanf("%d", &gelis[i]) != 1 || gelis[i] < 0) return 1;
        printf("P%d CPU suresi: ", i + 1);
        if (scanf("%d", &cpu[i]) != 1 || cpu[i] <= 0) return 1;
    }
    printf("Quantum: ");
    if (scanf("%d", &quantum) != 1 || quantum <= 0) return 1;
    printf("\nFCFS\nGantt: ");
    fcfs(n, gelis, cpu, bitis, &degisim);
    sonuclar(n, gelis, cpu, bitis, degisim, &f, &tf);
    printf("\nSJF\nGantt: ");
    sjf(n, gelis, cpu, bitis, &degisim);
    sonuclar(n, gelis, cpu, bitis, degisim, &s, &ts);
    printf("\nROUND ROBIN\nGantt: ");
    roundRobin(n, gelis, cpu, quantum, bitis, &degisim);
    sonuclar(n, gelis, cpu, bitis, degisim, &r, &tr);
    karsilastir(f, s, r, tf, ts, tr);
    return 0;
}
