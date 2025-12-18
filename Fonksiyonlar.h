#include <iostream>
#include <vector>
#include <algorithm> // sort() gibi algoritmalar için
#include <iomanip> // Çıktı formatlama için (setw, fixed, setprecision)
#include <fstream>
#include <climits> // INT_MAX gibi limit değerler için
#include "Graph.h"
using namespace std;

// Denklem 1: Net Enerji = Üretim - Tüketim
double netEnerji(Graph* g, int UlkeIndex) {
    if(UlkeIndex < 0 || UlkeIndex >= g ->N) {
        return 0; // eğer indeks geçersizse 0 döndür
    }

    Ulke& ulke = g -> ulkeler[UlkeIndex]; // Ülkeye referans al (performans için)
    ulke.netEnerji = ulke.uretim - ulke.tuketim;
    return ulke.netEnerji;
}

// Denklem 2: SAtın Alma Maliyeti = Net Enerji * Enerji Satış Fiyatı * (Mesafe / 100)
double satinAlmaMaliyeti(Graph* g, int aliciIndex, int saticiIndex, double enerjiMiktari) {
    if(aliciIndex < 0 || aliciIndex >= g -> N || saticiIndex < 0 || saticiIndex >= g -> N) {
        return 0; // Geçersiz indeksler için 0 döndür
    }

    if(aliciIndex == saticiIndex) {
        return 0; // Maliyet 0 (kendinden satın almaz)
    }

    int mesafe = g -> getMesafe(aliciIndex, saticiIndex); // iki ülke arası mesafe
    if(mesafe == 0) { // bağlantı yoksa
        return INT_MAX; // Çok büyük maliyet döndür (ulaşılamaz)
        // INT_MAX, <climits> kütüphanesinden gelir, int'in maksimum değeri
    }

    Ulke& satici = g -> ulkeler[saticiIndex]; // Satıcı ülkeye referans al
    double maliyet = abs(enerjiMiktari) * satici.enerjiSatisFiyati * (mesafe / 100);
    // abs(): Mutlak değer (enerjiMiktarı negatif olabilir)
    return maliyet;
}

// Denklem 3: Ceza Maliyeti = Net Enerji * Ceza Katsayısı
double cezaMaliyeti(Graph* g, int ulkeIndex, double enerjiAcisi) {
    if(ulkeIndex < 0 || ulkeIndex >= g -> N) {
        return 0;
    }

    Ulke& ulke = g -> ulkeler[ulkeIndex];
    double ceza = abs(enerjiAcisi) * ulke.cezaKatsayisi;
    return ceza;
}

// Enerji açığı olan ülkeye en uygun çözümü bul (satın alma veya ceza)
void enerjiDengesizliginiGider(Graph* g, int ulkeIndex) {
    // 2 seçenek: 1) Diğer ülkelerden satın al, 2) Ceza öde
    if(ulkeIndex < 0 || ulkeIndex >= g -> N) {
        return;
    }

    Ulke& ulke = g -> ulkeler[ulkeIndex]; // Açığı olan ülke
    double net = netEnerji(g, ulkeIndex); // Net enerjiyi hesapla

    if(net >= 0) {
        // Enerji fazlası var, transfer edebilir
        return;
    }

    // Enerji açığı var
    double enerjiAcisi = abs(net); // Mutlak değer al
    double minToplamMaliyet = cezaMaliyeti(g, ulkeIndex, enerjiAcisi); // Varsayılan: Sadece ceza öde (en kötü seneryo)
    double toplamSatinAlmaMaliyeti = 0;
    vector<pair<int ,double>> satinAlinanUlkeler; // (Ülke index, enerji miktarı)

    // Greedy yaklaşım: Her adımda en ucuz seçeneği seç
    double kalanAcisi = enerjiAcisi;
    double toplamMaliyet = 0;

    while(kalanAcisi > 0) {
        // Tüm ülkelerden mevcut duruma göre en ucuz seçeneği bul
        double enUcuzMaliyet = INT_MAX; // Başlangıçta çok büyük değer
        int enUcuzSaticiIndex = -1; // Bulunamadığında -1
        double enUcuzEnerjiMiktari = 0;

        for(int i = 0; i < g -> N; i++) {
            if(i == ulkeIndex) {
                continue;
            }

            Ulke& potansiyelSatici = g -> ulkeler[i];
            double saticiNetEnerji = potansiyelSatici.netEnerji;

            if(saticiNetEnerji > 0) { // Bu ülkenin enerji fazlalığı var
                double kullanilabilirEnerji = min(saticiNetEnerji, kalanAcisi);
                double maliyet = satinAlmaMaliyeti(g, ulkeIndex, i, kullanilabilirEnerji);

                // daha ucuz bir seçenek bulunduysa güncelle
                if(maliyet < INT_MAX && maliyet < enUcuzMaliyet) {
                    enUcuzMaliyet = maliyet;
                    enUcuzSaticiIndex = i;
                    enUcuzEnerjiMiktari = kullanilabilirEnerji;
                }
            }
        }

        // Eğer satın alma seçeneği yoksa veya ceza daha ucuzsa dur
        if(enUcuzSaticiIndex == -1 || (toplamMaliyet + enUcuzMaliyet >= minToplamMaliyet)) {
            break;
        }

        // En ucuz seçeneği uygula
        toplamMaliyet += enUcuzMaliyet;
        kalanAcisi -= enUcuzEnerjiMiktari;
        satinAlinanUlkeler.push_back({enUcuzSaticiIndex, enUcuzEnerjiMiktari});
        toplamSatinAlmaMaliyeti += enUcuzMaliyet;

        // Satıcı ülkenin net enerjisini güncelle
        g -> ulkeler[enUcuzSaticiIndex].netEnerji -= enUcuzEnerjiMiktari;
    }

    // Kalan açık için ceza hesapla
    double kalanCeza = 0;
    if(kalanAcisi > 0) {
        kalanCeza = cezaMaliyeti(g, ulkeIndex, kalanAcisi);
    }

    // Toplam maliyeti karşılatır
    if(toplamMaliyet + kalanCeza < minToplamMaliyet) {
        ulke.satinAlmaMaliyeti = toplamSatinAlmaMaliyeti;
        ulke.cezaMaliyeti = kalanCeza;
        ulke.toplamMaliyet = toplamMaliyet + kalanCeza;
        // Alıcı ülkenin net enerjisini güncelle (artık açık yok)
        ulke.netEnerji = 0;
    }
    else {
        // sadece ceza öde
        ulke.cezaMaliyeti = minToplamMaliyet;
        ulke.satinAlmaMaliyeti = 0;
        ulke.toplamMaliyet = minToplamMaliyet;
        // Satın alınan enerjileri geri al (kullanmadık)
        for(auto& satinAlma : satinAlinanUlkeler) {
            g -> ulkeler[satinAlma.first].netEnerji += satinAlma.second;
        }
    }
}

// Tüm ülkeler için enerji dengesizliklerini gideren ana fonksiyon
void tumUlkeleriDengele(Graph* g) {
    // Önce tüm net enerjileri hesapka
    for(int i=0; i< g->N; i++) {
        netEnerji(g, i);
    }

    // Enerji fazlası olan ülkeleri önce işle (transfer edebilirler)
    vector<int> enerjiFazlasiOlanlar;
    vector<int> enerjiAcigiOlanlar;

    for(int i = 0; i < g -> N; i++) {
        if(g -> ulkeler[i].netEnerji > 0) {
            enerjiFazlasiOlanlar.push_back(i); // Satıcı olabilecek ülkeler
        }
        else if(g -> ulkeler[i].netEnerji < 0) {
            enerjiAcigiOlanlar.push_back(i); // Alıcı olması gereken ülkeler
        }
    }

    // Önce enerji açığı olanları düzelt
    // Strateji: Önce alıcıları doyur, sonra satıcıların fazlasını değerlendir
    for(int i : enerjiAcigiOlanlar) {
        enerjiDengesizliginiGider(g, i);
    }
}

// Ülkeleri kar/zarar durumuna göre sırala
// Kar = (Enerji fazlası * satış fiyatı) - (Satın alma maliyeti + ceza maliyeti)
// Zarar = (Satın alma maliyeti + ceza maliyeti)
void sirala(Graph* g, vector<int>& siraliIndeksler) {
    // Ülkeleri kar/zarar durumuna göre sıralayan fonksiyon
    vector<pair<double, int >> kazancZarar; // (kazanç/zarar, index)

    for(int i = 0; i < g -> N; i++) {
        Ulke& ulke = g -> ulkeler[i];
        double kazanc = 0;

        if(ulke.netEnerji > 0) {
            // Enerji fazlası satışından kazanç
            kazanc = ulke.netEnerji * ulke.enerjiSatisFiyati;
        }

        double zarar = ulke.satinAlmaMaliyeti + ulke.cezaMaliyeti;
        double netKazanc = kazanc - zarar;

        kazancZarar.push_back({netKazanc, i});
    }

    // En çok kar edenden en çok zarar edene doğru sırala
    sort(kazancZarar.rbegin(), kazancZarar.rend()); 

    siraliIndeksler.clear();
    for(auto& p : kazancZarar) {
        siraliIndeksler.push_back(p.second);
    }
}

// Sonuçları dosyaya yaz
void sonuclariDosyayaYaz(Graph* g) {
    ofstream maliyetDosyasi("maliyet.txt");
    ofstream sonucDosyasi("sonuc.txt");

    if(!maliyetDosyasi.is_open() || !sonucDosyasi.is_open()) {
        cout << "Dosyalar yazilmadi !" << endl;
        return;
    }

    double toplamSatinAlmaMaliyeti = 0;
    double toplamCezaMaliyeti = 0;

    maliyetDosyasi << "=== ULKELERIN MALIYETLERI ===\n\n";
    maliyetDosyasi << left << setw(15) << "ulke"
                << setw(20) << "Satin Alma Maliyeti"
                << setw(20) << "Ceza Maliyeti"
                << setw(20) << "Toplam Maliyet" << "\n";
    maliyetDosyasi << string(75, '-') << "\n";

    for(int i = 0; i < g -> N; i++) {
        Ulke& ulke = g -> ulkeler[i];
        toplamSatinAlmaMaliyeti += ulke.satinAlmaMaliyeti;
        toplamCezaMaliyeti += ulke.cezaMaliyeti;

        maliyetDosyasi << left << setw(15) << ulke.isim
                    << setw(20) << fixed << setprecision << ulke.satinAlmaMaliyeti
                    << setw(20) << ulke.cezaMaliyeti
                    << setw(20) << ulke.toplamMaliyet << "\n";
    }

    maliyetDosyasi << "\n" << string(75, '-' ) << "\n";
    maliyetDosyasi << "TOPLAM SATIN ALMA MALIYETI: " << toplamSatinAlmaMaliyeti << " Euro\n";
    maliyetDosyasi << "TOPLAM CEZA MALIYETI: " <<toplamCezaMaliyeti << "Euro\n";
    maliyetDosyasi << "GENEL TOPLAM: " << (toplamSatinAlmaMaliyeti + toplamCezaMaliyeti) << "Euro\n";

    vector<int> siraliIndeksler;
    sirala(g, siraliIndeksler);

    sonucDosyasi << "=== 1 GUNLUK HESAPLAMA SONUCLARI ===\n\n";
    sonucDosyasi << "TOPLAM SATIN ALMA MALIYETI: " << toplamSatinAlmaMaliyeti << " Euro\n";
    sonucDosyasi << "TOPLAM CEZA MALIYETI: " << toplamCezaMaliyeti << "Euro\n";
    sonucDosyasi << "GENEL TOPLAM MALIYET: " << (toplamSatinAlmaMaliyeti + toplamCezaMaliyeti) << " Euro\n\n";


    sonucDosyasi << "=== ULKELERIN KAR/ZARAR SIRALAMASI ===n";
    sonucDosyasi << "(En cok kar edenden en cok zarar edene)\n\n";
    sonucDosyasi << left << setw(5) << "Sira"
                << setw(15) << "Ulke"
                << setw(15) << "Net Enerji"
                << setw(20) << "Satin Alma"
                << setw(20) << "Ceza"
                << setw(20) << "Net Kazanc/Zarar" << "\n";
    sonucDosyasi << string(95, '-') << "\n";

    for(int sira = 0; sira < siraliIndeksler.size(); sira++) {
        int idx = siraliIndeksler[sira];
        Ulke& ulke = g -> ulkeler[idx];

        double kazanc = 0;
        if(ulke.netEnerji > 0) {
            kazanc = ulke.netEnerji * ulke.enerjiSatisFiyati;
        }
        double zarar = ulke.satinAlmaMaliyeti + ulke.cezaMaliyeti;
        double netKazanc = kazanc - zarar;

        sonucDosyasi << left << setw(5) << (sira + 1)
                    << setw(15) << ulke.isim
                    << setw(15) << fixed << setprecision(2) << ulke.netEnerji
                    << setw(20) << ulke.satinAlmaMaliyeti
                    << setw(20) << ulke.cezaMaliyeti
                    << setw(20) << netKazanc << "\n";
    }

    maliyetDosyasi.close();
    sonucDosyasi.close();
}