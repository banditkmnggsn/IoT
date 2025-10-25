import 'package:flutter/material.dart';

class SaveSettingsButton extends StatelessWidget {
  final VoidCallback onSave;

  const SaveSettingsButton({
    super.key,
    required this.onSave,
  });

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: double.infinity,
      child: ElevatedButton(
        onPressed: onSave,
        style: ElevatedButton.styleFrom(
          padding: const EdgeInsets.symmetric(vertical: 16),
        ),
        child: const Text('Simpan Pengaturan'),
      ),
    );
  }
}