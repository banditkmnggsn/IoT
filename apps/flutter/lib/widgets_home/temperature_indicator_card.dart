import 'package:flutter/material.dart';

class TemperatureIndicatorCard extends StatelessWidget {
  final double temperature;

  const TemperatureIndicatorCard({
    super.key,
    required this.temperature,
  });

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            const Icon(
              Icons.thermostat,
              color: Colors.orange,
              size: 32,
            ),
            const SizedBox(height: 8),
            const Text(
              'Suhu',
              style: TextStyle(
                fontWeight: FontWeight.w500,
              ),
            ),
            Text(
              '${temperature.round()}°C',
              style: const TextStyle(
                fontSize: 20,
                fontWeight: FontWeight.bold,
              ),
            ),
          ],
        ),
      ),
    );
  }
}