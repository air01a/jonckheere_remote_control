import React, { useState, useEffect, useCallback, useRef } from 'react';
import { styles } from './styles/styles'; 
import { TimeSystem, Multiplier, Direction, CouButton, DecSpeedButton } from './types/buttons.type';
import { 
  View,  
  TouchableOpacity, 
  Text, 
  Alert, 
  ScrollView
} from 'react-native';
import { Buffer } from 'buffer';
import LedButton from './components/led-button';
import renderButtonRow from './components/button-row';
import Button from './components/button';
import { initUdpSocket, NETWORK_CONFIG } from './lib/udpsocket';


const UDPControlApp: React.FC = () => {
  const [selectedSolar, setSelectedSolar] = useState<TimeSystem | null>(null);
  const [selectedMultiplier, setSelectedMultiplier] = useState<Multiplier | null>(null);
  const [selectedDirection, setSelectedDirection] = useState<Direction | null>(null);
  const [selectedCou, setSelectedCou] = useState<CouButton | null>(null);
  const [selectedDecSpeed, setSelectedDecSpeed] = useState<DecSpeedButton | null>(null);
  const [receivedMessages, setReceivedMessages] = useState<string[]>([]);
  const [endCourse, setEndCourse] = useState<boolean>(false);
  const [isConnected, setIsConnected] = useState<boolean>(false);
  // Configuration réseau

  // Socket UDP pour l'envoi et la réception
  const [udpSocket, setUdpSocket] = useState<any>(null);
  const scrollViewRef =  useRef<ScrollView>(null);



  useEffect(() => {
    // Défile automatiquement vers la fin à chaque mise à jour des messages
    if (scrollViewRef.current) {
      scrollViewRef.current.scrollToEnd({ animated: true });
    }
  }, [receivedMessages]);

  const onUdpMessageCallBack = (msg: Buffer, rinfo: any) => {
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
  };
  // Initialisation du socket UDP
  useEffect(() => {
    // Créer le socket UDP
    const socket = initUdpSocket(onUdpMessageCallBack);

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
      setIsConnected(false);
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

  const handleDecSpeed = (speed: DecSpeedButton): void => {
    setSelectedDecSpeed(speed);
    sendUDPMessage({ 
      type: 'command', 
      command: 'decspeed',
      parameters: speed
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

  const connect = () => {
    sendUDPMessage({type: 'command', command:'connect', parameters:''});
  }

  // Rendu des boutons (identique aux versions précédentes)
  

  if (!isConnected) { 
    return (
      <View style={styles.container} >
        <Text style={styles.title}>Connectez vous à la lunette</Text>

        <Button options={['Connecter']} onPress={connect}/>
      </View>
    )
  }

  return (
    <View style={styles.container} >
          <View style={styles.rowContainer}>
            <Text style={styles.title}>Connected</Text>
            <LedButton isOn={true} />
            <Text style={styles.title}>End of track</Text>
            <LedButton isOn={!endCourse} />
          </View>
      {/* Boutons de contrôle (identiques aux versions précédentes) */}
      <Text style={styles.title}>AD Speed</Text>
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
      <Text style={styles.title}>Dec speed</Text>

      {renderButtonRow(
        ['slow', 'fast'], 
        selectedDecSpeed, 
        handleDecSpeed
      )}
      
      
      <Text style={styles.title}>Commands</Text>

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

      <Text style={styles.title}>Dome</Text>

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
        <Text style={styles.receivedMessagesTitle}>Received messages:</Text>
        <TouchableOpacity 
          style={styles.clearButton} 
          onPress={clearReceivedMessages}
        >
          <Text style={styles.clearButtonText}>Delete</Text>
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



export default UDPControlApp;