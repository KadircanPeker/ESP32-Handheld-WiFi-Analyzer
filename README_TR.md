# 📡 ESP32 Taşınabilir Wi-Fi Analiz Cihazı

![Sürüm](https://img.shields.io/badge/version-1.0.0-blue)
![Platform](https://img.shields.io/badge/platform-ESP32-orange)
![Lisans](https://img.shields.io/badge/license-MIT-green)

[English](README.md)

**ESP32** mikrodenetleyicisiyle geliştirilmiş taşınabilir bir ağ analiz cihazıdır. Sistem; 2,4 GHz Wi-Fi spektrumunun görüntülenmesini, sinyal seviyelerinin (RSSI) incelenmesini ve ağların kullandığı güvenlik protokollerinin gerçek zamanlı olarak belirlenmesini sağlar.

Proje, standart ağ tarayıcılarından farklı olarak kararlı çalışma için özel bir **durum makinesi mimarisi** kullanır. **Derin Tarama** algoritması, kullanıcı arayüzünün tepki süresini bozmadan zayıf sinyallerin algılanmasını sağlar.

## ✨ Temel Özellikler

* **📊 Gerçek Zamanlı Liste Modu:** Kaydırılabilir arayüzde SSID, RSSI (dBm) ve güvenlik durumunu (Açık/Korumalı) gösterir; gizli ağları uygun biçimde işler.
* **📈 Spektrum Grafik Modu:** Kanal yoğunluğunu (Kanal 1-13) görselleştirerek parazit kaynaklarının incelenmesine ve yönlendirici konumunun iyileştirilmesine yardımcı olur.
* **🔍 Ayrıntı Görünümü:** Seçilen ağ için MAC adresi (BSSID), kanal ve şifreleme türü (WEP/WPA2/WPA3) bilgilerini gösterir.
* **⚡ Akıcı Kullanıcı Arayüzü:** Küçük OLED ekranda kararlı gezinme için buton sıçramasını önleyen yazılım ve hızlı kaydırma işlevi kullanır.
* **💾 Verimli Kaynak Kullanımı:** Grafik verilerini PROGMEM üzerinde tutarak bellek kullanımını azaltır ve sistem kararlılığını destekler.

## 🛠️ Gerekli Donanım

| Bileşen | Adet | Açıklama |
| :--- | :--- | :--- |
| **ESP32 geliştirme kartı** | 1 | Herhangi bir ESP32 modeli; WROOM-32 önerilir |
| **0,96 inç OLED ekran** | 1 | SSD1306 sürücü, I2C arayüzü |
| **Buton** | 4 | 6x6 mm basmalı buton |
| **Jumper kablolar** | - | Erkek-erkek ve erkek-dişi kablolar |
| **Breadboard / PCB** | 1 | Prototip veya kalıcı montaj için |

## 🔌 Pin Bağlantıları

| Bileşen pini | ESP32 GPIO | İşlev |
| :--- | :--- | :--- |
| **OLED SDA** | GPIO 21 | I2C veri hattı |
| **OLED SCL** | GPIO 22 | I2C saat hattı |
| **K1 Butonu (YUKARI)** | GPIO 18 | Yukarı kaydırma / gezinme |
| **K2 Butonu (AŞAĞI)** | GPIO 19 | Aşağı kaydırma / gezinme |
| **K3 Butonu (MOD)** | GPIO 5 | **Grafik Modu** geçişi |
| **K4 Butonu (SEÇ)** | GPIO 4 | Seçim / **Ayrıntı Görünümü** / yeniden tarama |

*> Not: Bütün butonlar dahili `INPUT_PULLUP` dirençleriyle yapılandırılmıştır. Harici pull-up direnci gerekli değildir.*

## 📸 Görseller

Cihazın farklı çalışma modları:

| **Liste Modu** | **Grafik Modu** | **Ayrıntı Modu** |
| :---: | :---: | :---: |
| <img src="https://github.com/KadircanPeker/ESP32-Handheld-WiFi-Analyzer/blob/main/mode_list.jpeg?raw=true" width="250"> | <img src="https://github.com/KadircanPeker/ESP32-Handheld-WiFi-Analyzer/blob/main/mode_graph.jpeg?raw=true" width="250"> | <img src="https://github.com/KadircanPeker/ESP32-Handheld-WiFi-Analyzer/blob/main/mode_detail.jpeg?raw=true" width="250"> |

## 🚀 Kurulum ve Kullanım

1. Repository'yi indirin veya kaynak kodu kopyalayın.
2. Arduino IDE Kütüphane Yöneticisi üzerinden `Adafruit_GFX` ve `Adafruit_SSD1306` kütüphanelerini yükleyin.
3. Kart modelini seçin (örneğin `DOIT ESP32 DEVKIT V1`) ve `.ino` dosyasını ESP32'ye yükleyin.
4. Kontroller:
    * Liste içinde gezinmek için **K1/K2** butonlarını kullanın.
    * Seçilen ağın ayrıntılarını görüntülemek için **K4** butonuna basın.
    * Kanal yoğunluk grafiğini görüntülemek için **K3** butonuna basın.

## 👨‍💻 Geliştirici

**Kadircan Peker**  
*Elektrik-Elektronik Mühendisliği Öğrencisi*  
*Çalışma alanı: Gömülü Sistemler ve IoT*

---
*Uyarı: Bu cihaz yalnızca eğitim ve ağ analizi amaçları için tasarlanmıştır.*
