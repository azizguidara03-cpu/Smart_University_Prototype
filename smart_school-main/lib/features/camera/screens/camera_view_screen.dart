import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:video_player/video_player.dart';
import 'package:http/http.dart' as http;
import '../../../core/models/camera_model.dart';
import '../../../services/supabase_service.dart';

class CameraViewScreen extends StatefulWidget {
  final CameraModel camera;

  const CameraViewScreen({
    Key? key,
    required this.camera,
  }) : super(key: key);

  @override
  State<CameraViewScreen> createState() => _CameraViewScreenState();
}

class _CameraViewScreenState extends State<CameraViewScreen> {
  VideoPlayerController? _videoController;
  bool _isFullScreen = false;
  bool _showControls = true;
  bool _isInitializing = true;
  bool _hasError = false;
  
  // New SSE streaming properties
  String? _imageUrl;
  StreamSubscription? _streamSubscription;
  final _client = http.Client();
  bool _isSSEStream = false;

  @override
  void initState() {
    super.initState();
    _determineStreamType();
  }

  void _determineStreamType() {
    // Check if this is likely an SSE stream
    if (widget.camera.streamUrl.isEmpty) {
      setState(() {
        _isInitializing = false;
        _hasError = true;
      });
      return;
    }

    final isSSEStream = widget.camera.streamUrl.contains('events') || 
                        widget.camera.streamUrl.contains('sse') ||
                        widget.camera.streamUrl.contains('stream-events');

    final isVideoStream = widget.camera.streamUrl.contains('.m3u8') ||
                          widget.camera.streamUrl.contains('stream') ||
                          widget.camera.streamUrl.contains('.mp4');
    
    if (isSSEStream) {
      _isSSEStream = true;
      _connectToStream();
    } else if (isVideoStream) {
      _initializeVideoPlayer();
    } else {
      // Assume static image
      setState(() {
        _isInitializing = false;
      });
    }
  }

  void _initializeVideoPlayer() {
    _videoController = VideoPlayerController.network(
      widget.camera.streamUrl,
    );

    _videoController!.initialize().then((_) {
      if (mounted) {
        _videoController!.play();
        _videoController!.setLooping(true);
        setState(() {
          _isInitializing = false;
        });
      }
    }).catchError((error) {
      print("Video initialization error: $error");
      if (mounted) {
        setState(() {
          _isInitializing = false;
          _hasError = true;
        });
      }
    });
  }

  void _connectToStream() async {
    setState(() {
      _isInitializing = true;
      _hasError = false;
    });
    
    try {
      // Create a request to the stream URL
      final request = http.Request('GET', Uri.parse(widget.camera.streamUrl));
      request.headers['Cache-Control'] = 'no-cache';
      request.headers['Accept'] = 'text/event-stream';
      
      final response = await _client.send(request);
      
      // Handle the stream response
      _streamSubscription = response.stream
          .transform(utf8.decoder)
          .transform(const LineSplitter())
          .listen(
        (line) {
          if (line.startsWith('data:')) {
            final data = line.substring(5).trim();
            if (data != 'keep-alive') {
              if (mounted) {
                setState(() {
                  _imageUrl = data;
                  _isInitializing = false;
                });
              }
            }
          }
        },
        onError: (error) {
          print('Error with stream: $error');
          if (mounted) {
            setState(() {
              _hasError = true;
              _isInitializing = false;
            });
          }
          _reconnect();
        },
        onDone: () {
          print('Stream closed');
          _reconnect();
        },
      );
    } catch (e) {
      print('Failed to connect to stream: $e');
      if (mounted) {
        setState(() {
          _hasError = true;
          _isInitializing = false;
        });
      }
      _reconnect();
    }
  }

  void _reconnect() {
    Future.delayed(const Duration(seconds: 3), () {
      if (mounted) {
        _connectToStream();
      }
    });
  }

  @override
  void dispose() {
    _videoController?.dispose();
    _streamSubscription?.cancel();
    _client.close();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.black,
      body: Stack(
        children: [
          GestureDetector(
            onTap: () {
              setState(() {
                _showControls = !_showControls;
              });
            },
            child: Center(
              child: _isInitializing
                  ? const CircularProgressIndicator()
                  : _hasError
                      ? _buildErrorWidget()
                      : _buildStreamWidget(),
            ),
          ),
          if (_showControls) _buildControlsOverlay(),
        ],
      ),
    );
  }

  Widget _buildStreamWidget() {
    // Handle SSE image stream
    if (_isSSEStream && _imageUrl != null) {
      try {
        return Image.memory(
          base64Decode(_imageUrl!.split(',')[1]),
          fit: _isFullScreen ? BoxFit.cover : BoxFit.contain,
          gaplessPlayback: true, // Prevents flickering between frames
          errorBuilder: (context, error, stackTrace) {
            print("Error rendering image: $error");
            return _buildErrorWidget();
          },
        );
      } catch (e) {
        print("Error processing stream image: $e");
        return _buildErrorWidget();
      }
    }
    // Handle video stream
    else if (_videoController != null && _videoController!.value.isInitialized) {
      return AspectRatio(
        aspectRatio: _videoController!.value.aspectRatio,
        child: VideoPlayer(_videoController!),
      );
    } 
    // Handle static image
    else {
      return Image.network(
        widget.camera.streamUrl,
        fit: _isFullScreen ? BoxFit.cover : BoxFit.contain,
        errorBuilder: (context, error, stackTrace) {
          return _buildErrorWidget();
        },
      );
    }
  }

  Widget _buildErrorWidget() {
    return Container(
      color: Colors.black12,
      child: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const Icon(Icons.videocam_off, size: 64, color: Colors.white54),
            const SizedBox(height: 16),
            const Text(
              'Camera feed unavailable',
              style: TextStyle(color: Colors.white54, fontSize: 16),
            ),
            const SizedBox(height: 8),
            const Text(
              'Please check connection or try again later',
              style: TextStyle(color: Colors.white38, fontSize: 14),
            ),
            const SizedBox(height: 24),
            if (_isSSEStream)
              ElevatedButton(
                onPressed: _connectToStream,
                child: const Text('Retry Connection'),
              ),
          ],
        ),
      ),
    );
  }

  Widget _buildControlsOverlay() {
    return Column(
      children: [
        Container(
          padding: EdgeInsets.only(
            top: MediaQuery.of(context).padding.top + 8,
            bottom: 8,
            left: 16,
            right: 16,
          ),
          decoration: BoxDecoration(
            gradient: LinearGradient(
              begin: Alignment.topCenter,
              end: Alignment.bottomCenter,
              colors: [
                Colors.black.withOpacity(0.7),
                Colors.transparent,
              ],
            ),
          ),
          child: Row(
            children: [
              IconButton(
                icon: const Icon(Icons.arrow_back, color: Colors.white),
                onPressed: () => Navigator.pop(context),
              ),
              Expanded(
                child: Text(
                  widget.camera.name,
                  style: const TextStyle(
                    color: Colors.white,
                    fontSize: 18,
                    fontWeight: FontWeight.bold,
                  ),
                  overflow: TextOverflow.ellipsis,
                ),
              ),
            ],
          ),
        ),
        Spacer(),
        Container(
          padding: EdgeInsets.only(
            bottom: MediaQuery.of(context).padding.bottom + 8,
            top: 8,
            left: 16,
            right: 16,
          ),
          decoration: BoxDecoration(
            gradient: LinearGradient(
              begin: Alignment.bottomCenter,
              end: Alignment.topCenter,
              colors: [
                Colors.black.withOpacity(0.7),
                Colors.transparent,
              ],
            ),
          ),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              _buildControlButton(
                icon: _isFullScreen ? Icons.fullscreen_exit : Icons.fullscreen,
                label: 'Fullscreen',
                onPressed: () {
                  setState(() {
                    _isFullScreen = !_isFullScreen;
                  });
                },
              ),
              if (_videoController != null && !_isSSEStream)
                _buildControlButton(
                  icon: _videoController!.value.isPlaying
                      ? Icons.pause
                      : Icons.play_arrow,
                  label: _videoController!.value.isPlaying ? 'Pause' : 'Play',
                  onPressed: () {
                    setState(() {
                      if (_videoController!.value.isPlaying) {
                        _videoController!.pause();
                      } else {
                        _videoController!.play();
                      }
                    });
                  },
                ),
              if (_isSSEStream)
                _buildControlButton(
                  icon: Icons.refresh,
                  label: 'Reconnect',
                  onPressed: _connectToStream,
                ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildControlButton({
    required IconData icon,
    required String label,
    required VoidCallback onPressed,
  }) {
    return InkWell(
      onTap: onPressed,
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(icon, color: Colors.white),
          const SizedBox(height: 4),
          Text(label, style: const TextStyle(color: Colors.white, fontSize: 12)),
        ],
      ),
    );
  }
}