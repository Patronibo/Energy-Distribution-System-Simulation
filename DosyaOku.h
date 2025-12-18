#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Graph.h"
using namespace std;

Graph* dosyaOku() {
    ifstream f1("ulkeler.txt");
    ifstream f2("enerjihatlari.txt");

    if(!f1.is_open() || !f2.is_open()) {
        cout << "Dosyalar acilmadi !" << endl;
        return nullptr;
    }

    vector<Ulke> ulkelerList;
    string satir;

    // İlk satır olduğu için burda başlığı atlıyoruz
    getline(f1, satir);
    while(getline(f1, satir)) {
        if(satir.empty()) { 
            continue;
        }
        
        stringstream ss(satir);
        Ulke ulke;

        ss >> ulke.id >> ulke.isim >> ulke.uretim >> ulke.tuketim
        >> ulke.cezaKatsayisi >> ulke.enerjiSatisFiyati;

        ulkelerList.push_back(ulke);
    }

    int N = ulkelerList.size();
    Graph* g = new Graph(N);
    g -> setUlkeler(ulkelerList);

    int i = 0; // satır indeksi (kaynak ülke indeksi)
    while(getline(f2, satir) && i < N) { // Satır oku ve indeks kontrol et 
        if(satir.empty()) { // Boş satır varsa atla
            continue;
        }
 
        stringstream ss(satir); // Satırı stringstream'e aktar
        string cell; // Virgülle ayrılmış her hücre
        int j = 0; // Sütün indeksi (hedef ülke indeksi)

        while(getline(ss, cell, ',') && j < N) { // getline (ss, cell, ',' karakterinde kadar oku)
            if(cell != "-" && !cell.empty()) { // "-" veya boş değilse
                int w =stoi(cell); // String'i integer'a çevir
                g -> setDeğisken(i, j, w); // Grafaa mesafeyi ekle
                // i: kaynak ülke, j: hedef ülke, w:mesafe
            }
            j++; // Sonraki sütuna geç
        }
        i++; // sonraki satıra geç
    }
    // Bu döngü sonunda adjMatrix tamamen doldurulmuş olur

    f1.close();
    f2.close();
    // Biz burda dosyayı kapatıyoruz zaten ifstream nesneleri destruktor'da otomatik kapanır.

    return g;
}