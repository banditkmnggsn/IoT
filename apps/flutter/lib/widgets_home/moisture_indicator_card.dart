import 'package:flutter/material.dart';
import 'package:percent_indicator/percent_indicator.dart';

class MoistureIndicatorCard extends StatelessWidget {
  final double moistureValue;

  const MoistureIndicatorCard({
    super.key,
    required this.moistureValue,
  });

  Color _getMoistureColor(double value) {
    if (value >= 60) {
      return Colors.green;
    } else if (value >= 30) {
      return Colors.orange;
    } else {
      return Colors.red;
    }
  }

  Color _getMoistureBackgroundColor(double value) {
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
    final moistureColor = _getMoistureColor(moistureValue);
    final backgroundColor = _getMoistureBackgroundColor(moistureValue);

    return Card(
      elevation: 4,
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            const Text(
              'Kelembaban Tanah',
              style: TextStyle(
                fontSize: 18,
                fontWeight: FontWeight.bold,
              ),
            ),
            const SizedBox(height: 16),
            CircularPercentIndicator(
              radius: 70.0,
              lineWidth: 15.0,
              percent: moistureValue / 100,
              center: Text(
                "${moistureValue.toStringAsFixed(1)}%",
                style: TextStyle(
                  fontSize: 24,
                  fontWeight: FontWeight.bold,
                  color: moistureColor,
                ),
              ),
              progressColor: moistureColor,
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