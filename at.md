// get nmea
AT+CGNSSPORTSWITCH = 0,1

AT+CGNSSPORTSWITCH = 0,0
// 1. Vérifications de base
AT+CGNSSPWR? // État alimentation GNSS
AT+CGPSSTATUS? // Statut du fix
AT+CSQ // Force signal réseau

// 2. Si pas de fix, activer les données brutes
AT+CGNSSTST = 1 // Flux NMEA continu
AT+CGNSSPORTSWITCH = 1,1 // Sortie sur UART

// 3. Alternative pour position
AT+CGPSINFO // Position directe