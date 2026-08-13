# DALI CCT 48V 30W

DALI-2 DT8 Tunable White LED driver reference design für **48 V DC SELV Eingang** mit bis zu **30 W LED-Leistung**.

Dieses Repository enthält **Hardware- und Firmware-Umfang** inklusive Dokumentation für Entwicklung, Bring-up, Test und Inbetriebnahme.

> ⚠️ **Wichtig:** Referenzdesign für Entwicklungs- und Evaluierungszwecke. Kein automatisch zertifiziertes Endprodukt.

---

## Inhaltsübersicht

- [Projektüberblick](#projektüberblick)
- [Systemarchitektur](#systemarchitektur)
- [Repository-Struktur](#repository-struktur)
- [Hardware-Umfang](#hardware-umfang)
- [Software/Firmware-Umfang](#softwarefirmware-umfang)
- [Toolchain & Voraussetzungen](#toolchain--voraussetzungen)
- [Build & Flash (Firmware)](#build--flash-firmware)
- [Inbetriebnahme (HowTo)](#inbetriebnahme-howto)
- [Test & Verifikation](#test--verifikation)
- [Troubleshooting](#troubleshooting)
- [Sicherheitshinweise](#sicherheitshinweise)
- [Weiterführende Dokumentation](#weiterführende-dokumentation)

---

## Projektüberblick

Das Design realisiert einen DALI-2 DT8 CCT-Treiber (Tunable White) mit:

- **Eingang:** 48 V DC SELV (typischer Bereich 43.2–52.8 V)
- **Ausgang:** gemeinsamer Anodenbus `LED+` und zwei geregelte Kanäle `WW−` / `CW−`
- **Leistung:** bis 30 W
- **Regelung:** kontinuierliche analoge Stromdimmung (0.1–100 %, ohne niederfrequentes PWM-Flackern)
- **Kommunikation:** galvanisch isolierte DALI-2 DT8 Tc Schnittstelle
- **Steuerung:** STM32G031K8T6 + AT24C32E EEPROM

Ziel ist ein robustes Referenzsystem, das als Grundlage für eigene Leuchten-/Treiberentwicklungen dient.

---

## Systemarchitektur

1. **Primärversorgung / Eingangsseite**
   - 48 V SELV Versorgung mit Schutzkonzept für Überspannungsereignisse.

2. **Leistungsteil**
   - 4‑Switch synchroner Buck-Boost (LT8390A) erzeugt geregelten LED-Bus.

3. **LED-Kanalregelung**
   - Zwei lineare Low-Side Konstantstromsenken für Warmweiß/Kaltweiß.

4. **Steuer- und Kommunikationsschicht**
   - STM32G0 übernimmt DALI-Stack, DT8-Verhalten, Kanalmischung, Schutzreaktionen und Persistenz.

5. **Nichtflüchtige Datenhaltung**
   - Externes EEPROM für Parameter, Zustände und Konfigurationswerte.

---

## Repository-Struktur

```text
.
├─ .github/
├─ README.md
├─ firmware/
│  ├─ README.md
│  ├─ docs/
│  │  ├─ README.md
│  │  ├─ build.md
│  │  ├─ api.md
│  │  ├─ dali_implementation.md
│  │  └─ troubleshooting.md
│  ├─ src/
│  ├─ test/
│  └─ lib/
└─ hardware/
   └─ DALI_CCT_48V_30W/
      ├─ README.md
      └─ docs/
         ├─ design-specification.md
         ├─ bom.csv
         ├─ net-class-and-layout-rules.md
         ├─ bring-up-and-test-plan.md
         ├─ open-items-and-risks.md
         └─ firmware-interface.md
```

> Die exakte Dateiliste kann sich weiterentwickeln; für die aktuelle Struktur bitte das Repository-Browsing verwenden.

---

## Hardware-Umfang

Im Pfad `hardware/DALI_CCT_48V_30W/` liegt das vollständige Hardware-Referenzprojekt mit:

- KiCad-Projektdateien (4-Layer PCB, 150 × 60 mm)
- BOM mit Bauteilauswahl und Alternativen
- Spezifikation, Layout-/Netzklassenregeln
- Bring-up-/Testplan
- Risiken & offene Punkte
- Schnittstellendokument zur Firmware-Anbindung

Kernmerkmale:

- 48 V DC SELV Eingang
- 30 W Ausgangsleistung
- gemeinsamer LED+ Bus, getrennte WW/CW Senken
- analoge, kontinuierliche Stromdimmung
- isolierte DALI-2 DT8 Schnittstelle

---

## Software/Firmware-Umfang

Im Pfad `firmware/` befindet sich eine produktionsorientierte Firmware-Basis für **STM32G031** plus host-testbare Logik.

### Enthaltene Bereiche

- `firmware/src/`
  - Hardwareabstraktion/HAL-Wrapper
  - DALI-Kommunikations- und Auswertepfade
  - CCT-/Kanalregel- und Schutzlogik
  - NVM-Zugriffe (EEPROM)
  - Scheduler/Task-Orchestrierung

- `firmware/test/`
  - Host-seitige Unit-Tests für zentrale Protokoll-/Regellogik

- `firmware/docs/`
  - `build.md` (Build/Flash)
  - `api.md` (Modul-/API-Überblick)
  - `dali_implementation.md` (DT8-Verhalten)
  - `troubleshooting.md` (Fehlersuche)

- `firmware/lib/`
  - Integrationshaken für STM32CubeG0 bzw. plattformnahe Komponenten

### Firmware-Funktionsumfang (auf hoher Ebene)

- DALI-2/DT8 Kommandopfad
- Helligkeits- und CCT-bezogene Sollwertverarbeitung
- Kanalmischung WW/CW gemäß Zieltemperatur/Farbanteil
- Schutz- und Fehlerbehandlung (z. B. Grenzwertverletzungen)
- Persistenz relevanter Parameter im EEPROM

---

## Toolchain & Voraussetzungen

### Hardware

- Labornetzteil 48 V DC mit Strombegrenzung
- Last (geeignetes CCT-LED-Setup)
- DALI-Master / DALI-USB Interface
- Multimeter, optional Oszilloskop und Thermalkamera

### Software/Firmware

- ARM GCC Toolchain (oder projektspezifisch dokumentierte Alternative)
- Build-System gemäß `firmware/docs/build.md`
- Flash/Debug-Tool für STM32G0
- Optional: Host-Testumgebung (Compiler + Testframework laut `firmware/test`)

---

## Build & Flash (Firmware)

> Verbindliche Details stehen in `firmware/docs/build.md`.

Typischer Ablauf:

1. Toolchain installieren
2. Firmware-Projekt konfigurieren
3. Build erzeugen (Debug/Release)
4. Binary/ELF auf STM32G031 flashen
5. Serielles Logging / Debug-Schnittstelle prüfen
6. Basisfunktion ohne DALI-Verkehr verifizieren

Empfehlung:

- Erst mit konservativen Strom-/Leistungsgrenzen testen
- Schutzpfade und Fault-Indikationen früh validieren

---

## Inbetriebnahme (HowTo)

Diese Schrittfolge ist als praxisorientierter Leitfaden gedacht und ergänzt den detaillierten Bring-up-Plan unter `hardware/.../docs/bring-up-and-test-plan.md`.

### 1) Sichtprüfung (ohne Spannung)

- Bestückung, Polaritäten, Lötqualität, Steckverbinder prüfen
- Isolations- und Kriechstrecken visuell kontrollieren
- Sicherstellen, dass keine Kurzschlüsse zwischen kritischen Netzen vorliegen

### 2) Erstes Einschalten (ohne LED-Last)

- Labornetzteil auf 48 V setzen, Stromlimit niedrig starten
- Board versorgen und Ruhestrom plausibilisieren
- Grundversorgungen (MCU-Rails, Referenzen) messen

### 3) Firmware laden und Basiskommunikation testen

- Firmware flashen
- MCU startet ohne Fault-Latch
- Debug/Statusausgaben oder definierte Lebenszeichen prüfen

### 4) DALI-Bus anbinden

- DALI-Leitungen korrekt verbinden (Polarität/Buskonzept beachten)
- DALI-Master verbinden
- Erreichbarkeit und Reaktion auf Basisbefehle verifizieren

### 5) LED-Last anschließen

- CCT-Last korrekt an `LED+`, `WW−`, `CW−` anschließen
- Mit niedrigen Sollwerten starten
- Kanalströme messen und plausibilisieren

### 6) Funktionsprüfung CCT/Helligkeit

- Helligkeitsrampe über den vorgesehenen Bereich prüfen
- CCT-Änderungen (warm ↔ kalt) beobachten
- Übergänge auf Stabilität/Flickerfreiheit prüfen

### 7) Schutz- und Grenzfalltests

- Grenzbereiche in kontrollierter Form anfahren
- Verifikation von Fault-Reaktionen und Recovery-Strategie
- Sicherstellen, dass keine unzulässige Überlastung entsteht

### 8) Thermik-Check unter Dauerlast

- Erwärmung an kritischen Komponenten überwachen
- Betriebspunkt nur freigeben, wenn thermisch stabil

### 9) Persistenz/NVM prüfen

- Relevante Parameter ändern
- Power-Cycle durchführen
- Wiederherstellung aus EEPROM verifizieren

### 10) Abschlussfreigabe (Prototyp)

- Dokumentierte Sollwerte/Abnahmekriterien abgleichen
- Messergebnisse protokollieren

---

## Test & Verifikation

Empfohlene Reihenfolge:

1. Host-Unit-Tests (`firmware/test`)
2. Kommunikationspfad DALI/DT8
3. Kanalregelung und CCT-Verhalten
4. Fault-Injection / Schutzpfade
5. Thermik- und Dauerlasttests

Für formale Kriterien den hardwareseitigen Bring-up/Testplan verwenden.

---

## Troubleshooting

- Kein Start nach Flash:
  - Takt-/Reset-Konfiguration und Build-Target prüfen
  - Versorgung und Brownout-Bedingungen verifizieren

- DALI ohne Reaktion:
  - Busverdrahtung und Master-Konfiguration kontrollieren
  - Implementierte DT8-Kommandos mit `firmware/docs/dali_implementation.md` abgleichen

- Instabiles Dimmverhalten:
  - Sollwertgrenzen und Kalibrierungen prüfen
  - Kanalströme und Lastkompatibilität messen

- Unerwartete Faults:
  - Schutzschwellen/Telemetrie sichten
  - Schritte aus `firmware/docs/troubleshooting.md` anwenden

---

## Sicherheitshinweise

- Nur durch qualifiziertes Fachpersonal in geeigneter Laborumgebung betreiben
- Strombegrenzung beim Erststart immer aktivieren
- Thermische Risiken bei Dauerlast berücksichtigen
- Isolations- und EMV-Anforderungen für Zielprodukt separat validieren

---

## Weiterführende Dokumentation

### Root

- `README.md` (dieses Dokument)

### Hardware

- `hardware/DALI_CCT_48V_30W/README.md`
- `hardware/DALI_CCT_48V_30W/docs/design-specification.md`
- `hardware/DALI_CCT_48V_30W/docs/bom.csv`
- `hardware/DALI_CCT_48V_30W/docs/net-class-and-layout-rules.md`
- `hardware/DALI_CCT_48V_30W/docs/bring-up-and-test-plan.md`
- `hardware/DALI_CCT_48V_30W/docs/open-items-and-risks.md`
- `hardware/DALI_CCT_48V_30W/docs/firmware-interface.md`

### Firmware

- `firmware/README.md`
- `firmware/docs/README.md`
- `firmware/docs/build.md`
- `firmware/docs/api.md`
- `firmware/docs/dali_implementation.md`
- `firmware/docs/troubleshooting.md`
