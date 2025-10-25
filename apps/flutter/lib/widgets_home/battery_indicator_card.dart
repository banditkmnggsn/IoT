import 'package:flutter/material.dart';
import 'package:percent_indicator/percent_indicator.dart';

class BatteryIndicatorCard extends StatelessWidget {
  final double batteryValue;

  const BatteryIndicatorCard({
    super.key,
    required this.batteryValue,
  });

  Color _getBatteryColor(double value) {
    if (value >= 60) {
      return Colors.green;
    } else if (value >= 30) {
      return Colors.orange;
    } else {
      return Colors.red;
    }
  }

  Color _getBatteryBackgroundColor(double value) {
    if (value >= 60) {
      return Colors.green.shade100;
    } else if (value >= 30) {
      return Colors.orange.shade100;
    } else {
      return Colors.red.shade100;
    }
  }

  @override
  Widget build(BuildContext context) {
    final batteryColor = _getBatteryColor(batteryValue);
    final backgroundColor = _getBatteryBackgroundColor(batteryValue);

    return Card(
      elevation: 4,
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            const Text(
              'Persentase Baterai',
              style: TextStyle(
                fontSize: 18,
                fontWeight: FontWeight.bold,
              ),
            ),
            const SizedBox(height: 16),
            CircularPercentIndicator(
              radius: 70.0,
              lineWidth: 15.0,
              percent: batteryValue / 100,
              center: Text(
                "$batteryValue%",
                style: TextStyle(
                  fontSize: 24,
                  fontWeight: FontWeight.bold,
                  color: batteryColor,
                ),
              ),
              progressColor: batteryColor,
              backgroundColor: backgroundColor,
              animation: true,
              animationDuration: 1000,
              circularStrokeCap: CircularStrokeCap.round,
            ),
          ],
        ),
      ),
    );
  }
}