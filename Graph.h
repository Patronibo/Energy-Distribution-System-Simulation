#pragma once
// Bu bir ön işlemci direktifidir (preprocessor directive)
// Derleyiciye bu dosyanın sadece bir kez include edilmesini söyler
// Birden fazla dosyada include edilse bile tek bir kopyası kullanılır
// Header guard (#ifndef/#define/#enddif) yerine kullanılan modern bir yöntem
#include <vector>
#include <string>
#include <iostream>
using namespace std;

struct Ulke  { // Ulke adında bir yapı (struct) tanımlanıyor
    int id;
    string isim;
    long uretim, tuketim;
    int cezaKatsayisi, enerjiSatisFiyati;
    double netEnerji, toplamMaliyet;
    double satinAlmaMaliyeti, cezaMaliyeti;

    // Yapıcı fonksiyon (constructor) - varsayılan değer atamaları
    Ulke() : id(0), uretim(0), tuketim(0), cezaKatsayisi(0),
    enerjiSatisFiyati(0), netEnerji(0), toplamMaliyet(0),
    satinAlmaMaliyeti(0), cezaMaliyeti(0) {}
    // Member initialization list kullanılarak tüm değişkenlere 0 değeri atanıyor
    // Bu, ülke nesnesi oluşturulduğunda otomatik olarak çağrılır
    
};

class Graph { // Graph adında bir sınıf tanımladık
    public:
        int N;
        vector<Ulke> ulkeler;
        vector<vector<int>> adjMatrix; // adjMatrix[i][j] = i ve j ülkeleri arasındaki mesafeyi hesaplamsı için

        // Fonksiyon prototipleri (declarations)
        Graph(int n);
        void setUlkeler(const vector<Ulke>& ulkelerList);
        void setDeğisken(int i, int j, int w);
        void yazdirGraph();
        int getMesafe(int i, int j);
};

Graph::Graph(int n) { // Graph sınıfının yapıcı fonksiyonunun tanımı
    // Graph nesnesi oluşturulurken çağrılır, örn: Graph* g = new Graph(10);

    N = n; // Parametre olarak gelen n değerini N değişkenine ata

    adjMatrix.resize(N, vector<int>(N, 0));
    // Komşuluk matrisini boyutlandır:
    // 1. resize(N,.....) : Dış vektörü N boyutuna getir
    // 2. vector<int>(N, 0): Her bir iç vektör N boyutunda ve tüm elemanlar 0
    // Sonuç: N x N boyutunda, tüm elemanları 0 olan bir matris oluşturulur
    // Örnek: N=3 ise -> [[0,0,0], [0,0,0], [0,0,0]]
}

void Graph::setUlkeler(const vector<Ulke>& ulkelerList) {
    this -> ulkeler = ulkelerList;
    // this pointer'ı: Mevcut nesneyi işaret eder
    // ulkelerList parametresindeki Ülke listesini sınıfın ulkeler üyesine kopyalar
    // const & kullanımı: REferans ile geçirerek kopyalama maliyetini azaltır,
    // const ile değiştirilemez olduğunu garanti eder
}

void Graph::setDeğisken(int i, int j, int w) {
    adjMatrix[i][j] = w;
    adjMatrix[j][i] = w; // Burada simetrik graf alıyoruz
    // Bu, iki yönlü graf olduğunu gösterir
    // Mesafe/maliyet her iki yönde de aynıdır
}

int Graph::getMesafe(int i, int j) {
    if(i >= 0 && i < N && j >= 0 && j < N) { // İndeks sınır kontrolü
        return adjMatrix[i][j]; // Geçerli indeksler ile mesafeyi döndür
    }
    return 0;
    // Bu kontrol, "array/vector index out of bounds" hatasını önler
}

void Graph::yazdirGraph() {
    cout << "\n=== ULKELER ===\n";
    for(int i=0; i<N; i++) { // Ülkeler için üretim tüketimi yazdırıyor
        cout << ulkeler[i].id << " - " << ulkeler[i].isim
        << " (Uretim: " << ulkeler[i].uretim
        << ", Tuketim: " << ulkeler[i].tuketim << ")\n";
    }

    cout << "\n=== Graf ===\n";
    cout <<"\t";
    for(int i=0; i<N; i++) {
        cout << (i+1) << "\t";
    }
    cout << "\n";
    for(int i=0; i<N; i++) {
        cout << (i+1) << "\t";  // Burada da matrixi yapıyoruz
        for(int j=0; j<N; j++)
        {
            cout << adjMatrix[i][j] << "\t";
        }
        cout << endl;
    }
}