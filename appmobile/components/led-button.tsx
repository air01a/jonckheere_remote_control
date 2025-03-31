import React from 'react';
import { View, TouchableOpacity, StyleSheet, Text } from 'react-native';

const LedButton = ({isOn}:{isOn:boolean}) => {


  return (
    <View style={styles.container}>
      <TouchableOpacity
        style={[
          styles.led,
          { backgroundColor: isOn ? 'green' : 'red' }
        ]}
      >
      
      </TouchableOpacity>
    </View>
  );
};

export default LedButton;


const styles = StyleSheet.create({
    container: {
      flex: 1,
      justifyContent: 'center',
      alignItems: 'center',
    },
    led: {
      width: 30,
      height: 30,
      borderRadius: 30, // rond
      justifyContent: 'center',
      alignItems: 'center',
      elevation: 5, // ombre Android
      shadowColor: '#000', // ombre iOS
      shadowOpacity: 0.3,
      shadowRadius: 10,
    },
    text: {
      color: 'white',
      fontWeight: 'bold',
    },
  });