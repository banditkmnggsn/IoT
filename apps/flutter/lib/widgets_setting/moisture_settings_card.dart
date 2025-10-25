import 'package:flutter/material.dart';

class MoistureSettingsCard extends StatelessWidget {
  final bool autoWatering;
  final double moistureThreshold;
  final Function(bool) onAutoWateringChanged;
  final Function(double) onMoistureThresholdChanged;

  const MoistureSettingsCard({
    super.key,
    required this.autoWatering,
    required this.moistureThreshold,
    required this.onAutoWateringChanged,
    required this.onMoistureThresholdChanged,
  });

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text(
              'Pengaturan Penyiraman',
              style: TextStyle(
                fontSize: 18,
                fontWeight: FontWeight.bold,
              ),
            ),
            const SizedBox(height: 16),
            SwitchListTile(
              title: const Text('Penyiraman Otomatis'),
              subtitle: const Text(
                'Mengaktifkan penyiraman otomatis berdasarkan kelembaban',
              ),
              value: autoWatering,
              onChanged: onAutoWateringChanged,
            ),
            const SizedBox(height: 16),
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'Batas Kelembaban: ${moistureThreshold.round()}%',
                  style: const TextStyle(
                    fontSize: 16,
                    fontWeight: FontWeight.w500,
                  ),
                ),
                const Text(
                  'Penyiraman akan dimulai ketika kelembaban di bawah nilai ini',
                  style: TextStyle(
                    color: Colors.grey,
                    fontSize: 14,
                  ),
                ),
                Slider(
                  value: moistureThreshold,
                  min: 0,
                  max: 100,
                  divisions: 100,
                  label: '${moistureThreshold.round()}%',
                  onChanged: autoWatering ? onMoistureThresholdChanged : null,
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}