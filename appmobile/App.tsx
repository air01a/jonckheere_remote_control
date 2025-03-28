import React, { useState, useEffect, useCallback, useRef } from 'react';
import { 
  View, 
  StyleSheet, 
  TouchableOpacity, 
  Text, 
  Alert, 
  ScrollView
} from 'react-native';
import dgram from 'react-native-udp';
import { Buffer } from 'buffer';

type TimeSystem = 'solar' | 'lunar' | 'sidereal';
type Multiplier = 'x1' | 'x2' | 'x4' | 'x16';
type Direction = 'up' | 'down' | 'left' | 'right' | 'stop';
type CouButton = 'left' | 'right' | 'stop';

interface UDPMessage {
  type: string;
  command: string;
  parameters: string;
}

const UDPControlApp: React.FC = () => {
  const [selectedSolar, setSelectedSolar] = useState<TimeSystem | null>(null);
  const [selectedMultiplier, setSelectedMultiplier] = useState<Multiplier | null>(null);
  const [selectedDirection, setSelectedDirection] = useState<Direction | null>(null);
  const [selectedCou, setSelectedCou] = useState<CouButton | null>(null);
  const [receivedMessages, setReceivedMessages] = useState<string[]>([]);

  // Configuration réseau
  const NETWORK_CONFIG = {
    SERVER_IP: '192.168.1.1',   // Émulateur Android
    SERVER_SEND_PORT: 4000,  // Port pour envoyer
    CLIENT_LISTEN_PORT: 4000// Port pour écouter
  };

  // Socket UDP pour l'envoi et la réception
  const [udpSocket, setUdpSocket] = useState<any>(null);
  const scrollViewRef =  useRef<ScrollView>(null);
  useEffect(() => {
    // Défile automatiquement vers la fin à chaque mise à jour des messages
    if (scrollViewRef.current) {
      scrollViewRef.current.scrollToEnd({ animated: true });
    }
  }, [receivedMessages]);
  // Initialisation du socket UDP
  useEffect(() => {
    // Créer le socket UDP
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

    // Gestion des messages reçus
    socket.on('message', (msg: Buffer, rinfo: any) => {
      try {
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
    });

    // Gestion des erreurs
    socket.on('error', (err) => {
      console.error('Erreur de socket UDP:', err);
      Alert.alert('Erreur de Socket', err.message);
    });

    // Sauvegarde du socket
    setUdpSocket(socket);

    // Nettoyage à la fermeture
    return () => {
      socket.close();
    };
  }, []);

  // Envoi de message UDP
  const sendUDPMessage = useCallback((data: UDPMessage): void => {
    if (!udpSocket) {
      Alert.alert('Erreur', 'Socket UDP non initialisé');
      return;
    }

    try {
      const message = Buffer.from(JSON.stringify(data), 'utf8');
      
      udpSocket.send(
        message, 
        0, 
        message.length, 
        NETWORK_CONFIG.SERVER_SEND_PORT, 
        NETWORK_CONFIG.SERVER_IP, 
        (err: Error) => {
            if (err) {
            console.error('Erreur lors de l\'envoi UDP:', err);
            Alert.alert('Erreur d\'envoi', err.message);
          } else {
            console.log('Message UDP envoyé:', data);
          }
        }
      );
    } catch (error) {
      console.error('Erreur de création de message UDP:', error);
      Alert.alert('Erreur', 'Impossible d\'envoyer le message UDP');
    }
  }, [udpSocket]);

  // Méthodes de gestion des boutons (identiques aux versions précédentes)
  const handleSolarPress = (type: TimeSystem): void => {
    setSelectedSolar(type);
    sendUDPMessage({ 
      type: 'command', 
      command: 'mode',
      parameters: type
    });
  };

  const handleMultiplierPress = (multiplier: Multiplier): void => {
    setSelectedMultiplier(multiplier);
    sendUDPMessage({ 
      type: 'command', 
      command: 'multiplier',
      parameters: multiplier
    });
  };

  const handleDirectionPress = (direction: Direction): void => {
    setSelectedDirection(direction);
    sendUDPMessage({ 
      type: 'command', 
      command: 'direction',
      parameters: direction
    });
  };

  const handleCouPress = (value: CouButton): void => {
    setSelectedCou(value);
    sendUDPMessage({ 
      type: 'command', 
      command: 'coupole',
      parameters: value
    });
  };

  // Effacement des messages reçus
  const clearReceivedMessages = () => {
    setReceivedMessages([]);
  };

  // Rendu des boutons (identique aux versions précédentes)
  const renderButtonRow = (
    options: string[], 
    selectedValue: string | null, 
    onPress: (value: any) => void
  ) => (
    <View style={styles.rowContainer}>
      {options.map((option) => (
        <TouchableOpacity
          key={option}
          style={[
            styles.button, 
            selectedValue === option && styles.selectedButton
          ]}
          onPress={() => onPress(option)}
        >
          <Text style={styles.buttonText}>{option}</Text>
        </TouchableOpacity>
      ))}
    </View>
  );
  

  return (
    <View style={styles.container}>
      {/* Boutons de contrôle (identiques aux versions précédentes) */}
      {renderButtonRow(
        ['solar', 'lunar', 'sidereal'], 
        selectedSolar, 
        handleSolarPress
      )}

      {renderButtonRow(
        ['x1', 'x2', 'x4', 'x16'], 
        selectedMultiplier, 
        handleMultiplierPress
      )}

      <View style={styles.directionalContainer}>
        <View style={styles.directionalRow}>
          <TouchableOpacity
            style={[
              styles.directionalButton, 
              selectedDirection === 'up' && styles.selectedButton
            ]}
            onPressIn={() => handleDirectionPress('up')}
            onPressOut={() => handleDirectionPress('stop')}
          >
            <Text style={styles.buttonText}>▲</Text>
          </TouchableOpacity>
        </View>
        <View style={styles.directionalMiddleRow}>
          <TouchableOpacity
            style={[
              styles.directionalButton, 
              selectedDirection === 'left' && styles.selectedButton
            ]}
            onPressIn={() => handleDirectionPress('left')}
            onPressOut={() => handleDirectionPress('stop')}
          >
            <Text style={styles.buttonText}>◀</Text>
          </TouchableOpacity>
          <TouchableOpacity
            style={[
              styles.directionalButton, 
              selectedDirection === 'right' && styles.selectedButton
            ]}
            onPressIn={() => handleDirectionPress('right')}
            onPressOut={() => handleDirectionPress('stop')}
          >
            <Text style={styles.buttonText}>▶</Text>
          </TouchableOpacity>
        </View>
        <View style={styles.directionalRow}>
          <TouchableOpacity
            style={[
              styles.directionalButton, 
              selectedDirection === 'down' && styles.selectedButton
            ]}
            onPressIn={() => handleDirectionPress('down')}
            onPressOut={() => handleDirectionPress('stop')}
          >
            <Text style={styles.buttonText}>▼</Text>
          </TouchableOpacity>
        </View>
      </View>


      <Text style={styles.buttonText}>Coupole</Text>

      <View style={styles.directionalContainer}>
        <View style={styles.directionalMiddleRow}>
          <TouchableOpacity
            style={[
              styles.directionalButton, 
              selectedCou === 'left' && styles.selectedButton
            ]}
            onPressIn={() => handleCouPress('left')}
            onPressOut={() => handleCouPress('stop')}
          >
            <Text style={styles.buttonText}>◀</Text>
          </TouchableOpacity>
          <TouchableOpacity
            style={[
              styles.directionalButton, 
              selectedCou === 'right' && styles.selectedButton
            ]}
            onPressIn={() => handleCouPress('right')}
            onPressOut={() => handleCouPress('stop')}
          >
            <Text style={styles.buttonText}>▶</Text>
          </TouchableOpacity>
        </View>
      </View>
      {/* Section des messages reçus */}
      <View style={styles.receivedMessagesContainer}>
        <Text style={styles.receivedMessagesTitle}>Messages Reçus:</Text>
        <TouchableOpacity 
          style={styles.clearButton} 
          onPress={clearReceivedMessages}
        >
          <Text style={styles.clearButtonText}>Effacer</Text>
        </TouchableOpacity>
        <ScrollView style={styles.receivedMessagesList} ref={scrollViewRef} >
          {receivedMessages.map((msg, index) => (
            <Text key={index} style={styles.receivedMessageText}>
              {msg}
            </Text>
          ))}
        </ScrollView>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  // ... (styles précédents)
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    padding: 20,
    backgroundColor: '#f0f0f0'
  },
  rowContainer: {
    flexDirection: 'row',
    marginVertical: 10,
    justifyContent: 'center'
  },
  button: {
    backgroundColor: '#e0e0e0',
    padding: 15,
    margin: 5,
    borderRadius: 10,
    minWidth: 70,
    alignItems: 'center'
  },
  selectedButton: {
    backgroundColor: '#4CAF50'
  },
  buttonText: {
    color: 'black',
    fontWeight: 'bold'
  },
  directionalContainer: {
    marginVertical: 10
  },
  directionalRow: {
    flexDirection: 'row',
    justifyContent: 'center'
  },
  directionalMiddleRow: {
    flexDirection: 'row',
    justifyContent: 'center'
  },
  directionalButton: {
    backgroundColor: '#e0e0e0',
    padding: 15,
    margin: 5,
    borderRadius: 10,
    width: 70,
    height: 70,
    alignItems: 'center',
    justifyContent: 'center'
  },
  // Nouveaux styles pour les messages reçus
  receivedMessagesContainer: {
    marginTop: 20,
    width: '100%',
    maxHeight: 200,
    borderTopWidth: 1,
    borderTopColor: '#ccc',
    paddingTop: 10,
  },
  receivedMessagesTitle: {
    fontSize: 16,
    fontWeight: 'bold',
    marginBottom: 10,
    textAlign: 'center',
  },
  receivedMessagesList: {
    maxHeight: 150,
  },
  receivedMessageText: {
    backgroundColor: '#f0f0f0',
    padding: 5,
    marginVertical: 2,
    borderRadius: 5,
  },
  clearButton: {
    alignSelf: 'flex-end',
    marginRight: 10,
    backgroundColor: '#ff4500',
    padding: 5,
    borderRadius: 5,
  },
  clearButtonText: {
    color: 'white',
    fontSize: 12,
  },
  pressReleaseButton: {
    backgroundColor: '#007bff',
    padding: 15,
    margin: 10,
    borderRadius: 10,
    minWidth: 100,
    alignItems: 'center'
  },
});

export default UDPControlApp;