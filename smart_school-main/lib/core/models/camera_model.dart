import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';

class CameraModel {
  final int cameraId;
  final String name;
  final String streamUrl;
  final bool isActive;
  final int? deviceId;
  final int? classroomId;
  final String? classroomName;

  CameraModel({
    required this.cameraId,
    required this.name,
    required this.streamUrl,
    this.isActive = true,
    this.deviceId,
    this.classroomId,
    this.classroomName, required bool motionDetectionEnabled, required String description, required bool isRecording,
  });

  factory CameraModel.fromJson(Map<String, dynamic> json) {
    return CameraModel(
      cameraId: json['camera_id'] ?? json['id'] ?? 0,
      name: json['name'] ?? 'Unnamed Camera',
      streamUrl: json['stream_url'] ?? json['streamUrl'] ?? '',
      isActive: json['is_active'] ?? json['isActive'] ?? true,
      deviceId: json['device_id'] ?? json['deviceId'],
      classroomId: json['classroom_id'] ?? json['classroomId'],
      classroomName: json['classroom_name'] ?? json['classroomName'], motionDetectionEnabled: true, description: '', isRecording: true,
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'camera_id': cameraId,
      'name': name,
      'stream_url': streamUrl,
      'is_active': isActive,
      'device_id': deviceId,
      'classroom_id': classroomId,
    };
  }
}

class CameraProvider extends ChangeNotifier {
  bool _isFullscreen = false;
  bool _isRecording = false;
  bool _isPanning = false;
  bool _isLoading = false;
  double _zoomLevel = 1.0;
  String _recordingDuration = "00:00";
  Timer? _recordingTimer;
  int _recordingSeconds = 0;
  String? _errorMessage;
  dynamic _camera;

  bool get isLoading => _isLoading;
  String? get errorMessage => _errorMessage;
  dynamic get camera => _camera;

  bool get isFullscreen => _isFullscreen;
  bool get isRecording => _isRecording;
  bool get isPanning => _isPanning;
  double get zoomLevel => _zoomLevel;
  String get recordingDuration => _recordingDuration;

  Future<void> loadCamera(int cameraId) async {
    _isLoading = true;
    _errorMessage = null;
    notifyListeners();

    try {
      await Future.delayed(const Duration(seconds: 1));

      _camera = {
        'id': cameraId,
        'name': 'Camera $cameraId',
        'streamUrl': 'http://192.168.0.22:3000/stream',
        'isActive': true,
      };
      _isLoading = false;
    } catch (e) {
      _isLoading = false;
      _errorMessage = 'Failed to load camera: $e';
    }

    notifyListeners();
  }
}