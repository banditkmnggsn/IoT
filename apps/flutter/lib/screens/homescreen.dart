import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'dart:async'; // Tambahkan import untuk Timer
import '../widgets_home/battery_indicator_card.dart';
import '../widgets_home/moisture_indicator_card.dart';
import '../widgets_home/manual_button.dart';
import '../widgets_home/temperature_indicator_card.dart';
import '../screens/settings_page.dart';

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  bool isWatering = false;
  double batteryValue = 0; // Mulai dengan 0 agar tidak tampil nilai default
  double temperatureValue = 24; // Default
  double moistureValue = 65; // Default
  Timer? _refreshTimer; // Timer untuk refresh otomatis

  // Perbaikan path Firebase
  final DatabaseReference batteryRef =
      FirebaseDatabase.instance.ref("arduino_to_app/battery_percentage");
  final DatabaseReference temperatureRef =
      FirebaseDatabase.instance.ref("arduino_to_app/suhu");
  final DatabaseReference moistureRef =
      FirebaseDatabase.instance.ref("arduino_to_app/moisture_value");

  @override
  void initState() {
    super.initState();
    // Mulai pemantauan Firebase
    _setupFirebaseListeners();
    
    // Set timer untuk refresh data Firebase setiap 5 detik
    _refreshTimer = Timer.periodic(const Duration(seconds: 5), (timer) {
      _forceUpdateFromFirebase();
    });
  }
  
  @override
  void dispose() {
    // Batalkan timer saat widget dihancurkan
    _refreshTimer?.cancel();
    super.dispose();
  }

  Future<void> _setupFirebaseListeners() async {
    try {
      // 1. Periksa status koneksi Firebase
      FirebaseDatabase.instance.ref('.info/connected').onValue.listen((event) {
        final connected = event.snapshot.value as bool? ?? false;
        print('Status koneksi Firebase: ${connected ? "TERHUBUNG" : "TIDAK TERHUBUNG"}');
        if (mounted) {
          setState(() {
          });
        }
      });

      // Ambil data battery_percentage sekali di awal untuk nilai awal
      final batterySnapshot = await batteryRef.get();
      if (batterySnapshot.exists) {
        final rawValue = batterySnapshot.value;
        _updateBatteryValue(rawValue);
      }

      // 2. Memantau nilai battery_percentage
      batteryRef.onValue.listen((event) {
        final timestamp = DateTime.now().toString().substring(11, 23); // hh:mm:ss.sss
        
        if (event.snapshot.exists) {
          final dynamic rawValue = event.snapshot.value;
          print('[$timestamp] Data battery diterima: $rawValue (${rawValue.runtimeType})');
          _updateBatteryValue(rawValue);
        } else {
          print('[$timestamp] PERINGATAN: Snapshot battery tidak ada nilai!');
          if (mounted) {
            setState(() {
            });
          }
        }
      }, onError: (error) {
        print('ERROR pada listener battery: $error');
        if (mounted) {
          setState(() {
          });
        }
      });

      // 3. Memantau nilai suhu
      temperatureRef.onValue.listen((event) {
        if (event.snapshot.exists) {
          final dynamic rawValue = event.snapshot.value;
          if (rawValue != null) {
            double temp = 24;
            if (rawValue is double) {
              temp = rawValue;
            } else if (rawValue is int) {
              temp = rawValue.toDouble();
            } else if (rawValue is String) {
              temp = double.tryParse(rawValue) ?? 24;
            }
            if (mounted) {
              setState(() {
                temperatureValue = temp;
              });
            }
          }
        }
      });

      // 4. Memantau nilai kelembaban
      moistureRef.onValue.listen((event) {
        if (event.snapshot.exists) {
          final dynamic rawValue = event.snapshot.value;
          if (rawValue != null) {
            double moisture = 0;
            if (rawValue is double) {
              moisture = rawValue;
            } else if (rawValue is int) {
              moisture = rawValue.toDouble();
            } else if (rawValue is String) {
              moisture = double.tryParse(rawValue) ?? 0;
            }
            if (mounted) {
              setState(() {
                moistureValue = moisture;
              });
            }
          }
        }
      });

      // 5. Cek sekali saat startup untuk melihat struktur database
      _checkDatabaseStructure();
      
    } catch (e) {
      print('Error dalam setup Firebase: $e');
      if (mounted) {
        setState(() {
        });
      }
    }
  }

  void _updateBatteryValue(dynamic rawValue) {
    try {
      double parsedValue = 0;
      
      if (rawValue is double) {
        parsedValue = rawValue;
      } else if (rawValue is int) {
        parsedValue = rawValue.toDouble();
      } else if (rawValue is String) {
        parsedValue = double.tryParse(rawValue) ?? 0;
      } else {
        print('PERINGATAN: Tipe data battery tidak dikenal: ${rawValue.runtimeType}');
        return;
      }
      
      if (mounted) {
        setState(() {
          batteryValue = parsedValue.clamp(0.0, 100.0);
        });
      }
      print('Battery value updated to: $batteryValue');
    } catch (e) {
      print('ERROR parsing nilai battery: $e');
      if (mounted) {
        setState(() {
        });
      }
    }
  }

  void _checkDatabaseStructure() async {
    try {
      print('Memeriksa struktur database...');
      
      // Cek root database
      final rootSnapshot = await FirebaseDatabase.instance.ref().get();
      print('Root keys: ${rootSnapshot.children.map((c) => c.key).toList()}');
      
      // Cek node arduino_to_app
      final arduinoRef = FirebaseDatabase.instance.ref('arduino_to_app');
      final arduinoSnapshot = await arduinoRef.get();
      
      if (arduinoSnapshot.exists) {
        print('Node arduino_to_app ditemukan');
        print('Keys dalam arduino_to_app: ${arduinoSnapshot.children.map((c) => c.key).toList()}');
        
        if (arduinoSnapshot.hasChild('battery_percentage')) {
          final value = arduinoSnapshot.child('battery_percentage').value;
          print('battery_percentage ditemukan dengan nilai: $value (${value.runtimeType})');
          if (mounted) {
            setState(() {
              _updateBatteryValue(value); // Langsung update dengan nilai yang ditemukan
            });
          }
        } else {
          print('PERINGATAN: battery_percentage TIDAK ditemukan!');
          if (mounted) {
            setState(() {
            });
          }
        }
      } else {
        print('PERINGATAN: Node arduino_to_app TIDAK ditemukan!');
        if (mounted) {
          setState(() {
          });
        }
      }
    } catch (e) {
      print('ERROR memeriksa struktur database: $e');
      if (mounted) {
        setState(() {
        });
      }
    }
  }

  void _navigateToSettings() {
    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => const SettingsPage(),
      ),
    );
  }

  // Fungsi untuk memaksa pembaruan dari Firebase
  void _forceUpdateFromFirebase() async {
    try {
      // Pembaruan battery_percentage
      final batterySnapshot = await batteryRef.get();
      if (batterySnapshot.exists) {
        final value = batterySnapshot.value;
        _updateBatteryValue(value);
      }
      
      // Pembaruan suhu
      final tempSnapshot = await temperatureRef.get();
      if (tempSnapshot.exists && tempSnapshot.value != null) {
        final dynamic rawValue = tempSnapshot.value;
        double temp = 24;
        if (rawValue is double) {
          temp = rawValue;
        } else if (rawValue is int) {
          temp = rawValue.toDouble();
        } else if (rawValue is String) {
          temp = double.tryParse(rawValue) ?? 24;
        }
        if (mounted) {
          setState(() {
            temperatureValue = temp;
          });
        }
      }
      
      // Pembaruan moisture
      final moistureSnapshot = await moistureRef.get();
      if (moistureSnapshot.exists && moistureSnapshot.value != null) {
        final dynamic rawValue = moistureSnapshot.value;
        double moisture = 0;
        if (rawValue is double) {
          moisture = rawValue;
        } else if (rawValue is int) {
          moisture = rawValue.toDouble();
        } else if (rawValue is String) {
          moisture = double.tryParse(rawValue) ?? 0;
        }
        if (mounted) {
          setState(() {
            moistureValue = moisture;
          });
        }
      }
      
    } catch (e) {
      print('Error saat refresh otomatis: $e');
      if (mounted) {
        setState(() {
        });
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Smart Plant Watering'),
        elevation: 0,
        actions: [
          IconButton(
            icon: const Icon(Icons.settings),
            onPressed: _navigateToSettings,
            tooltip: 'Pengaturan',
          ),
        ],
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Debug Info Card - Tampilkan info Firebase untuk debugging
            Card(
              color: Colors.amber[100],
              child: Padding(
                padding: const EdgeInsets.all(8.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Row(
                      children: [
                        Icon(Icons.bug_report, color: Colors.amber[800]),
                        const SizedBox(width: 8),
                        const Text(
                          'Debug Info Firebase',
                          style: TextStyle(
                            fontWeight: FontWeight.bold,
                          ),
                        ),
                      ],
                    ),
                    const SizedBox(height: 8),
                    const Text('Auto refresh setiap 5 detik'),
                    ElevatedButton(
                      onPressed: _forceUpdateFromFirebase,
                      child: const Text('Refresh Data'),
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 20),

            // Status Cards
            Row(
              children: [
                Expanded(
                  child: TemperatureIndicatorCard(temperature: temperatureValue),
                ),
              ],
            ),
            const SizedBox(height: 20),

            // Battery and Moisture Indicators in Row
            Row(
              children: [
                // Battery Status - menggunakan nilai dari state
                Expanded(
                  child: BatteryIndicatorCard(batteryValue: batteryValue),
                ),
                const SizedBox(width: 16),
                // Moisture Indicator
                Expanded(
                  child: MoistureIndicatorCard(
                    moistureValue: moistureValue,
                  ),
                ),
              ],
            ),
            const SizedBox(height: 20),

            // Watering Control
            WateringControlCard(
              isWatering: isWatering,
              onWateringChanged: (bool value) {
                setState(() {
                  isWatering = value;
                });
              },
            ),
          ],
        ),
      ),
    );
  }
}