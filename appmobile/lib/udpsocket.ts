import dgram from 'react-native-udp';

export const NETWORK_CONFIG = {
    SERVER_IP: '192.168.1.1',   // Émulateur Android
    SERVER_SEND_PORT: 4000,  // Port pour envoyer
    CLIENT_LISTEN_PORT: 4000// Port pour écouter
  };

export const initUdpSocket = (callback: (msg: Buffer, rinfo: any) => void) =>{


    const socket = dgram.createSocket({ 
      type: 'udp4', 
      reusePort: true,
      debug: __DEV__ 
    });


    // Écoute des messages entrants
    socket.bind(NETWORK_CONFIG.CLIENT_LISTEN_PORT);
    
    socket.on('listening', () => {
      const address = socket.address();
      console.log(`Socket UDP en écoute sur ${address.address}:${address.port}`);
    });


    socket.on('message',callback);
    // Gestion des messages reçus
    /*socket.on('message', (msg: Buffer, rinfo: any) => {
      try {
        if (!isConnected) setIsConnected(true);
        //convert msg to object with json
        const msgJson = JSON.parse(msg.toString('utf8'));
        if (msgJson.type === 'endCourse') {
          setEndCourse(msgJson.value);
        } else { 
          if (msgJson.type === 'connect') {
            setSelectedDecSpeed(msgJson.dec_speed);
            setSelectedMultiplier(msgJson.multiplier);
            setSelectedSolar(msgJson.time_system);
          }
        }

        const message = msg.toString('utf8');
        
        console.log('Message UDP reçu:', message);
        
        // Mise à jour de la liste des messages reçus
        setReceivedMessages(prev => [
          ...prev, 
          `From ${rinfo.address}:${rinfo.port} - ${message}`
        ]);
      } catch (error) {
        console.error('Erreur de traitement du message:', error);
      }
    });*/

    // Gestion des erreurs
    socket.on('error', (err) => {
      console.error('Erreur de socket UDP:', err);
      //Alert.alert('Erreur de Socket', err.message);
    });
    return socket;
}
