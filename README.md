# Energy-Distribution-System-Simulation
This project is a program that addresses energy deficits and calculates costs, based on examples from some countries in Europe.

# Avrupa Enerji Dağıtım Sistemi Simülasyonu

Bu proje, **Algoritma Tasarım ve Analizi** dersi kapsamında geliştirilmiş bir C++ programıdır.  
Amaç, Avrupa ülkeleri arasındaki enerji dağıtımını simüle ederek **enerji açığı yaşayan ülkeleri tespit etmek**, **en ucuz transfer yollarını bulmak** ve **sistemin toplam maliyetini hesaplamaktır**.  

## Projenin Amacı
- Hangi ülkeler enerji açığı yaşıyor?
- Açığı kapatmanın en ucuz yolu nedir?
- Hangi ülke en çok kar ediyor?
- Sistemin toplam maliyeti ne kadar?

## Nasıl Çalışıyor?
Program menü tabanlıdır ve aşağıdaki adımlarla çalışır:

1. **Dosyaları oku ve grafi oluştur**  
   - `ulkeler.txt` → Ülkelerin üretim/tüketim bilgileri  
   - `enerjihatlari.txt` → Ülkeler arası mesafeler  

2. **Net enerji hesapla**  
   - Ülkelerin üretim ve tüketim farklarını hesaplar.

3. **Enerji dengesizliklerini gider**  
   - Açgözlü (Greedy) algoritma ile en ucuz enerji transferini yapar.

4. **Maliyetleri göster**  
   - Satın alma ve ceza maliyetlerini listeler.

5. **Kar/Zarar durumunu sırala**  
   - Ülkeleri en çok kar edenden en çok zarar edene sıralar.

6. **Sonuçları dosyaya yaz**  
   - `maliyet.txt` ve `sonuc.txt` dosyaları oluşturulur.

7. **Tüm işlemleri otomatik çalıştır**  
   - Yukarıdaki adımlar tek seferde yapılır.

8. **Grafi göster**  
   - Komşuluk matrisi (adjacency matrix) ile ülkeler arası bağlantılar listelenir.

## Kullanılan Algoritmalar
- **Graf Oluşturma Algoritması**  
  - Komşuluk matrisi yaklaşımı  
  - Karmaşıklık: `O(N^2)`  

- **Enerji Dengeleme Algoritması (Greedy)**  
  - Her adımda en ucuz seçeneği seçer  
  - Satın alma maliyeti < Ceza maliyeti → Satın al  
  - Satın alma maliyeti ≥ Ceza maliyeti → Ceza öde  

## Kurulum ve Çalıştırma
1. Projeyi klonla:
   ```bash
   git clone https://github.com/Patronibo/enerji-dagitim-simulasyonu.git
   cd enerji-dagitim-simulasyonu
