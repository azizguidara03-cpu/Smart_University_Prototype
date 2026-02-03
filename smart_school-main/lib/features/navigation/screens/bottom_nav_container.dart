import 'package:flutter/material.dart';
import '../../../features/dashboard/screens/dashboard_screen.dart';
import '../../../features/department/screens/department_list_screen.dart';
import '../../../features/security/screens/security_dashboard_screen.dart';
import '../../../features/settings/screens/settings_screen.dart';
import '../../../core/constants/app_constants.dart';

class BottomNavContainer extends StatefulWidget {
  final int initialIndex;
  
  const BottomNavContainer({
    Key? key,
    this.initialIndex = 0,
  }) : super(key: key);

  @override
  State<BottomNavContainer> createState() => _BottomNavContainerState();
}

class _BottomNavContainerState extends State<BottomNavContainer> {
  late int _currentIndex;
  late final List<Widget> _screens;
  
  @override
  void initState() {
    super.initState();
    _currentIndex = widget.initialIndex;
    _screens = const [
      DashboardScreen(),
      DepartmentListScreen(),
      SecurityDashboardScreen(),
      SettingsScreen(), // Add the settings screen
    ];
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: IndexedStack(
        index: _currentIndex,
        children: _screens,
      ),
      bottomNavigationBar: BottomNavigationBar(
        currentIndex: _currentIndex,
        onTap: (index) {
          setState(() {
            _currentIndex = index;
          });
        },
        selectedItemColor: AppColors.primary,
        unselectedItemColor: Colors.grey,
        type: BottomNavigationBarType.fixed, // Important for 4+ items
        items: const [
          BottomNavigationBarItem(
            icon: Icon(Icons.dashboard),
            label: 'Dashboard',
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.business),
            label: 'Departments',
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.security),
            label: 'Security',
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.settings),
            label: 'Settings',
          ),
        ],
      ),
    );
  }
}