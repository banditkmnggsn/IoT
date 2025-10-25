import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:iot_micon/widgets_setting/moisture_settings_card.dart'; // Path impor diperbaiki
import 'package:iot_micon/widgets_setting/save_settings_button.dart'; // Path impor diperbaiki
import 'package:iot_micon/widgets_setting/fertiliser_settings_card.dart'; // Path impor diperbaiki

class SettingsPage extends StatefulWidget {
  const SettingsPage({super.key});

  @override
  State<SettingsPage> createState() => _SettingsPageState();
}

class _SettingsPageState extends State<SettingsPage> {
  // Default values
  double moistureThreshold = 60.0;
  double fertiliserAmount = 50.0;
  bool autoWatering = true;
  bool autoFertilising = false;
  bool isLoading = true;
  String errorMessage = '';

  final DatabaseReference _database = FirebaseDatabase.instance.ref();

  @override
  void initState() {
    super.initState();
    _loadSettings();
  }

  // Memuat pengaturan dari Firebase
  Future<void> _loadSettings() async {
    setState(() {
      isLoading = true;
      errorMessage = '';
    });

    try {
      // Ambil nilai minimal_kelembaban
      final moistureSnapshot = await _database.child('app_to_arduino/minimal_kelembaban').get();
      
      // Ambil nilai value_pupuk
      final fertiliserSnapshot = await _database.child('app_to_arduino/value_pupuk').get();
      
      setState(() {
        // Periksa dan update nilai kelembaban
        if (moistureSnapshot.exists) {
          final value = moistureSnapshot.value;
          if (value != null) {
            int intValue = 0;
            
            if (value is int) {
              intValue = value;
            } else if (value is double) {
              intValue = value.round();
            } else if (value is String) {
              intValue = int.tryParse(value) ?? 0;
            }
            
            moistureThreshold = intValue.toDouble();
            autoWatering = intValue > 0; // Jika nilai > 0, berarti auto watering aktif
          }
        }
        
        // Periksa dan update nilai pupuk
        if (fertiliserSnapshot.exists) {
          final value = fertiliserSnapshot.value;
          if (value != null) {
            int intValue = 0;
            
            if (value is int) {
              intValue = value;
            } else if (value is double) {
              intValue = value.round();
            } else if (value is String) {
              intValue = int.tryParse(value) ?? 0;
            }
            
            fertiliserAmount = intValue.toDouble();
            autoFertilising = intValue > 0; // Jika nilai > 0, berarti auto fertilising aktif
          }
        }
        
        isLoading = false;
      });
    } catch (e) {
      setState(() {
        isLoading = false;
        errorMessage = 'Gagal memuat pengaturan: $e';
      });
      
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text('Gagal memuat pengaturan: $e'),
          behavior: SnackBarBehavior.floating,
        ),
      );
    }
  }

  void _saveSettings() async {
    try {
      // Tampilkan indikator loading
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(
          content: Text('Menyimpan pengaturan...'),
          duration: Duration(seconds: 1),
          behavior: SnackBarBehavior.floating,
        ),
      );

      // Jika penyiraman otomatis mati, set nilai minimal_kelembaban ke 0
      int updatedMoistureThreshold = autoWatering ? moistureThreshold.round() : 0;
      await _database.child('app_to_arduino/minimal_kelembaban').set(updatedMoistureThreshold);

      // Jika pemupukan otomatis mati, set nilai value_pupuk ke 0
      int updatedFertiliserAmount = autoFertilising ? fertiliserAmount.round() : 0;
      await _database.child('app_to_arduino/value_pupuk').set(updatedFertiliserAmount);

      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(
          content: Text('Pengaturan berhasil disimpan'),
          behavior: SnackBarBehavior.floating,
        ),
      );
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text('Gagal menyimpan pengaturan: $e'),
          behavior: SnackBarBehavior.floating,
        ),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Pengaturan'),
        elevation: 0,
        actions: [
          IconButton(
            icon: const Icon(Icons.refresh),
            onPressed: _loadSettings,
            tooltip: 'Muat ulang pengaturan',
          ),
        ],
      ),
      body: isLoading 
        ? const Center(child: CircularProgressIndicator())
        : errorMessage.isNotEmpty
          ? Center(
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  const Icon(Icons.error_outline, size: 48, color: Colors.red),
                  const SizedBox(height: 16),
                  Text(errorMessage, textAlign: TextAlign.center),
                  const SizedBox(height: 16),
                  ElevatedButton(
                    onPressed: _loadSettings,
                    child: const Text('Coba Lagi'),
                  ),
                ],
              ),
            )
          : SingleChildScrollView(
              padding: const EdgeInsets.all(16.0),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  MoistureSettingsCard(
                    autoWatering: autoWatering,
                    moistureThreshold: moistureThreshold,
                    onAutoWateringChanged: (value) {
                      setState(() {
                        autoWatering = value;
                      });
                    },
                    onMoistureThresholdChanged: (value) {
                      setState(() {
                        moistureThreshold = value;
                      });
                    },
                  ),
                  const SizedBox(height: 16),
                  FertiliserSettingsCard(
                    autoFertilising: autoFertilising,
                    fertiliserAmount: fertiliserAmount,
                    onAutoFertilisingChanged: (value) {
                      setState(() {
                        autoFertilising = value;
                      });
                    },
                    onFertiliserAmountChanged: (value) {
                      setState(() {
                        fertiliserAmount = value;
                      });
                    },
                  ),
                  const SizedBox(height: 24),
                  SaveSettingsButton(onSave: _saveSettings),
                ],
              ),
            ),
    );
  }
}