import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'dart:async';

class WateringControlCard extends StatefulWidget {
  final bool isWatering;
  final ValueChanged<bool> onWateringChanged;

  const WateringControlCard({
    super.key,
    required this.isWatering,
    required this.onWateringChanged,
  });

  @override
  State<WateringControlCard> createState() => _WateringControlCardState();
}

class _WateringControlCardState extends State<WateringControlCard> {
  bool isAutoMode = false;
  int wateringDuration = 5;
  Timer? countdownTimer;
  int remainingSeconds = 0;
  bool isTimerRunning = false;

  final DatabaseReference _database =
      FirebaseDatabase.instance.ref("app_to_arduino");

  @override
  void dispose() {
    countdownTimer?.cancel();
    super.dispose();
  }

  Future<void> updateWateringDuration(int duration) async {
    try {
      await _database.update({'lama_menyiram': duration});
      print('Durasi menyiram diperbarui di Firebase Realtime Database: $duration');
    } catch (e) {
      print('Gagal memperbarui Firebase: $e');
    }
  }

  /// 🔹 Fungsi untuk memulai timer
  void startTimer() {
    remainingSeconds = wateringDuration * 60;
    updateWateringDuration(wateringDuration); // ⬅️ Update ke Firebase saat mulai

    countdownTimer?.cancel();
    countdownTimer = Timer.periodic(const Duration(seconds: 1), (timer) {
      setState(() {
        if (remainingSeconds > 0) {
          remainingSeconds--;
          isTimerRunning = true;
        } else {
          timer.cancel();
          isTimerRunning = false;
          widget.onWateringChanged(false);
        }
      });
    });
  }

  /// 🔹 Fungsi untuk menampilkan durasi dalam format menit:detik
  String formatDuration(int seconds) {
    int minutes = seconds ~/ 60;
    int remainingSecs = seconds % 60;
    return '$minutes:${remainingSecs.toString().padLeft(2, '0')}';
  }

  void incrementDuration() {
    if (!isTimerRunning && wateringDuration < 30) {
      setState(() {
        wateringDuration++;
      });
    }
  }

  void decrementDuration() {
    if (!isTimerRunning && wateringDuration > 1) {
      setState(() {
        wateringDuration--;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 4,
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text(
              'Kontrol Manual',
              style: TextStyle(
                fontSize: 18,
                fontWeight: FontWeight.bold,
              ),
            ),
            const SizedBox(height: 20),
            
            // Timer Control Section
            Container(
              padding: const EdgeInsets.all(16),
              decoration: BoxDecoration(
                color: Colors.grey[100],
                borderRadius: BorderRadius.circular(12),
              ),
              child: Column(
                children: [
                  const Text(
                    'Atur Durasi',
                    style: TextStyle(
                      fontSize: 16,
                      fontWeight: FontWeight.w500,
                    ),
                  ),
                  const SizedBox(height: 12),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      IconButton(
                        icon: const Icon(
                          Icons.remove_circle_outline_outlined,
                          size: 50,
                          color: Colors.red,
                        ),
                        onPressed: isTimerRunning ? null : decrementDuration,
                      ),
                      Container(
                        width: 100,
                        padding: const EdgeInsets.symmetric(
                          horizontal: 16,
                          vertical: 8,
                        ),
                        decoration: BoxDecoration(
                          color: Colors.white,
                          borderRadius: BorderRadius.circular(8),
                          border: Border.all(color: Colors.grey[300]!),
                        ),
                        child: Center(
                          child: Text(
                            isTimerRunning
                                ? formatDuration(remainingSeconds)
                                : '$wateringDuration menit',
                            style: const TextStyle(
                              fontSize: 18,
                              fontWeight: FontWeight.bold,
                            ),
                          ),
                        ),
                      ),
                      IconButton(
                        icon: const Icon(
                          Icons.add_circle_outline_sharp,
                          size: 50,
                          color: Colors.green,
                        ),
                        onPressed: isTimerRunning ? null : incrementDuration,
                      ),
                    ],
                  ),
                  const SizedBox(height: 16),
                  ElevatedButton.icon(
                    style: ElevatedButton.styleFrom(
                      backgroundColor: widget.isWatering ? Colors.red : Colors.green,
                      padding: const EdgeInsets.symmetric(
                        horizontal: 24,
                        vertical: 12,
                      ),
                    ),
                    icon: Icon(
                      widget.isWatering ? Icons.stop : Icons.play_arrow,
                    ),
                    label: Text(
                      widget.isWatering ? 'Stop' : 'Mulai',
                      style: const TextStyle(fontSize: 16),
                    ),
                    onPressed: () {
                      if (!widget.isWatering) {
                        startTimer();
                      } else {
                        countdownTimer?.cancel();
                        setState(() {
                          isTimerRunning = false;
                          remainingSeconds = 0;
                        });
                      }
                      widget.onWateringChanged(!widget.isWatering);
                    },
                  ),
                ],
              ),
            ),
            
            // Status Section
            const SizedBox(height: 16),
            Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: widget.isWatering ? Colors.red[50] : Colors.green[50],
                borderRadius: BorderRadius.circular(8),
                border: Border.all(
                  color: widget.isWatering ? Colors.red[200]! : Colors.green[200]!,
                ),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Row(
                    children: [
                      Icon(
                        widget.isWatering ? Icons.water_drop : Icons.water_drop_outlined,
                        color: widget.isWatering ? Colors.red : Colors.green,
                      ),
                      const SizedBox(width: 8),
                      Text(
                        widget.isWatering ? 'Sedang Menyiram' : 'Siap',
                        style: TextStyle(
                          fontWeight: FontWeight.bold,
                          color: widget.isWatering ? Colors.red : Colors.green,
                        ),
                      ),
                    ],
                  ),
                  if (isTimerRunning) ...[
                    const SizedBox(height: 4),
                    Text(
                      'Sisa waktu: ${formatDuration(remainingSeconds)}',
                      style: const TextStyle(fontSize: 12),
                    ),
                  ],
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
