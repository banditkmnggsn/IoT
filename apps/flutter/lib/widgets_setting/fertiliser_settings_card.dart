import 'package:flutter/material.dart';

class FertiliserSettingsCard extends StatelessWidget {
  final bool autoFertilising;
  final double fertiliserAmount;
  final Function(bool) onAutoFertilisingChanged;
  final Function(double) onFertiliserAmountChanged;

  const FertiliserSettingsCard({
    super.key,
    required this.autoFertilising,
    required this.fertiliserAmount,
    required this.onAutoFertilisingChanged,
    required this.onFertiliserAmountChanged,
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
              'Pengaturan Pemupukan',
              style: TextStyle(
                fontSize: 18,
                fontWeight: FontWeight.bold,
              ),
            ),
            const SizedBox(height: 16),
            SwitchListTile(
              title: const Text('Pemupukan Otomatis'),
              subtitle: const Text(
                'Mengaktifkan pemupukan otomatis saat penyiraman',
              ),
              value: autoFertilising,
              onChanged: onAutoFertilisingChanged,
            ),
            const SizedBox(height: 16),
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'Jumlah Pupuk: ${fertiliserAmount.round()} ml',
                  style: const TextStyle(
                    fontSize: 16,
                    fontWeight: FontWeight.w500,
                  ),
                ),
                const Text(
                  'Jumlah pupuk yang akan dikeluarkan setiap pemupukan',
                  style: TextStyle(
                    color: Colors.grey,
                    fontSize: 14,
                  ),
                ),
                Slider(
                  value: fertiliserAmount,
                  min: 0,
                  max: 200,
                  divisions: 40,
                  label: '${fertiliserAmount.round()} ml',
                  onChanged: autoFertilising ? onFertiliserAmountChanged : null,
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}