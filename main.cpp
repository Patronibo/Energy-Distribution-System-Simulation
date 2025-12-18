#include "Graph.h"
#include "DosyaOku.h"
#include "Fonksiyonlar.h"
#include <iostream>
#include <iomanip>  // Çıktı formatlama için standart kütüphane
#include <vector>
using namespace std;

void menuGoster() {
    cout << "\n========================================\n";
    cout << "   AVRUPA ENERJI DAGITIM SISTEMI\n";
    cout << "==========================================\n";
    cout << "1. Dosyalari Oku ve Grafi Olustur\n";
    cout << "2. Net Enerji Hesapla\n";
    cout << "3. Enerji Dengesizliklerini Gider\n";
    cout << "4. Maliyetleri Goster\n";
    cout << "5. Ulkeleri Kar/Zarar Durumuna Gore Sirala\n";
    cout << "6. Sonuclari Dosyaya Yaz\n";
    cout << "7. Tum Islemleri Otomatik Calistir\n";
    cout << "8. Grafi Goster\n";
    cout << "0. Cikis\n";
    cout << "===========================================\n";
    cout << "Seciminiz: ";
}

void netEnerjileriGoster(Graph* g) {
    if(!g) {
        cout << "Graf olusturulmadi !\n"; // Burda g yi kontrol et graf oluşmuşmu
        return;
    }

    cout << "\n === ULKELERIN NET ENERJI DURUMU ===\n\n";
    cout << left << setw(5) << "ID" // Sola hizala, 5 karakter genişliğinde "ID"
       << setw(15) << "Ulke" // 15 karakter genişliğinde "ULKE"
       << setw(15) << "Uretim"
       << setw(15) << "Tuketim"
       << setw(15) << "Net Enerji" << "\n";
    cout << string(65, '-') << "\n"; // 65 Karakter uzunluğunda çizgi

    for(int i = 0; i < g -> N; i++) { // Tüm ülkeler için döngü (g->N: Ülke sayısı)
        double net = netEnerji(g, i); // i. ülkenin net enerjisini hesapla
        Ulke& ulke = g -> ulkeler[i]; // i. ülkeye referans al

        cout << left << setw(5) << ulke.id // Sol tarafa 5 karakterle ülke ID si ni yaz
            << setw(15) << ulke.isim
            << setw(15) << ulke.uretim
            << setw(15) << ulke.tuketim
            << setw(15) << fixed << setprecision(2) << net << "\n"; // Net enerjiyi 2 ondalıkla yaz
    }  // fixed = kayan nokta değerlerini sabit nokta gösteriminde yazıyor
    cout << "\n";
}

void maliyetliGoster(Graph* g) {
    if(!g) {
        cout << "Graf olusturulmadi !\n";
        return;
    }

    cout << "\n=== ULKELERIN MALIYETLERI ===\n\n";
    cout << left << setw(5) << "ID"
        << setw(15) << "ULKE"
        << setw(20) << "Satin Alma Maliyeti"
        << setw(20) << "Ceza Maliyeti"
        << setw(20) << "Toplam Maliyet" << "\n";
    cout << string(80, '-') << "\n";

    double toplamSatinAlma = 0;
    double toplamCeza = 0;

    for(int i = 0; i < g->N; i++) { // Tüm ülkeler için döngü
        Ulke& ulke = g->ulkeler[i]; // Ülkeye referans al
        toplamSatinAlma += ulke.satinAlmaMaliyeti; // bu iki satırda toplama ekleme yapıyoruz
        toplamCeza += ulke.cezaMaliyeti;

        cout << left << setw(5) << ulke.id // Ülke bilgilerini yazdırıyoruz
            << setw(15) << ulke.isim
            << setw(20) << fixed << setprecision(2) << ulke.satinAlmaMaliyeti
            << setw(20) << ulke.cezaMaliyeti
            << setw(20) << ulke.toplamMaliyet << "\n";
    }

    cout << string(80, '-') << "\n";
    cout << left << setw(20) << "TOPLAM: "
        << setw(20) << fixed << setprecision(2) << toplamSatinAlma
        << setw(20) << toplamCeza
        << setw(20) << (toplamSatinAlma + toplamCeza) << "\n\n";
}

void karZararSiralamasiniGoster(Graph* g) {
    if(!g) {
        cout << "Graf olusturulmadi!\n";
        return;
    }

    vector<int> siraliIndeksler; // Sıralı indeksleri tutacak vector 
    sirala(g, siraliIndeksler); // Sıralama fonksiyonunu çağır

    cout << "\n=== ULKELERIN KAR/ZARAR SIRALAMASI ===\n";
    cout << "(En cok kar edenden en cok zarar edene)\n\n";
    cout << left << setw(5) << "Sira"
        << setw(15) << "Ulke"
        << setw(15) << "Net Enerji"
        << setw(20) << "Satin Alma"
        << setw(20) << "Ceza"
        << setw(20) << "Net Kazanc/Zarar" << "\n";
    cout << string(95, '-') << "\n";

    for(int sira = 0; sira < siraliIndeksler.size(); sira++) { // Sıralı listeyi dolaş
        int idx = siraliIndeksler[sira]; // Sıradaki ülke indeksi
        Ulke& ulke = g -> ulkeler[idx]; // Ülkeye referans al

        double kazanc = 0;  // Kazanç değişkeni
        if(ulke.netEnerji > 0) { // Eğer net  enerji pozitifse
            kazanc = ulke.netEnerji * ulke.enerjiSatisFiyati; // Satış kazancı hesapla
        }
        double zarar = ulke.satinAlmaMaliyeti + ulke.cezaMaliyeti;
        double netKazanc = kazanc - zarar;

        cout << left << setw(5) << (sira + 1)
            << setw(15) << ulke.isim
            << setw(15) << fixed << setprecision(2) << ulke.netEnerji
            << setw(20) << ulke.satinAlmaMaliyeti
            << setw(20) << ulke.cezaMaliyeti
            << setw(20) << netKazanc << "\n";
    }
    cout << "\n";
}

void tumIslemleriCalistir(Graph* g) {
    if(!g) {
        cout << "Graf olusturulmadi !\n";
        return;
    }

    cout << "\n=== TUM ISLEMLER CALISTIRILIYOR ===\n\n";

    cout << "1. Net enerjiler hesaplaniyor...\n";
    for(int i = 0; i < g->N; i++) {
        netEnerji(g, i);
    }
    cout << "   Tamamlandi!\n\n";

    cout << "2. Enerji dengesizlikleri gideriliyor...\n";
    tumUlkeleriDengele(g);
    cout << "   Tamamlandi!\n\n";

    cout << "3. Sonuclar dosyaya yaziliyor...\n";
    sonuclariDosyayaYaz(g);
    cout << "   Tamamlnadi! (maaliyet.txt ve sonuc.txt dosyalari olusturuldu)\n\n";

    cout << "=== ISLEMLER TAMAMLANDI ===\n\n";

    // Özet bilgileri göster
    double toplamSatinAlma = 0;
    double toplamCeza = 0;
    for(int i = 0; i < g->N; i++) { // Tüm ülkeler için toplamları hesaplama döngüsü
        toplamSatinAlma += g -> ulkeler[i].satinAlmaMaliyeti; 
        toplamCeza += g -> ulkeler[i].cezaMaliyeti;
    }

    cout << "OZET:\n";
    cout << " Toplam  Satin Alma Maliyeti : " << fixed << setprecision(2) << toplamSatinAlma << "Euro\n";
    cout << " Toplam Ceza Maliyeti : "<< toplamCeza << "Euro\n";
    cout << " Genel Toplam: " << (toplamSatinAlma + toplamCeza) << "Euro\n\n";
}

int main() {
    Graph* g = nullptr; // Başlangıçta bunu boş yapıyoruz
    int secim;

    cout << "Avrupa Enerji Dagitim Sistemi'ne Hos Geldiniz!\n";

    do {
        menuGoster();
        cin >> secim;

        switch(secim) {
            case 1:
                cout << "\nDosyalar okunuyor...\n";
                g = dosyaOku();
                if(g) {
                    cout << "Basarili!" << g->N << " ulke yuklendi.\n";
                }
                else {
                    cout << " Dosyalar okunmadi!\n";
                }
                break;

            case 2:
                if(g) {
                    netEnerjileriGoster(g);
                }
                 else {
                    cout << "\n Once dosyalari okuyunuz! (Secenek 1) \n";
                 }
                 break;

            case 3:
                 if(g) {
                    cout << "\nEnerji dengesizlikleri gideriliyor...\n";
                    tumUlkeleriDengele(g);
                    cout << "Tamamlandi!\n";
                 }
                 else {
                    cout << "\n Once dosyalari okuyunuz! (Secenek 1) \n";
                 }
                 break;

            case 4:
                 if(g) {
                    maliyetliGoster(g);
                 }
                 else {
                    cout << "\nOnce dosyalari okuyunuz! (Secenek 1) \n";
                 }
                 break;

            case 5:
                if(g) {
                    karZararSiralamasiniGoster(g);
                }
                else {
                    cout << "\nOnce dosyalari okuyunuz! (Secenek 1) \n";
                }
                break;

            case 6:
                if(g) {
                    cout << "\nSonuclar dosyaya yaziliyor...\n";
                    sonuclariDosyayaYaz(g);
                    cout << "Tamamlandi! (maliyet.txt ve sonuc.txt dosyalari olusturuldu)\n";
                }
                else {
                    cout << "\nOnce dosyalari okuyunuz! (Secenek 1)\n";
                }
                break;
            case 7:
                if(g) {
                    tumIslemleriCalistir(g);
                }
                else {
                    cout << "\nOnce dosyalari okuyunuz! (Secenek 1)\n";
                }
                break;
                
            case 8:
                if(g) {
                    g->yazdirGraph();
                }
                else {
                    cout << "\nOnce dosyalari okuyunuz! (Secenek 1)\n";
                }
                break;
                
            case 0:
                cout << "\nProgramdan cikiliyor...\n";
                break;
                
            default:
                cout << "\nGecersiz secim! Lutfen tekrar deneyin.\n";
                break;

        }

        if(secim != 0) {
            cout << "\n Devam etmek icin Enter'a basin ...";
            cin.ignore(); // Önceki girişi temizle
            cin.get(); // Enterda çıkar gibi
        }
    }while (secim != 0);

    if(g) {
        delete g; // Eğer graf oluştuysa grafı serbest bırak
    }
    return 0;
    
}